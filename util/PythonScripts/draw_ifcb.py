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
import math
import argparse
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

import utils
import gnss_time
import gnss_tools


parser = argparse.ArgumentParser(description="draw IFCB series script")
parser.add_argument('beg_year',type=str,help="beg year of IFCB products. (e.g. 2021)")
parser.add_argument('beg_doy',type=str,help="beg doy of IFCB products. (e.g. 001)")
parser.add_argument('days',type=str,help="days of IFCB products. (e.g. 10)")
parser.add_argument('system',type=str,help="satellite system of IFCB products. (e.g. G or GE or GCE)")
parser.add_argument("data_dir",type=str,help="directory of IFCB products. (-YYYY- can be repleaced by 4-digit year; -DDD- can be \
											 repleaced by 3-digit doy; e.g. /works/-YYYY--DDD-/ifcb_-YYYY--DDD-_G) ")
parser.add_argument("--metadata",type=str,default=None,help="absolute directory of satellite metadata file. Default is using the info from \
							https://files.igs.org/pub/station/general/igs_satellite_metadata.snx")											

def draw_ifcb_bds(data_ifcb,save_file_name):
	col = 4 
	row = 2
	# using sharex and shrey
	with plt.style.context(['science','no-latex','scatter','grid']):
		matplotlib.rc('font',size=10)
		# figsize(8,8)
		fig,ax_list= plt.subplots(row,col,sharex=True,sharey=True,figsize=(5.07,3))
		fig.subplots_adjust(wspace=0,hspace=0.9)
		## BDS-2
		sat_type = ["GEO","IGSO","MEO"]
		sat_type_list = [utils.sat_list_GEO,utils.sat_list_IGSO,utils.sat_list_MEO2]
		for i,title in enumerate(sat_type):
			ax = ax_list[0,i]   
			for sat in sat_type_list[i]:
				if data_ifcb.get(sat,None) is None:
					continue
				nx,ny = utils.pointlist2xy(data_ifcb[sat])
				ax.plot(nx,ny,marker='o',ms=0.7,label=sat)
			ax.autoscale(tight=True)
			ax.set_xlim([0,days])
			ax.set_ylim([-0.15,0.15])
			ax.set_xticks(np.linspace(0,days,2,endpoint=False))
			ax.set_yticks(np.linspace(-0.1,0.1,3))
			ax.legend(ncol=3,edgecolor="white",columnspacing=0,borderpad=0,handletextpad=0,handlelength=0.5,loc='upper center',bbox_to_anchor=(0.5,1.8),markerscale=4.0,fontsize=9)

		## BDS-3
		for i in range(4):
			ax = ax_list[1,i]
			for sat in utils.sat_list_MEO3[i*7:(i+1)*7]:
				if data_ifcb.get(sat,None) is None:
					continue
				nx,ny = utils.pointlist2xy(data_ifcb[sat])
				ax.plot(nx,ny,marker='o',ms=0.5,label=sat)
			ax.autoscale(tight=True)
			ax.set_xlim([0,days])
			ax.set_ylim([-0.15,0.15])
			ax.set_xticks(np.linspace(0,days,2,endpoint=False))
			ax.set_yticks(np.linspace(-0.1,0.1,3))
			ax.legend(ncol=3,edgecolor="white",columnspacing=0,borderpad=0.0,handletextpad=0.0,handlelength=0.5,loc='lower center',bbox_to_anchor=(0.5,1),markerscale=4.0,fontsize=9)
			

		ax_list[0,0].set_ylabel("PIFCB (m)")
		ax_list[1,0].set_ylabel("PIFCB (m)")
		fig.savefig(save_file_name,dpi=300)
	


def draw_ifcb_gal(data_ifcb,save_file_name):
	sat_list = [ sat for sat in data_ifcb.keys() if sat[0] == "E" ]
	col = 2 
	row = math.ceil(len(sat_list)/(col*7))
	# using sharex and shrey
	with plt.style.context(['science','no-latex','scatter','grid']):
		matplotlib.rc('font',size=10)
		fig,ax_list= plt.subplots(row,col,sharex=True,sharey=True,figsize=(5.07,3.2))
		fig.subplots_adjust(wspace=0,hspace=0.5)
		for r_i in range(row):
			ax_list[r_i,0].set_ylabel("PIFCB (m)")
			for c_i in range(col):
				i = r_i*col+c_i
				if (i >= len(sat_list)):
					continue
				ax = ax_list[r_i,c_i]
				for sat in sat_list[i*7:(i+1)*7]:
					if data_ifcb.get(sat,None) is None:
						continue
					nx,ny = utils.pointlist2xy(data_ifcb[sat])
					ax.plot(nx,ny,marker='o',ms=0.5,label=sat)
				ax.autoscale(tight=True)
				ax.set_xlim([0,days])
				ax.set_ylim([-0.15,0.15])
				ax.set_xticks(np.linspace(0,days,2,endpoint=False))
				ax.set_yticks(np.linspace(-0.1,0.1,3))
				ax.legend(ncol=4,edgecolor="white",columnspacing=0.1,borderpad=0.1,handletextpad=0.1,handlelength=0.7,loc='lower center',bbox_to_anchor=(0.5,1),markerscale=4.0)
		
		fig.savefig(save_file_name,dpi=300)
	


def draw_ifcb_gps(data_ifcb,days,save_file_name):
	sat_list = utils.sat_list_II+utils.sat_list_III
	col = 4
	row = math.ceil(len(sat_list)/col)
	# using sharex and shrey
	with plt.style.context(['science','no-latex','scatter','grid']):
		matplotlib.rc('font',size=10)
		fig,ax_list= plt.subplots(row,col,sharex=True,sharey=True,figsize=(7,4.5))
		fig.subplots_adjust(wspace=0,hspace=0)
		for r_i in range(row):
			for c_i in range(col):
				i = r_i*col+c_i
				if (i >= len(sat_list)):
					continue
				ax = ax_list[r_i,c_i]
				if data_ifcb.get(sat_list[i],None) is None:
					continue
				nx,ny = utils.pointlist2xy(data_ifcb[sat_list[i]])
				if r_i < 3:
					ax.plot(nx,ny,marker='o',ms=0.5,label=sat_list[i])
				else:
					ax.plot(nx,ny,marker='o',ms=0.5,label=sat_list[i],color="red")
				ax.autoscale(tight=True)
				ax.set_xlim((0,days))
				ax.set_ylim([-0.15,0.15])
				ax.set_xticks(np.linspace(0,days,2,endpoint=False))
				ax.set_yticks(np.linspace(-0.1,0.1,3))
				ax.legend(borderpad=0.2,handletextpad=0.4,handlelength=1.0,loc='lower left',markerscale=4.0)
		fig.text(0.02,0.45,"PIFCB (m)",rotation="vertical")
		fig.savefig(save_file_name,dpi=300)

if __name__=="__main__":
	args =parser.parse_args()
	ifcb_data = {}
	t = gnss_time.gnss_time(args.beg_year,args.beg_doy)
	utils.init_sat_list(args.metadata,t)
	data_dir = os.path.abspath(args.data_dir)
	days = int(args.days)
	for i in range(days):
		ifcb_dir = gnss_tools.replace_YYYYDDD(data_dir,t.year(),t.doy())
		utils.get_ifcb_data(ifcb_dir,i,ifcb_data)
		t = t+1
	if "G" in args.system:
		draw_ifcb_gps(ifcb_data,days,"ifcb_result_GPS.jpg")
	if "E" in args.system:
		draw_ifcb_gal(ifcb_data,"ifcb_result_GAL.jpg")
	if "C" in args.system:
		draw_ifcb_bds(ifcb_data,"ifcb_result_BDS.jpg")
