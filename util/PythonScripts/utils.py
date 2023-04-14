#-*- coding:utf-8 -*-

import math
import numpy as np

import gnss_time


### sat list of GPS
# BLOCK II-F
sat_list_II = ["G01","G03","G06","G08","G09","G10","G24","G25","G26","G27","G30","G32"]
# BLOCK III-A
sat_list_III = ["G04","G14","G18","G23"]

### sat list of BDS
# BDS-2G
sat_list_GEO =["C01","C02","C03","C04","C05"]
# BDS-2I
sat_list_IGSO =["C06","C07","C08","C09","C10","C13","C16"]
# BDS-2M
sat_list_MEO2 = ["C11","C12","C14"]
# BDS-3
sat_list_MEO3 = ["C"+str(i) for i in range(19,61)]

def sod2hms(sod):
    h=int(sod/3600.0)
    m=int((sod-h*3600.0)/60.0)
    s=sod-h*3600-m*60
    return h,m,s

def pointlist2xy(pl):
    xl=[]
    yl=[]
    for x,y in pl:
        xl.append(x)
        yl.append(y)
    return np.array(xl),np.array(yl)

def rms(nx):
    sum =0.0
    for num in nx:
        sum += num**2
    return math.sqrt(sum/len(nx))

def get_ifcb_data(filename,doy,data):
    with open(filename,"r") as myfile:
        epoch_time = 0
        for line in myfile:
            if line[0] in ("%","x"):
                continue
            line = line[:-1]
            line = line.split()
            if len(line)<3:
                continue
            if line[0] == "EPOCH-TIME":
                epoch_time = doy + float(line[2]) / 3600.0 / 24.0
                #epoch_time = float(line[2]) / 3600.0 
                continue
            sat = line[0]
            value = float(line[1])
            sat_data = data.setdefault(sat,[])
            sat_data.append([epoch_time,value])
    return data

def set_sat_block_type(sat,block_type):
    if sat[0] == "G":
        if "GPS-III" in block_type:
            sat_list_III.append(sat)
        elif "GPS-IIF" in block_type:
            sat_list_II.append(sat)
    elif sat[0] == "C":
        if "BDS-2G" in block_type:
            sat_list_GEO.append(sat)
        elif "BDS-2I" in block_type:
            sat_list_IGSO.append(sat)
        elif "BDS-2M" in block_type:
            sat_list_MEO2.append(sat)
        else:
            sat_list_MEO3.append(sat)

def init_sat_block_type(fileobj,t):
    svn2block={}
    # GPS
    global sat_list_II,sat_list_III
    sat_list_II=[]
    sat_list_III=[]
    # BDS
    global sat_list_GEO,sat_list_IGSO,sat_list_MEO2,sat_list_MEO3
    sat_list_GEO=[]
    sat_list_IGSO=[]
    sat_list_MEO2=[]
    sat_list_MEO3=[]

    for line in fileobj:
        if "+SATELLITE/IDENTIFIER" in line :
            line = fileobj.readline()
            while "-SATELLITE/IDENTIFIER" not in line:
                if line[0]!="*":
                    line = line.split()
                    svn2block[line[0]]=line[3]
                line = fileobj.readline()
        elif "+SATELLITE/PRN" in line:
            line = fileobj.readline()
            while "-SATELLITE/PRN " not in line:
                if line[0]!="*":
                    line = line.split()
                    # check time
                    t_beg = line[1].split(":")
                    t_beg = gnss_time.gnss_time(t_beg[0],t_beg[1])
                    if t_beg > t:
                        line = fileobj.readline()
                        continue
                    t_end = line[2].split(":")
                    if int(t_end[0])!=0:
                        t_end = gnss_time.gnss_time(t_end[0],t_end[1]) 
                        if t_end < t:
                            line = fileobj.readline()
                            continue
                    # check block type
                    set_sat_block_type(line[3],svn2block[line[0]])
                line = fileobj.readline()
    sat_list_II.sort()
    sat_list_III.sort()
    sat_list_GEO.sort()
    sat_list_IGSO.sort()
    sat_list_MEO2.sort()
    sat_list_MEO3.sort()

def init_sat_list(metadata_file,t):
    if metadata_file is None:
        print("using metadata from: https://files.igs.org/pub/station/general/igs_satellite_metadata.snx")
        import io
        import requests
        r = requests.get("https://files.igs.org/pub/station/general/igs_satellite_metadata.snx")
        f_in = io.StringIO(r.content.decode("utf-8","ignore"))
    else:
        print("using metadata from metadata_file")
        f_in = open(metadata_file,"r")
        
    init_sat_block_type(f_in,t)

if __name__=="__main__":
    init_sat_list(None,gnss_time.gnss_time(2021,1,1))
    print(sat_list_II)
    print(sat_list_III)
    print(sat_list_GEO)
    print(sat_list_IGSO)
    print(sat_list_MEO2)
    print(sat_list_MEO3)