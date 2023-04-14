#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Guolong Feng  2020-05-05 created                     * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    Download GNSS Navigation Files                     * |
# * Author   Guolong Feng, Wuhan University                     * |
# * Date     2020-05-05                                         * |
# * Description     python 3.*                                  * |
# *                                                             * |
# ----------------------------------------------------------------

import os 
import argparse
import gnss_time
import gnss_tools


parser = argparse.ArgumentParser(description="Download GNSS Navigation Files\
		When downloading files, try the following download source in order:"+\
		' '.join([item[0] for item in gnss_tools.DOMAIN_LIST]))
parser.add_argument('year',type=int,help="Year")
parser.add_argument('doy',type=int,help="Day of Year")
parser.add_argument('length',type=int,help="Length of Day")
parser.add_argument('dst',type=str,help="Save Path For Downloaded Navigation Files \
	If the save path does not exist, it will be created \
	(-YYYY- can be repleaced by 4-digit year; \
	-DDD- can be repleaced by 3-digit doy;\
	e.g. /nav_save_path/-YYYY--DDD-)")

def main():
    args = parser.parse_args()

    beg_t = gnss_time.gnss_time(args.year,args.doy)
    
    for iday in range(args.length):
        t = beg_t + iday 

        str_nav_p = f"BRDM00DLR_S_{t.str_year()}{t.str_doy()}0000_01D_MN.rnx.gz"
        
        # check save path exist or not
        str_daily_dst = gnss_tools.replace_YYYYDDD(args.dst,t.year(),t.doy())
        if not os.path.exists(f"{str_daily_dst}"):
            os.makedirs(f"{str_daily_dst}")
        
        if gnss_tools.ftp_list_download(gnss_tools.DOMAIN_LIST,f"/pub/gps/data/daily/{t.str_year()}/brdc/",str_daily_dst,str_nav_p):
            save_nav_p= f"brdm{t.str_doy()}0.{t.str_yr()}p"
            print(f'- Renamed {str_nav_p} to {save_nav_p}')
            gnss_tools.unzip_gzfile(os.path.join(str_daily_dst,str_nav_p),os.path.join(str_daily_dst,save_nav_p))

if __name__ == "__main__":
    main()
