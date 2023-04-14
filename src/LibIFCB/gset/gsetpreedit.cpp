/**
* 
* @verbatim
History
-1.0 hzjheng	2022-07-02 created
@endverbatim
* Copyright (c) 2020, Wuhan University. All rights reserved.
*
* @file			gsetturboedit.cpp
* @brief		preedit set
* @author       hjzheng, Wuhan University
* @version		1.0.0
* @date			2022-07-02
*
*/ 
#include <iomanip>
#include <sstream>
#include <algorithm>

#include "gset/gsetpreedit.h"

using namespace std;
using namespace pugi;


namespace great
{

	t_gsetpreedit::t_gsetpreedit()
	{
		_set.insert(XMLKEY_PREEDIT);
		_defaultMinimumEle = 7.0;	   // unit: degrees
	    _defaultMWLimit = 4.0;         // unit: cycle
	    _defaultGFLimit = 1.5;         // unit: meter
		_defaultGapArcLimit = 20;      // unit: epoch
		_defaultShortArcLimit = 10;    // unit: epoch
	}

	// Return value
	// ----------
	double t_gsetpreedit::minimum_elev()
	{
		_gmutex.lock();

		double tmp = _doc.child(XMLKEY_ROOT).child(XMLKEY_PREEDIT).child("minimum_elev").text().as_double();

		_gmutex.unlock(); return tmp;
	}

	bool t_gsetpreedit::checkMW(double& mw_limit)
	{
		xml_node tmp_set = _doc.child(XMLKEY_ROOT).child(XMLKEY_PREEDIT).child("check_mw");
		string tmp = tmp_set.attribute("mw_limit").value();

		if (tmp.empty())  mw_limit = _defaultMWLimit;
		else              mw_limit = str2dbl(tmp);

		return  tmp_set.attribute("valid").as_bool();
	}

	bool t_gsetpreedit::checkGF(double& gf_limit)
	{

		xml_node tmp_set = _doc.child(XMLKEY_ROOT).child(XMLKEY_PREEDIT).child("check_gf");
		string tmp_gf = tmp_set.attribute("gf_limit").value();
		string tmp_rms = tmp_set.attribute("gf_rms_limit").value();

		if (tmp_gf.empty()) gf_limit = _defaultGFLimit;
		else             gf_limit = str2dbl(tmp_gf);

		return  tmp_set.attribute("valid").as_bool();
	}

	bool t_gsetpreedit::checkGap(int& gap_limit)
	{
		xml_node tmp_set = _doc.child(XMLKEY_ROOT).child(XMLKEY_PREEDIT).child("check_gap");

		string tmp = tmp_set.attribute("gap_limit").value(); 

		if (tmp.empty())  gap_limit = _defaultGapArcLimit;
		else              gap_limit = str2int(tmp);

		return tmp_set.attribute("valid").as_bool();
	}

	bool t_gsetpreedit::checkShort(int& short_limit)
	{
		xml_node tmp_set = _doc.child(XMLKEY_ROOT).child(XMLKEY_PREEDIT).child("check_short");

		string tmp = tmp_set.attribute("short_limit").value();

		if (tmp.empty())  short_limit = _defaultShortArcLimit;
		else              short_limit = str2int(tmp);

		return tmp_set.attribute("valid").as_bool();
	}

	void t_gsetpreedit::help()
	{	
		cerr << "<turboedit  lite_mode=\"false\" >" << endl
			<< "<amb_output  valid=\"true\"  />  " << endl
			<< "<simulation  valid=\"false\" />  " << endl
			<< "<ephemeris   valid=\"true\"  />  " << endl
			<< "<check_pc   pc_limit=\"250\" valid=\"true\"  />" << endl
			<< "<check_mw   mw_limit=\"4\"   valid=\"true\"  />" << endl
			<< "<check_gf   gf_limit=\"1\"   gf_rms_limit=\"2\"   valid=\"true\" />" << endl
			<< "<check_sf   sf_limit=\"1\"   valid=\"false\" />         " << endl
			<< "<check_gap    gap_limit=\"20\"    valid=\"true\" />     " << endl
			<< "<check_short  short_limit=\"10\"  valid=\"true\" />     " << endl
			<< "<check_statistics  min_percent=\"60\"  min_mean_nprn=\"4\"  max_mean_namb=\"3\"  valid=\"true\" /> " << endl
			<< "</turboedit>" << endl
			<< endl;
		return;
	}

}