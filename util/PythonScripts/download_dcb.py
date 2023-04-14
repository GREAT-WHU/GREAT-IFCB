#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Hongjie Zheng 2020-11-10 created                     * |
# *   -1.1 Hongjie Zheng 2022-07-12 using the gzip library      * |
# *   						for cross-platform decompression    * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    Download IGG DCB Files                             * |
# * Author   Hongjie Zheng, Wuhan University                    * |
# * Date     2020-11-10                                         * |
# * Description     python 3.*                                  * |
# *                                                             * |
# ----------------------------------------------------------------
import os 
import argparse
import gnss_time
import gnss_tools


parser = argparse.ArgumentParser(description="Donwnload IGG DCB Files. \
		When downloading files, try the following download source in order:"+\
		' '.join([item[0] for item in gnss_tools.DOMAIN_LIST]))
parser.add_argument('year',type=int,help ="Year")
parser.add_argument('doy',type=int,help="Day of Year")
parser.add_argument("length",type=int,help="Length of Days")
parser.add_argument("dst",type=str,help="Save Path For Downloaded DCB Files.\
	If the save path does not exist, it will be created \
	(-YYYY- can be repleaced by 4-digit year; \
	-DDD- can be repleaced by 3-digit doy;\
	e.g. /dcb_save_path/-YYYY--DDD-)")

	
if __name__ == "__main__":
	args =parser.parse_args()

	beg_t = gnss_time.gnss_time(args.year,args.doy)
	dst_file_list=[]
	for iday in range(args.length):
		t = beg_t+iday
		# check save path exist or not
		save_dir = gnss_tools.replace_YYYYDDD(args.dst,t.year(),t.doy())
		if not os.path.exists(save_dir):
			os.makedirs(save_dir)

		str_file = f"CAS0MGXRAP_{t.str_yeardoy()}0000_01D_01D_DCB.BSX.gz"
		str_dir = f"/pub/gps/products/mgex/dcb/{t.str_year()}"
		
		if gnss_tools.ftp_list_download(gnss_tools.DOMAIN_LIST,str_dir,save_dir,str_file):
			# add downloaded files
			dst_file_list.append(os.path.join(save_dir,str_file))
	
	for dst_f in dst_file_list:
		txt_f = dst_f[:-3]
		gnss_tools.unzip_gzfile(dst_f,txt_f)