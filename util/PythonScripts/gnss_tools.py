#-*- coding:utf-8 -*-

import os
import re
import shutil
import time
import gzip
import ftplib
from unittest import result
################################### CONSTANTS ###################################
DOMAIN_LIST = [
	("gdc.cddis.eosdis.nasa.gov",True), # CDDIS_DOMAIN,USE TLS
	("igs.gnsswhu.cn",False) 		    # WHU_DOMAIN
]


#################################### FTP Tool ####################################
# ===========================================================
# Purpose:
#     Download file from FTP site to local path
#
# Inputs:
#    domain:       Domain name of FTP site
#    serve_path:   FTP server path
#    local_path:   Local path
#    file_name:    Target file name
#    use_TLS:      True: use TLS protocol; Flase: not use TLS protocol
# ===========================================================
def ftp_download(domain,serve_path,local_path,file_name,use_TLS=False):

    FTP_OBJ =ftplib.FTP
    if use_TLS:
        FTP_OBJ=ftplib.FTP_TLS
    # Local Path exist or Not
    if not os.path.exists(local_path):
        os.makedirs(local_path)
    # Whether File exist in Local Path
    if os.path.exists(f"{local_path}/{file_name}"):
        # Compare local size and remote size
        with FTP_OBJ(domain,timeout=30) as myftp:
            myftp.login()
            myftp.cwd(serve_path)
            # Avoid the prompt ftplib.error_perm: 550 SIZE not allowed in ASCII
            myftp.voidcmd('TYPE I') 
            remote_size = myftp.size(file_name)
            local_size = os.path.getsize(f"{local_path}/{file_name}")
            if local_size != remote_size:
                print(f"- Local file is different with remote file: {file_name} ")
            else:
                print(f"- Download {file_name} Success[exists] ")
                return True

    time_beg = time.time()
    print(f"- Begin download file {file_name}... download source: {domain} ")
    try:
        with FTP_OBJ(domain,timeout=30) as myftp:
            myftp.login()
            serve_path
            myftp.cwd(serve_path)
            if use_TLS:
                myftp.prot_p()
            try:
                with open(f"{local_path}/{file_name}","wb") as myfile:
                    myftp.retrbinary("RETR "+file_name,myfile.write)
                    time_end = time.time()
                    print(f"- Download {file_name} Success! Spend: {time_end-time_beg:.3f}s ")
                return True
            except ftplib.all_errors as e:
                print(e)
                print(f"- Download {file_name} Fail! ")
                os.remove(f"{local_path}/{file_name}")
                return False
    except ftplib.all_errors:
        print(f"- Can't Connect Download {file_name} Fail!")
        return False

#################################### FTP Tool ####################################
# ===========================================================
# Purpose:
#     Download file from FTP site list to local path
#
# Inputs:
#    domain_list:  List of domain name of FTP site
#    serve_path:   FTP server path
#    local_path:   Local path
#    file_name:    Target file name
# ===========================================================
def ftp_list_download(domain_list,serve_path,local_path,file_name):
    for domain,use_tls in domain_list:
        if ftp_download(domain,serve_path,local_path,file_name,use_tls):
            return True
    print(f"- Can't Download {file_name} From All server!")
    return False

# ===========================================================
# Purpose:
#     Get filelist from FTP site
#
# Inputs:
#    domain:       Domain name of FTP site
#    serve_path:   FTP server path
#    use_TLS:      True: use TLS protocol; Flase: not use TLS protocol
#
# Outputs:
#    files map     Upper site name: long name MGEX RINEX observation files
#                  Lower site name: IGS RINEX observation files
# ===========================================================
def get_filelist_from_ftp(domain,serve_path,use_TLS=False):

    FTP_OBJ =ftplib.FTP
    if use_TLS:
        FTP_OBJ=ftplib.FTP_TLS

    try:
        with FTP_OBJ(domain,timeout=100) as myftp:
            myftp.login()
            myftp.cwd(serve_path)
            if use_TLS:
                myftp.prot_p()
            files = []
            try:
                files = myftp.nlst()
            except ftplib.all_errors as e:
                print(e)
                print(f"- No files in ftp server:{domain}, path:{serve_path} ")
            map_files = { f[:4]:f for f in files}
    except Exception:
        return {}

    return map_files

# ===========================================================
# Purpose:
#     Get filelist from FTP site
#
# Inputs:
#    domain_list:  Domain name of FTP site
#    serve_path:   FTP server path
#    use_TLS:      True: use TLS protocol; Flase: not use TLS protocol
#
# Outputs:
#    files map     Upper site name: long name MGEX RINEX observation files
#                  Lower site name: IGS RINEX observation files
# ===========================================================
def get_filelist_from_ftplist(domain_list,serve_path):
    print("Get file list...")
    results={}
    for domain,use_tls in domain_list:
        r = get_filelist_from_ftp(domain,serve_path,use_tls)
        for k,v in r.items():
            if k not in results:
                results[k] =v
    return results
    


#################################### File Tool ####################################
# ===========================================================
# Purpose:
#     Read Sitelist
#
# Inputs:
#    file_name:    Name of sitelist
#
# Outputs:
#    site_list in "list" form
#
# Formats:
#    -------------------------------------------
#    | areg
#    | gop7
#    | kir8
#    -------------------------------------------
# ===========================================================
def read_sitelist(filename):
    site_list = []
    with open(filename, "r") as myfile:
        for line in myfile:
            if re.search(r"^\s(\w{4})$", line) or re.search(r"^\s(\w{4})\s", line) or re.search(r"^(\w{4})$", line) or re.search(r"^(\w{4})\s+", line):
                site = line.split()[0]
                site_list.append(site.lower())
    site_list.sort()
    return site_list

# ===========================================================
# Purpose:
#     Unzip gz format files
#
# Inputs:
#    gzfile:     gz compressed filename
#    outputfile: output filename
#    remove:     [option] True: delete gzfile; False: save gzilfe
#
# Outputs:
#    string of new name
#
def unzip_gzfile(gzfile,outputfile,remove=True):
    with gzip.open(gzfile,"rb") as f_in:
        with open(outputfile,"wb") as f_out:
            shutil.copyfileobj(f_in,f_out)
    if remove:
        os.remove(gzfile)


#################################### String Tool ####################################
# ===========================================================
# Purpose:
#     Replace -YYYY- with 4-digit year, and replace -DDD- with DOY
#
# Inputs:
#    old_name:    string of old name 
#    year,doy:    year, day of year
#
# Outputs:
#    string of new name
# ===========================================================
def replace_YYYYDDD(old_name, year, doy):
    new_name = old_name.replace("-YYYY-",f"{year:04}")
    new_name = new_name.replace("-DDD-",f"{doy:03}")
    return new_name

# ===========================================================
# Purpose:
#     Convert Sys to System
#
# Inputs:
#    sys:    abbreviation of GNSS system (G/C/E/R)
#
# Outputs:
#    string of systems
# ===========================================================
### 
def sat_system(sys):
    process_sys = []
    for onesys in sys:
        if (onesys == "G"):
            process_sys.append("GPS")
        elif (onesys == "E"):
            process_sys.append("GAL")
        elif (onesys == "R"):
            process_sys.append("GLO")
        elif (onesys == "C"):
            process_sys.append("BDS")
    return " ".join(process_sys)
