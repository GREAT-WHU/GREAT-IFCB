/**
*
* @verbatim
History
-1.0 bwang  2019-02-25  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gambflag.cpp
* @brief	Storage the XXXXddd0.yyo.log/log13 files' data(only one site)
*				XXXX  ---- SITE name
*				 ddd  ---- Doy of the file
*				  yy  ---- year
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/

#include"gdata/gambflag.h"

using namespace std;
namespace great {

	t_gambflag::t_gambflag()
	{
		gtrace("t_gambflag::constructor");
		id_type(t_gdata::AMBFLAG);
	}

	t_gambflag::~t_gambflag()
	{
		gtrace("t_gambflag::destructor");
	}

	void t_gambflag::setAmbFlagHead(ambflag_hd head)
	{
		gtrace("t_gambflag::set_ambflag_head");
		_ambflag_head = make_shared<ambflag_hd>(head);
	}

	shared_ptr<ambflag_hd>& t_gambflag::getAmbFlagHead()
	{
		return _ambflag_head;
	}

	void t_gambflag::addAmbFlagData(string prn, const ambflag_data& data)
	{
		gtrace("t_gambflag::add_ambflag_data");
		try	
		{
			
			_gmutex.lock();
			_all_sats_ambflag[prn].push_back(make_shared<ambflag_data>(data));
			_gmutex.unlock();
		}
		catch (exception ex)
		{
			std::cout << ex.what();
		}
	}

	t_map_sat_ambflag& t_gambflag::getAmbFlagData()
	{
		return _all_sats_ambflag;
	}

	bool t_gambflag::isValid(const string& prn, const t_gtime& time, int& pos)
	{
		pos = 0;
		if (_all_sats_ambflag.find(prn) == _all_sats_ambflag.end())
		{
			return false;
		}
		t_gtime beg;
		t_gtime end;

		// find in unused
		for (auto iter = _all_sats_ambflag[prn].begin(); iter != _all_sats_ambflag[prn].end(); iter++)
		{
			beg = _ambflag_head->beg_time + _ambflag_head->interval * ((*iter)->beg_epo - 1);
			end = _ambflag_head->beg_time + _ambflag_head->interval * ((*iter)->end_epo - 1);
			if (time >= beg && time <= end) {
				if ((*iter)->identify == "BAD" || (*iter)->identify == "DEL") {
					return false;
				}
				else {
					pos = distance(_all_sats_ambflag[prn].begin(), iter);
					return true;
				}
			}
		}
		return false;
	}

	void t_gambflag::reset_iflag(const string& prn, const string& flag, const int& pos)
	{ 
		_all_sats_ambflag[prn][pos]->iflag = flag;
	}

	set<string> t_gambflag::getAllSatSet()
	{
		set<string> satlist;
		auto it_sat = _all_sats_ambflag.begin();
		for (; it_sat != _all_sats_ambflag.end(); it_sat++)
		{
			satlist.insert(it_sat->first);
		}
		return satlist;
	}



}//namespace