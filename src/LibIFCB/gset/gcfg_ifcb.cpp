/**
*
* @verbatim
History
-1.0 hjzheng   2020-10-06 creat the file.
-1.1 hjzheng   2020-10-18 Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gcfg_ifcb.cpp
* @brief	   control set from XML
* @author      hjzheng, Wuhan University
* @version	   1.1.0
* @date		   2020-10-18
*
*/

#include "gset/gcfg_ifcb.h"

using namespace std;
using namespace pugi;
using namespace gnut;
namespace great
{
	t_gcfg_ifcb::t_gcfg_ifcb():
		t_gsetgen(),
		t_gsetinp(),
		t_gsetout(),
		t_gsetgnss()
	{
		_IFMT_supported.insert(RINEXO_INP);
		_IFMT_supported.insert(BIABERN_INP);
		_IFMT_supported.insert(RINEXN_INP);
		_IFMT_supported.insert(IFCB_INP);

		_OFMT_supported.insert(IFCB_OUT);
		_OFMT_supported.insert(LOG_OUT);
	}

	/** @brief default destructor. */
	t_gcfg_ifcb::~t_gcfg_ifcb()
	{

	}

	/** @brief settings check. */
	void t_gcfg_ifcb::check()
	{
		t_gsetgen::check();
		t_gsetinp::check();
		t_gsetout::check();
		t_gsetgnss::check();
	}

	/** @brief settings help. */
	void t_gcfg_ifcb::help()
	{
		t_gsetbase::help_header();
		t_gsetgen::help();
		t_gsetinp::help();
		t_gsetout::help();
		t_gsetgnss::help();
		t_gsetbase::help_footer();
	}



}