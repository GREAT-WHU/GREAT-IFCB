#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Guolong Feng  2020-05-05 created                     * |
# *   -1.1 Hongjie Zheng 2022-07-12 using the gzip library      * |
# *   						for cross-platform decompression    * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    Download GNSS Observation Files <MGEX, then IGS>   * |
# * Author   Guolong Feng, Wuhan University                     * |
# * Date     2020-05-05                                         * |
# * Description     python 3.*                                  * |
# *                                                             * |
# ----------------------------------------------------------------
from multiprocessing import set_forkserver_preload
import os
import sys
import platform
import argparse
import gnss_time
import gnss_tools

parser = argparse.ArgumentParser(description="Download GNSS Observation Files <priority: MGEX, then IGS>. \
		When downloading files, try the following download source in order:"+\
		' '.join([item[0] for item in gnss_tools.DOMAIN_LIST]))
parser.add_argument('year',type=int,help="Year")
parser.add_argument('doy',type=int,help="Day of Year")
parser.add_argument('length',type=int,help="Length of Days")
parser.add_argument('dst',type=str,help="Save Path For Downloaded Observation Files \
	If the save path does not exist, it will be created \
	(-YYYY- can be repleaced by 4-digit year; \
	-DDD- can be repleaced by 3-digit doy;\
	e.g. /obs_save_path/-YYYY--DDD-)")

parser.add_argument('sitelist',type=str,help="Site List File Path")

def main():
    args = parser.parse_args()

    CRX2RNX_EXE = f"{sys.path[0]}/crx2rnx"
    if platform.system() == "Linux": 
        os.system(f"chmod +x {CRX2RNX_EXE}")
   
    # get sitelist
    site_list = gnss_tools.read_sitelist(args.sitelist)
    
    beg_t = gnss_time.gnss_time(args.year,args.doy)
    # loop doy
    for iday in range(args.length):
        t = beg_t+iday
        # check save path exist or not
        str_daily_dst = gnss_tools.replace_YYYYDDD(args.dst,t.year(),t.doy()) 
        if not os.path.exists(f"{str_daily_dst}"):
            os.makedirs(f"{str_daily_dst}")

        # get ftp file list
        serve_path = f"/pub/gps/data/daily/{t.str_year()}/{t.str_doy()}/{t.str_yr()}d"
        ftplist = gnss_tools.get_filelist_from_ftplist(gnss_tools.DOMAIN_LIST,serve_path)
        # loop site
        for site in site_list:
            site_upper = site.upper()
            site_lower = site.lower()
        
            obs_name = f"{site_lower}{t.str_doy()}0.{t.str_yr()}o"
            str_obs = f"{str_daily_dst}/{obs_name}"

            # check obs file whether exist
            if os.path.exists(f"{str_obs}"): 
                print(f"{site} observation file alreaady exist.")
                continue

            # First MGEX
            if site_upper in ftplist.keys():
                if gnss_tools.ftp_list_download(gnss_tools.DOMAIN_LIST,serve_path,str_daily_dst,ftplist[site_upper]):
                    str_crx_gz = os.path.join(str_daily_dst,ftplist[site_upper]) 
                    str_crx = str_crx_gz[:-3]
                    str_rnx = str_crx[:-3] + "rnx"
                
                    gnss_tools.unzip_gzfile(str_crx_gz,str_crx)
                    os.system(f"{CRX2RNX_EXE} -f {str_crx}")
                    os.remove(str_crx)
                    print(f'- Renamed {ftplist[site_upper]} to {obs_name}')
                    os.rename(str_rnx,str_obs)
                    continue
            
            # Then IGS Obs
            if site_lower in ftplist.keys():
                if gnss_tools.ftp_list_download(gnss_tools.DOMAIN_LIST,serve_path,str_daily_dst,ftplist[site_lower]):
                    str_igs_dZ = os.path.join(str_daily_dst,ftplist[site_lower])
                    str_igs_d = str_igs_dZ[:-2]
                    gnss_tools.unzip_gzfile(str_igs_dZ,str_igs_d)
                    os.system(f"{CRX2RNX_EXE} -f {str_igs_d}") 
                    os.remove(str_igs_d)
    
if __name__ == "__main__":
    main()
