#-*- coding:utf-8 -*-

# ===========================================================
#     Initialization File (ini) Decoder
# ===========================================================
import configparser
import gnss_time

class GNSSconfig:
    def __init__(self,configfile=""):
        self.project_info = {}
        self.process_info = {}
        self.data_info = {}
        if configfile != "":
            cf = configparser.ConfigParser()
            cf.read(configfile)

            if cf.has_section("project"):
                self.project_info = dict(cf.items("project"))
            else:
                self.project_info = dict()
            
            if cf.has_section("data"):
                self.data_info = dict(cf.items("data"))
            else:
                self.data_info = dict()
# ===========================================================
#     Section project
# ===========================================================
    def ymd_beg(self):
        try:
            ymd_beg = self.project_info["ymd_beg"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the ymd_beg from config file!! Check the config file!!!")
            exit(1)
        return gnss_time.gnss_time(*ymd_beg.split("-"))

    def ymd_end(self):
        try:
            ymd_end = self.project_info["ymd_end"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the ymd_end from config file!! Check the config file!!!")
            exit(1)
        return gnss_time.gnss_time(*ymd_end.split("-"))

    def hms_beg(self):
        try:
            hms_beg = self.project_info["hms_beg"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the hms_beg from config file!! Check the config file!!!")
            exit(1)
        hms_beg = hms_beg.split(":") 
        str_hms_beg = hms_beg[0].zfill(2) + ":" + hms_beg[1].zfill(2) + ":" + hms_beg[2].zfill(2)
        return str_hms_beg

    def hms_end(self):
        try:
            hms_end = self.project_info["hms_end"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the hms_end from config file!! Check the config file!!!")
            exit(1)
        hms_end = hms_end.split(":") 
        str_hms_end = hms_end[0].zfill(2) + ":" + hms_end[1].zfill(2) + ":" + hms_end[2].zfill(2)
        return str_hms_end

    def satsys(self):
        return self.project_info.setdefault("satsys","G")

    def gps_band(self):
        return self.project_info.setdefault("gps_band","1 2 5")

    def gal_band(self):
        return self.project_info.setdefault("gal_band","1 5 7")

    def bds_band(self):
        return self.project_info.setdefault("bds_band","2 7 6")

    def sitelist(self):
        return self.project_info.setdefault("sitelist","site_list")

    def interval(self):
        return int(self.project_info.setdefault("interval","30"))

    def sat_rm(self):
        if not self.process_info.get("sat_rm",""):
            return self.process_info.setdefault("sat_rm","")
        else:
            sats = self.process_info["sat_rm"]
        if sats.strip() == "NONE": return ""      
        sats_remove = sats.split("+")
        str = " "
        for single in sats_remove:
            str += f"{single} "
        return str

    def work_dir(self):
        return self.project_info.setdefault("work_dir",".")

    def software(self):
        return self.project_info.setdefault("software","./GREAT-IFCB")

    def minimum_elev(self):
        return float(self.project_info.setdefault("minimum_elev",7.0))

    def mw_limit(self):
        return float(self.project_info.setdefault("mw_limit",3.0))

    def gf_limit(self):
        return float(self.project_info.setdefault("gf_limit",0.10))

    def gap_limit(self):
        return int(self.project_info.setdefault("gap_limit",20))

    def short_limit(self):
        return int(self.project_info.setdefault("short_limit",10))


# ===========================================================
#     Section data directory
# ===========================================================
    def obs_dir(self):
        return self.data_info["obs_dir"]

    def nav_dir(self):
        return self.data_info["nav_dir"]

    def dcb_dir(self):
        return self.data_info["dcb_dir"]

    def ambflag_dir(self):
        return self.data_info["ambflag_dir"]

    def ambflag13_dir(self):
        return self.data_info["ambflag13_dir"]

