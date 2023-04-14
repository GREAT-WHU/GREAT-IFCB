/**
*
* @verbatim
History
-1.0 bwang  2019-02-23  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gifcb.cpp
* @brief	Storage the ifcb files' data include wide-lane, narrow-lane and ex-wide-lane
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/

#include"gdata/gifcb.h"

using namespace std;
namespace great {

	t_ifcbrec::t_ifcbrec()
	{
		obj = " ";
		npoint = 0;
		value = 0.0;
		sigma = 1E4;
		isRef = false;
	}

	t_gifcb::t_gifcb(){
		gtrace("t_gifcb::constructor");
		id_type(t_gdata::IFCB);

	}


	t_gifcb::~t_gifcb()
	{
		_ifcb.clear();
	}

	void t_gifcb::add_sat_ifcb(t_gtime epoch, string prn, t_ifcbrec one_sat_ifcb)
	{
		_ifcb[epoch][prn] = make_shared<t_ifcbrec>(one_sat_ifcb);
	}

	void t_gifcb::add_epo_ifcb(t_gtime epoch, one_epoch_ifcb one_epo_ifcb){
		_ifcb[epoch] = one_epo_ifcb;
	}

	bool t_gifcb::ifcb_usable(const t_gtime& t, const string& str)
	{
		try
		{
			bool ifcb_usable;
			ifcb_usable = (_ifcb[t][str]->sigma < 0.100 && _ifcb[t][str]->npoint >= 3)
				|| _ifcb[t][str]->isRef || _ifcb[t][str]->sigma <= 1e-4;
			return ifcb_usable;
		}
		catch (...)
		{
			cout << "ERROR:t_gifcb::ifcb_usable wrong!!!" << endl;
			throw("t_gifcb::ifcb_usable is Wrong!");
		}
	}

	void t_gifcb::re_init_ifcb(const t_gtime& t, string str)
	{
		t_ifcbrec one_ifcb;
		one_ifcb.npoint = 0;
		one_ifcb.value = 0.0;
		one_ifcb.sigma = 1E4;
		one_ifcb.isRef = false;
		t_gifcb::add_sat_ifcb(t, str, one_ifcb);
	}

	void t_gifcb::reset_ifcb(const t_gtime& t, const string& str, const double& value,
		const double& sigma, const int& npoint)
	{
		_ifcb[t][str]->npoint = npoint;
		_ifcb[t][str]->value = value;
		_ifcb[t][str]->sigma = sigma;
	}

	void t_gifcb::reset_ifcb_value(const t_gtime& t, const string& str, const double& value)
	{
		_ifcb[t][str]->value = value;
	}

	void t_gifcb::copy_ifcb(const t_gtime& pre_t, const string& str, const t_gtime& current_t,
		const bool& is_first, const bool& is_site)
	{
		if (is_first == true)
		{
			t_gifcb::re_init_ifcb(current_t, str);
		}
		else
		{
			t_ifcbrec one_ifcb;
			one_ifcb.npoint = _ifcb[pre_t][str]->npoint;
			one_ifcb.value = _ifcb[pre_t][str]->value;
			one_ifcb.sigma = _ifcb[pre_t][str]->sigma;
			one_ifcb.isRef = _ifcb[pre_t][str]->isRef;
			if (is_site) _ifcb[pre_t].erase(str);
			t_gifcb::add_sat_ifcb(current_t, str, one_ifcb);

		}
		return;
	}

	void t_gifcb::set_valid_beg(const t_gtime& t)
	{
		if (_valid_beg > t) _valid_beg = t;
	};


}//namespace