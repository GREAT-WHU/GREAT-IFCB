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
import gnss_config
import gnss_tools
import xml.etree.ElementTree as et
from xml.dom.minidom import parseString

parser = argparse.ArgumentParser(description="IFCB estimation batch-process script")
parser.add_argument('configfile',type=str,help="configure ini file")
args =parser.parse_args()

class xml_file:

    def __init__(self,root_tag):
        self.tree = et.ElementTree(et.Element(root_tag))
        self.root = self.tree.getroot()

    def set_node_text(self,node_list,text):
        father = self.root 
        for n in node_list:
            if father.find(n) is None:
                father.append(et.Element(n))
            father = father.find(n)

        father.text = f" {text} "

    def set_node_attribute(self,node_list,k,v):
        father = self.root
        for n in node_list:
            if father.find(n) is None:
                father.append(et.Element(n))
            father = father.find(n)

        father.attrib[str(k)]=str(v)

    def write(self,output_xmlfile):
        origin_string =et.tostring(self.root,encoding='utf-8')
        reparsed = parseString(origin_string)
        reparsed.toprettyxml(indent='\t')
        with open(output_xmlfile,"w") as f_out:
            f_out.write(reparsed.toprettyxml(indent='\t'))
        #self.tree.write(output_xmlfile)

class IFCB():
    ### Initialization
    def __init__(self,GNSSconfig):
        self.config = GNSSconfig                                     # Configure File [ini]
        self.workdir = os.path.abspath(GNSSconfig.work_dir())        # Working Directory
        self.runsoft = os.path.abspath(GNSSconfig.software())        # GREAT-IFCB Software Executable program Directory
        
        self.ymd_beg = GNSSconfig.ymd_beg()                          # Beg DOY
        self.ymd_end = GNSSconfig.ymd_end()                          # End DOY
        self.hms_beg = GNSSconfig.hms_beg()                          # Beg Hour/Min/Sec in one Day
        self.hms_end = GNSSconfig.hms_end()                          # End Hour/Min/Sec in one Day
        self.satsys = gnss_tools.sat_system(GNSSconfig.satsys())          # Processing System
        self.sitelist_file = os.path.abspath(GNSSconfig.sitelist())  # Site List File
        self.interval = GNSSconfig.interval()                        # Sampling Interval
        self.sat_rm = GNSSconfig.sat_rm()                            # Excluded Satellites


        # Preedit Settings
        self.minimum_elev = GNSSconfig.minimum_elev()
        self.mw_limit = GNSSconfig.mw_limit()
        self.gf_limit = GNSSconfig.gf_limit()
        self.gap_limit = GNSSconfig.gap_limit()
        self.short_limit = GNSSconfig.short_limit()

        self.site_list = gnss_tools.read_sitelist(self.sitelist_file)      # Read Site List
        
    ### Generate GNSS Files(RINEX-Obs+RINEX-Nav+DCB) List
    def __gen_filelist(self,t):

        ## Receiever List
        count = 0
        self.rec_list=" \n"
        for i,site in enumerate(self.site_list):
            if  (count+1) % 6 == 1:
                self.rec_list += "            "
            self.rec_list += f" {site.upper()} "
            if  (count+1) % 6 == 0:
                self.rec_list += "\n"
            count += 1
        self.rec_list += "      "
 
        ## Nav File
        navdir = gnss_tools.replace_YYYYDDD(self.config.nav_dir(),t.year(),t.doy())
        self.navfile = os.path.abspath(os.path.join(navdir,f"brdm{t.str_doy()}0.{t.str_yr()}p"))

        # DCB(IGG) File
        dcbdir = gnss_tools.replace_YYYYDDD(self.config.dcb_dir(),t.year(),t.doy())
        self.dcbfile = os.path.abspath(os.path.join(dcbdir,f"CAS0MGXRAP_{t.str_yeardoy()}0000_01D_01D_DCB.BSX"))

        ## Obs File
        count = 0
        self.obs_list=" \n"
        obsdir = gnss_tools.replace_YYYYDDD(self.config.obs_dir(),t.year(),t.doy())
        obsdir = os.path.abspath(obsdir)

        for i,site in enumerate(self.site_list):
            if  count == 0:
                self.obs_list += "               "

            self.obs_list += f" {os.path.join(obsdir,f'{site.lower()}{t.str_doy()}0.{t.str_yr()}o') } "

            if  count == 2:
                self.obs_list += "\n"

            count = (count+1)%3

    # Generate GREAT-FICB Config File
    def __gen_IFCB_XML(self,t,out_ifcb_file,xmlname):
        self.__gen_filelist(t)
        ifcb_xml = xml_file("config")
        
        ### general settings
        # beg time
        ifcb_xml.set_node_text(["gen","beg"],f" {t.str_year()}-{t.str_month()}-{t.str_day()} {self.hms_beg} ")
        # end time
        ifcb_xml.set_node_text(["gen","end"],f" {t.str_year()}-{t.str_month()}-{t.str_day()} {self.hms_end} ")
        # system 
        ifcb_xml.set_node_text(["gen","sys"]," " + self.satsys + " ")
        # site list
        ifcb_xml.set_node_text(["gen","rec"],self.rec_list)
        # interval
        ifcb_xml.set_node_text(["gen","int"]," " + str(self.interval) + " ") 
        # excluded sats
        ifcb_xml.set_node_text(["gen","sat_rm"],self.sat_rm)
    
        ### inputs settings
        # nav RINEX
        ifcb_xml.set_node_text(["inputs","rinexn"]," " + self.navfile + " ")
        # obs RINEX
        ifcb_xml.set_node_text(["inputs","rinexo"],self.obs_list)
        # DCB 
        ifcb_xml.set_node_text(["inputs","biabern"],self.dcbfile)

        # GNSS settings
        sys_list=["G","C","E"]
        tag_list=["gps","bds","gal"]
        func_list=[self.config.gps_band,self.config.bds_band,self.config.gal_band]
        for sys,tag,func in zip(sys_list,tag_list,func_list):
            if sys in self.config.satsys():
                ifcb_xml.set_node_text([tag,"band"],func())
                ifcb_xml.set_node_text([tag,"freq"],"1 2 3")

        # Proc settings
        ifcb_xml.set_node_attribute(["process"],"bds_cod_bias_corr","true")

        # Preedit settings
        ifcb_xml.set_node_text(["preedit","minimum_elev"],self.minimum_elev)
        ifcb_xml.set_node_attribute(["preedit","check_mw"],"mw_limit",self.mw_limit)
        if self.mw_limit >0 :
            ifcb_xml.set_node_attribute(["preedit","check_mw"],"valid","true")
        else:
            ifcb_xml.set_node_attribute(["preedit","check_mw"],"valid","false")

        ifcb_xml.set_node_attribute(["preedit","check_gf"],"gf_limit",self.gf_limit)
        if self.gf_limit >0 :
            ifcb_xml.set_node_attribute(["preedit","check_gf"],"valid","true")
        else:
            ifcb_xml.set_node_attribute(["preedit","check_gf"],"valid","false")

        ifcb_xml.set_node_attribute(["preedit","check_gap"],"gap_limit",self.gap_limit)
        if self.gap_limit >0 :
            ifcb_xml.set_node_attribute(["preedit","check_gap"],"valid","true")
        else:
            ifcb_xml.set_node_attribute(["preedit","check_gap"],"valid","false")

        ifcb_xml.set_node_attribute(["preedit","check_short"],"short_limit",self.short_limit)
        if self.short_limit >0 :
            ifcb_xml.set_node_attribute(["preedit","check_short"],"valid","true")
        else:
            ifcb_xml.set_node_attribute(["preedit","check_short"],"valid","false")

        ## outputs settings
        ifcb_xml.set_node_attribute(["outputs"],"append","false")
        ifcb_xml.set_node_attribute(["outputs"],"verb","0")

        # log IFCB estimation
        ifcb_xml.set_node_text(["outputs","log"]," LOGRT.log ")
        # output ifcb file
        ifcb_xml.set_node_text(["outputs","ifcb"],f" {out_ifcb_file} ")

    
        # Write xml file
        ifcb_xml.write(xmlname)

    def process(self):
        t = self.ymd_beg
        while t <= self.ymd_end:
            print("Process IFCB estimation of",t.year(),t.month(),t.day())
            # Create work directory
            CurrentDir = gnss_tools.replace_YYYYDDD(self.workdir,t.year(),t.doy())
            if not os.path.exists(f"{CurrentDir}"):
                os.makedirs(f"{CurrentDir}")
            
            print (f"---------------------------------------------------------------- ")
            print (f" Working   Path : {CurrentDir}")
        
            # Create IFCB XML File
            xmlfilename=os.path.join(CurrentDir,"ifcb.xml")
            out_ifcb_file=f"ifcb_{t.str_yeardoy()}_{''.join(self.config.satsys().split(' '))}"
            self.__gen_IFCB_XML(t,out_ifcb_file,xmlfilename)
            print (f" Product of IFCB XML : {xmlfilename} ")           

            # Run GREAT-IFCB executable program
            print(f" Runing GREAT-IFCB")
            os.chdir(CurrentDir)
            print(f" {self.runsoft} -x {xmlfilename}")
            os.system(f"{self.runsoft} -x {xmlfilename}")

            print(" Process finish")
           
            ## next day
            t = t+1

if __name__ == "__main__":
    # Parse the config ini file
    cfg = gnss_config.GNSSconfig(args.configfile)

    # Initialize IFCB class
    runIFCB = IFCB(cfg)

    # Run IFCB
    runIFCB.process()