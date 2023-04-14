
/* ----------------------------------------------------------------------
 * G-Nut - GNSS software development library 
 * 
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  This file is part of the G-Nut C++ library.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 3 of
  the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses>.

-*/

#include <stdlib.h>
#include <iostream>
#include <iomanip>

#include "gall/gallbias.h"

using namespace std;

namespace gnut
{

	// constructor
	// ----------
	t_gallbias::t_gallbias()
		: _overwrite(false)
	{
		id_type(t_gdata::ALLBIAS);
		id_group(t_gdata::GRP_MODEL);

		_ac_order["COD_A"] = 1;
		_ac_order["CAS_A"] = 2;
		_ac_order["WHU_A"] = 3;
		_ac_order["DLR_A"] = 4;
		_ac_order["CAS_R"] = 5;
		_ac_order["COD_R"] = 6;
		_ac_order["WHU_R"] = 7;
		_ac_order["DLR_R"] = 8;
	}


	// destructor
	// ----------
	t_gallbias::~t_gallbias() {
		_mapbias.clear();
	}


	// add satellite bias
	// ----------
	void t_gallbias::add(const string& ac, const t_gtime& epo, const string& obj, t_spt_bias pt_cb)
	{
#ifdef BMUTEX   
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_gmutex.lock();

		if (pt_cb == nullptr) {
			_gmutex.unlock();
			return;
		}

		if (pt_cb->ref() == X) {     // When ref GOBS is X => bias is expressed in absolute sense
			_mapbias[ac][epo][obj][pt_cb->gobs()] = pt_cb;
		}
		else
		{                     // Differential biases need to be splitted
			if (_mapbias[ac][epo][obj].size() == 0)
			{
				shared_ptr<t_gbias> pt_ref = make_shared<t_gbias>();                            // create first reference bias 
				pt_ref->set(pt_cb->beg(), pt_cb->end(), 0.0, pt_cb->ref(), pt_cb->ref());       // reference bias is set up to zero
				_mapbias[ac][epo][obj][pt_ref->gobs()] = pt_ref;                                // store new bias (reference)
				_mapbias[ac][epo][obj][pt_cb->gobs()] = pt_cb;                                 // store new bias
			}
			else
			{
				t_spt_bias pt_obs1 = _find(ac, epo, obj, pt_cb->gobs());
				t_spt_bias pt_obs2 = _find(ac, epo, obj, pt_cb->ref());
				if (pt_obs1 != nullptr && pt_obs2 == nullptr)
				{         // connection with first signal
					_connect_first(pt_obs1, pt_cb);
					_mapbias[ac][epo][obj][pt_cb->gobs()] = pt_cb;      // store modified bias
				}
				else if (pt_obs1 == nullptr && pt_obs2 != nullptr)
				{   // connection with second signal
					_connect_second(pt_obs2, pt_cb);
					_mapbias[ac][epo][obj][pt_cb->gobs()] = pt_cb;      // store modified bias
				}
				else if (pt_obs1 != nullptr && pt_obs2 != nullptr)
				{
					// connectin two groups with different reference signal
					// WARNING!!! - this case has not been tested (not happen in tested files)
					// connection with first signal
					_connect_first(pt_obs1, pt_cb);
					// all biases connected with second signal need to be consolidated
					_consolidate(ac, obj, pt_cb, pt_obs2);
				}
				else
				{
					shared_ptr<t_gbias> pt_ref = make_shared<t_gbias>();                            // create first reference bias 
					pt_ref->set(pt_cb->beg(), pt_cb->end(), 0.0, pt_cb->ref(), pt_cb->ref());       // reference bias is set up to zero
					_mapbias[ac][epo][obj][pt_ref->gobs()] = pt_ref;                                // store new bias (reference)
					_mapbias[ac][epo][obj][pt_cb->gobs()] = pt_cb;                                 // store new bias
				}
			}
		}

		_gmutex.unlock(); return;
	}

	// get undifferenced bias element
	// Note the variable 'meter' means in the original unit: meter for RTCM format, cycle for SGG format
	// ------------------------------
	double t_gallbias::get(const string prd, const t_gtime& epo, const string& prn, const GOBS& gobs, const bool meter) {
#ifdef BMUTEX   
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_gmutex.lock();

		double bias = 999.0;
		auto itAC = _mapbias.find(prd);
		if (itAC != _mapbias.end()) {
			auto itEPO = itAC->second.upper_bound(epo);
			//auto itEOP0 = itEPO;
			//if (itEOP0 != itAC->second.end() && fabs(itEOP0->first - epo) < fabs(itEPO->first - epo)) itEPO = itEOP0;	//nearst one
			if (itEPO != itAC->second.begin() && itEPO != itAC->second.begin()) itEPO--;  // between epochs
			if (itEPO == itAC->second.end() && itAC->second.size() != 0) itEPO--;         // no epochs

			if (itEPO != itAC->second.end()) {
				auto itSAT = itEPO->second.find(prn);
				if (itSAT != itEPO->second.end() && itSAT->second.find(gobs) != itSAT->second.end()) {
					t_spt_bias pobs1 = itSAT->second.find(gobs)->second;
					bias = pobs1->bias();
				}
			}
		}

		_gmutex.unlock();
		return bias;
	}

	string t_gallbias::ac_priority()
	{
		string used_ac;
		int    loc = 999;
		const map<string, int> ac_order{
				{"COD_A",1}, {"CAS_A",2}, {"WHU_A",3}, {"DLR_A",4},
				{"CAS_R",5}, {"COD_R",6}, {"WHU_R",7}, {"DLR_R",8}
		};
		for (auto item : _mapbias)
		{
			string ac = (item.first == "WHU_PHASE") ? "WHU" : item.first;
			if (ac_order.at(ac) < loc)
			{
				used_ac = ac;
				loc = ac_order.at(item.first);
			}
		}
		return used_ac;
	}

	string t_gallbias::get_used_ac()
	{
		if (_used_ac.empty())  _used_ac = ac_priority();
		return _used_ac;
	}

	// get single code bias
	// -------------------------------
	double t_gallbias::get(const t_gtime& epo, const string& obj, const GOBS& gobs1, const GOBS& gobs2, string ac) {

#ifdef BMUTEX   
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_gmutex.lock();

		double dcb = 0.0;

		if (ac == "" && _isOrdered == true) ac = _pri_ac;

		if (ac == "" && _isOrdered == false) 
		{
			int loc = 999;
			for (const auto& item : _mapbias) 
			{
				if (_ac_order.at(item.first) < loc) 
				{
					ac = item.first;
					loc = _ac_order.at(item.first);
				}
			}

			_isOrdered = true;
			_pri_ac = ac;
		}

		bool found = false;
		if (gobs2 == gobs1)
		{
			GOBS gobs1_convert = gobs1;
			this->_convert_obstype(ac, obj, gobs1_convert);
			t_spt_bias pobs1 = _find(ac, epo, obj, gobs1_convert);
			if (pobs1 != nullptr) 
			{ 
				found = true; dcb = pobs1->bias(); 
			}
		}
		else
		{
			GOBS gobs1_convert = gobs1;
			GOBS gobs2_convert = gobs2;

			// align obstype to ac(code or cas)
			this->_convert_obstype(ac, obj, gobs1_convert);
			this->_convert_obstype(ac, obj, gobs2_convert);

			t_spt_bias pobs1 = _find(ac, epo, obj, gobs1_convert);
			t_spt_bias pobs2 = _find(ac, epo, obj, gobs2_convert);


			if (pobs1 != nullptr && pobs2 != nullptr && pobs1->ref() == pobs2->ref())
			{
				found = true;
				dcb = pobs1->bias() - pobs2->bias();
			}
		}
		if (!found && _log) 
		{
			// string dcb_str = "(" + gobs2str(gobs1) + "-" + gobs2str(gobs2) + ")";
			// _log->comment(1, "gallbias", epo.str_mjdsod("WARNING: code bias " + obj + "  " + dcb_str + " not found!"));
		}

		_gmutex.unlock();
		return dcb;
	}

	// get single code bias
	// -------------------------------
	t_spt_bias t_gallbias::_find(const string& ac, const t_gtime& epo, const string& obj, const GOBS& gobs)
	{
		t_spt_bias pt_bias = nullptr;

		auto itAC = _mapbias.find(ac);
		if (itAC != _mapbias.end()) {
			auto itEPO = itAC->second.upper_bound(epo);
			if (itEPO != itAC->second.begin() && itEPO != itAC->second.begin()) itEPO--;  // between epochs
			if (itEPO == itAC->second.end() && itAC->second.size() != 0) itEPO--;         // no epochs

			if (itEPO != itAC->second.end()) {
				auto itOBJ = itEPO->second.find(obj);
				if (itOBJ != itEPO->second.end()) {
					auto itGOBS = itOBJ->second.find(gobs);
					if (itGOBS != itOBJ->second.end()) {
						if (itGOBS->second->valid(epo)) {
							pt_bias = itGOBS->second;
						}
					}
				}
			}
		}

		return pt_bias;
	}

	// get all biases with particular reference singal
	// -------------------------------
	vector<t_spt_bias> t_gallbias::_find_ref(const string& ac, const t_gtime& epo, const string& obj, const GOBS& ref)
	{
		vector<t_spt_bias> vec_bias;

		auto itAC = _mapbias.find(ac);
		if (itAC != _mapbias.end()) {
			auto itEPO = itAC->second.find(epo);
			if (itEPO != itAC->second.end()) {
				auto itOBJ = itEPO->second.find(obj);
				if (itOBJ != itEPO->second.end()) {
					for (auto itGOBS = itOBJ->second.begin(); itGOBS != itOBJ->second.end(); itGOBS++) {
						if (itGOBS->second->ref() == ref) vec_bias.push_back(itGOBS->second);
					}
				}
			}
		}

		return vec_bias;
	}

	void t_gallbias::_convert_obstype(const string& ac, const string& obj, GOBS& obstype)
	{
		if (ac == "COD_R")
		{
			// GPS GLO
			if (obj[0] == 'G' || obj[0] == 'R' || obj[0] == '2' || obj[0] == '3' || obj[0] == '4') {
				switch (obstype)
				{
				case C1C:obstype = C1; break;
				case C1P:
				case C1Y:
				case C1W:obstype = P1; break;
				case C2C:obstype = C2; break;
				case C2P:
				case C2Y:
				case C2W:obstype = P2; break;
				default:
					break;
				}
			}
		}
		else if (ac == "CAS_R")
		{
			// GPS
			if (obj[0] == 'G' || obj[0] == '2' || obj[0] == '3' || obj[0] == '4')
			{
				switch (obstype)
				{
				case P1:obstype = C1W; break;
				case P2:obstype = C2W; break;
				case C1:obstype = C1C; break;
				case C2:obstype = C2C; break;
				default:
					break;
				}
			}
			// GLO
			if (obj[0] == 'R')
			{
				switch (obstype)
				{
				case P1:obstype = C1P; break;
				case P2:obstype = C2P; break;
				case C1:obstype = C1C; break;
				case C2:obstype = C2C; break;
				default:
					break;
				}
			}
		}
	}

	// Connect DCB pt_cb2 with first GOBS
	void t_gallbias::_connect_first(const t_spt_bias pt_cb1, t_spt_bias pt_cb2)
	{
		double newval = pt_cb1->bias() - pt_cb2->bias();
		pt_cb2->set(newval, pt_cb2->ref(), pt_cb1->ref());
	}

	// Connect DCB pt_cb2 with second GOBS
	void t_gallbias::_connect_second(const t_spt_bias pt_cb1, t_spt_bias pt_cb2)
	{
		double newval = pt_cb1->bias() + pt_cb2->bias();
		pt_cb2->set(newval, pt_cb2->gobs(), pt_cb1->ref());
	}

	// Consolidate all biases with reference signal of pt_cb2
	void t_gallbias::_consolidate(const string& ac, const string& obj, const t_spt_bias pt_cb1, t_spt_bias pt_cb2)
	{
		//double diff = pt_cb2->ref() - pt_cb1->ref();
		double diff = pt_cb2->val() - pt_cb1->val();
		t_gtime epo = pt_cb1->beg();
		vector<t_spt_bias> vec = _find_ref(ac, epo, obj, pt_cb2->ref());

		for (auto itSPT = vec.begin(); itSPT != vec.end(); itSPT++) {
			double newval = (*itSPT)->bias() - diff;
			GOBS   gobs = (*itSPT)->gobs();
			GOBS   newref = pt_cb1->ref();
			(*itSPT)->set(newval, gobs, newref);
		}

	}

} // namespace
