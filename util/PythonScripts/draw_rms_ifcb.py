#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Hongjie Zheng  2020-11-10 created                    * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    batch process script for IFCB estimation           * |
# * Author   Hongjie Zheng, Wuhan University                    * |
# * Date     2020-11-10                                         * |
# * Description     python 3.*                                  * |
# *                                                             * |
# ----------------------------------------------------------------

import os
import argparse
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

import gnss_time
import gnss_tools
from utils import *

parser = argparse.ArgumentParser(description="draw RMS of IFCB series script")
parser.add_argument('beg_year',type=str,help="beg year of IFCB products. (e.g. 2021)")
parser.add_argument('beg_doy',type=str,help="beg doy of IFCB products. (e.g. 001)")
parser.add_argument('days',type=str,help="days of IFCB products. (e.g. 10)")
parser.add_argument('system',type=str,help="satellite system of IFCB products. (e.g. G or GE or GCE)")
parser.add_argument("data_dir",type=str,help="directory of IFCB products. (-YYYY- can be repleaced by 4-digit year; -DDD- can be \
											 repleaced by 3-digit doy; e.g. /works/-YYYY--DDD-/ifcb_-YYYY--DDD-_G) ")
parser.add_argument("--metadata",type=str,default=None,help="absolute directory of satellite metadata file. Default is using the info from \
							https://files.igs.org/pub/station/general/igs_satellite_metadata.snx")											



def draw_ifcb_rms_gps(data_ifcb,save_file_name):
	# using sharex and shrey
	with plt.style.context(['science','no-latex','grid']):
		matplotlib.rc('font',size=10)
		fig,ax= plt.subplots(1,1,figsize=(5.07,1.6))
		fig.subplots_adjust(wspace=0,hspace=0.5)
		# GPS BLOCK II
		nx = []
		ny = []
		for k,v in data_ifcb.items():
			if k not in sat_list_II:
				continue
			nx.append(k)
			ny.append(rms(pointlist2xy(v)[1])*100)
		ax.bar(range(1,len(ny)+1),ny,label="GPS BLOCK II")
		# GPS BLOCK III
		nx = []
		ny = []
		for k,v in data_ifcb.items():
			if k not in sat_list_III:
				continue
			nx.append(k)
			ny.append(rms(pointlist2xy(v)[1])*100)
		ax.bar(range(len(sat_list_II)+1,len(ny)+len(sat_list_II)+1),ny,label="GPS BLOCK III",color="red")
		ax.set_xticks(range(1,len(sat_list_II+sat_list_III)+1))
		ax.set_xticklabels(sat_list_II+sat_list_III,rotation='vertical')
		ax.set_ylim(0,6)
		ax.set_yticks(np.linspace(0,6,4))
		if len(ny)>0:
			ax.legend()
		ax.set_ylabel("RMS (cm)")
		fig.savefig(save_file_name,dpi=300)

def draw_ifcb_rms_gal(data_ifcb,save_file_name):
	with plt.style.context(['science','no-latex','grid']):
		matplotlib.rc('font',size=10)
		fig,ax= plt.subplots(1,1,figsize=(5.07,1.6))
		fig.subplots_adjust(wspace=0,hspace=0.5)
		# GAL
		nx = []
		ny = []
		for k,v in data_ifcb.items():
			if k[0] != "E":
				continue
			rms_value = rms(pointlist2xy(v)[1])*100
			if rms_value == 0:
				continue
			nx.append(k)
			ny.append(rms_value)
		ax.bar(range(1,len(nx)+1),ny,label="Galileo")
		ax.set_xticks(range(1,len(nx)+1))
		ax.set_xticklabels(nx,rotation="vertical")
		ax.set_ylim(0,6)
		ax.set_yticks(np.linspace(0,6,4))
		if len(ny)>0:
			ax.legend()

		ax.set_ylabel("RMS (cm)")
		fig.savefig(save_file_name,dpi=300)

def draw_ifcb_rms_bds(data_ifcb,save_file_name):
	# using sharex and shrey
	with plt.style.context(['science','no-latex','grid']):
		matplotlib.rc('font',family='sans-serif',size=10)
		row = 2
		col = 1
		fig,ax= plt.subplots(row,col,figsize=(5.07,3.2))
		fig.subplots_adjust(wspace=0,hspace=0.5)
		# BDS GEO
		nx = []
		ny = []
		for k,v in data_ifcb.items():
			if k not in sat_list_GEO:
				continue
			nx.append(k)
			ny.append(rms(pointlist2xy(v)[1])*100)
		ax[0].bar(range(1,len(sat_list_GEO)+1),ny,label="BDS-2 GEO")
		# BDS IGSO
		nx = []
		ny = []
		for k,v in data_ifcb.items():
			if k not in sat_list_IGSO:
				continue
			nx.append(k)
			ny.append(rms(pointlist2xy(v)[1])*100)
		ax[0].bar(range(len(sat_list_GEO)+1,len(sat_list_GEO+sat_list_IGSO)+1),ny,label="BDS-2 IGSO",color="red")
		# BDS MEO
		nx = []
		ny = []
		for k,v in data_ifcb.items():
			if k not in sat_list_MEO2:
				continue
			nx.append(k)
			ny.append(rms(pointlist2xy(v)[1])*100)
		ax[0].bar(range(len(sat_list_GEO+sat_list_IGSO)+1,len(sat_list_GEO+sat_list_IGSO)+len(ny)+1),ny,label="BDS-2 MEO",color="orange")
		ax[0].set_xlim(0,len(sat_list_GEO+sat_list_IGSO+sat_list_MEO2)+1)
		ax[0].set_xticks(range(1,len(sat_list_GEO+sat_list_IGSO+sat_list_MEO2)+1))
		ax[0].set_xticklabels(sat_list_GEO+sat_list_IGSO+sat_list_MEO2,rotation='vertical')
		ax[0].set_ylim(0,6)
		ax[0].set_yticks(np.linspace(0,6,4))
		if len(ny) >0 :
			ax[0].legend(ncol=1,borderpad=0.2,handletextpad=0.4,handlelength=2.0,labelspacing=0.2,loc='upper right')

		# BDS-3 
		nx = []
		ny = []
		for k,v in data_ifcb.items():
			if k not in sat_list_MEO3:
				continue
			rms_value = rms(pointlist2xy(v)[1])*100
			if rms_value == 0:
				continue
			nx.append(k)
			ny.append(rms_value)
		ax[1].bar(range(1,len(nx)+1),ny,label="BDS-3")
		ax[1].set_xticks(range(1,len(nx)+1))
		ax[1].set_xticklabels(nx,rotation="vertical")
		ax[1].set_ylim(0,6)
		ax[1].set_yticks(np.linspace(0,6,4))
		if len(ny)>0:
			ax[1].legend(ncol=1,borderpad=0.2,handletextpad=0.4,handlelength=2.0,labelspacing=0.2,loc='upper right')

		#fig.text(0.05,0.4,"RMS (cm)",rotation="vertical")
		ax[0].set_ylabel("RMS (cm)")
		ax[1].set_ylabel("RMS (cm)")
		fig.savefig(save_file_name,dpi=300)

if __name__=="__main__":
	args =parser.parse_args()
	ifcb_data = {}
	t = gnss_time.gnss_time(args.beg_year,args.beg_doy)
	init_sat_list(args.metadata,t)
	data_dir = os.path.abspath(args.data_dir)
	days = int(args.days)
	for i in range(days):
		ifcb_dir = gnss_tools.replace_YYYYDDD(data_dir,t.year(),t.doy())
		get_ifcb_data(ifcb_dir,i,ifcb_data)
		t = t+1
	if "G" in args.system:
		draw_ifcb_rms_gps(ifcb_data,"ifcb_rms_GPS.jpg")
	if "E" in args.system:
		draw_ifcb_rms_gal(ifcb_data,"ifcb_rms_GAL.jpg")
	if "C" in args.system:
		draw_ifcb_rms_bds(ifcb_data,"ifcb_rms_BDS.jpg")
