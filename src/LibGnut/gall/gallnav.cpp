
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
#include <sstream>
#include <iomanip>
#include <cmath>

#include "gall/gallnav.h" 
#include "gutils/gsys.h"
#include "gutils/gtimesync.h"
#include "gutils/gtypeconv.h"
#include "gutils/gfileconv.h"



using namespace std;

namespace gnut
{

	t_gallnav::t_gallnav()
		: t_gdata(),
		_com(false),
		_offset(0),
		_nepoch(t_gtime::GPS),
		_multimap(false),
		_overwrite(false),
		_chk_health(true),
		_chk_navig(true),
		_chk_tot(false)
	{
		gtrace("t_gallnav::constructor");
		id_type(t_gdata::ALLNAV);
		id_group(t_gdata::GRP_EPHEM);
	}


	t_gallnav::~t_gallnav()
	{
		gtrace("t_gallnav::destructor");

		_mapsat.clear();
	}


	// return gnav element
	// ----------
	shared_ptr<t_geph> t_gallnav::find(string sat, const t_gtime& t, bool chk_mask)
	{
		gtrace("t_gallnav::find");

		_gmutex.lock();

		shared_ptr<t_geph> tmp = t_gallnav::_find(sat, t, _chk_health && chk_mask);

		_gmutex.unlock(); return tmp;
	};

	// return position
	// ----------
	int t_gallnav::pos(string sat, const t_gtime& t, double  xyz[],
		double  var[], double  vel[], bool chk_mask) // [m]
	{
		gtrace("t_gallnav::pos");

#ifdef BMUTEX   
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_gmutex.lock();

		shared_ptr<t_geph> tmp = t_gallnav::_find(sat, t, _chk_health && chk_mask);
		// added by yqyuan
		if (tmp == _null) 
		{
			tmp = t_gallnav::_find(sat, t, false);
		}
		// end adding
		if (tmp == _null)
		{
			for (int i = 0; i < 3; i++)
			{
				xyz[i] = 0.0;
				if (var) var[i] = 0.0;
				if (vel) vel[i] = 0.0;
			}
			_gmutex.unlock(); return -1;
		}

		int irc = tmp->pos(t, xyz, var, vel, _chk_health && chk_mask);
		// added by yqyuan
		if (irc == -1) 
		{
			irc = tmp->pos(t, xyz, var, vel, false);
		}
		// end adding		

		_gmutex.unlock(); return irc;

		//  return find( sat, t )->pos( t, xyz, var, vel, _chk_health && chk_mask );
	}
	// return clock corrections
	// ----------
	int t_gallnav::clk(string sat, const t_gtime& t, double*  clk,
		double*  var, double* dclk, bool chk_mask) // [s]
	{
		gtrace("t_gallnav::clk");

#ifdef BMUTEX   
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_gmutex.lock();

		shared_ptr<t_geph> tmp = t_gallnav::_find(sat, t, _chk_health && chk_mask);

		if (tmp == _null) {
			*clk = 0.0;
			if (var) *var = 0.0;
			if (dclk) *dclk = 0.0;
			_gmutex.unlock(); return -1;
		}

		int irc = tmp->clk(t, clk, var, dclk, _chk_health && chk_mask);
		_gmutex.unlock(); return irc;

		//  return this->find( sat, t )->clk( t, clk, var, dclk, _chk_health && chk_mask );
	}
	int t_gallnav::add(shared_ptr<t_gnav> nav)
	{
		gtrace("t_gallnav::add");

#ifdef BMUTEX   
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_gmutex.lock();

		t_gtime ep(nav->epoch());
		string sat = nav->sat();

		if (_chk_navig) {
			set<string> msg; nav->chk(msg);
			if (_log) {
				for (auto it = msg.begin(); it != msg.end(); it++)
					_log->comment(2, "gallnav", *it);
			}
		}

		// test navigation type
		bool add = false;
		if (_multimap) { add = true; }    // multimap
		else if (_mapsat[sat].find(ep) == _mapsat[sat].end()) { add = true; }    // non-existent
		else if (nav->id_type() == t_gdata::EPHGAL) {                            // check nav-type
			auto itB = _mapsat[sat].lower_bound(ep);
			auto itE = _mapsat[sat].upper_bound(ep);
			while (itB != itE) {
				if (dynamic_pointer_cast<t_gnav>(itB->second)->gnavtype() == nav->gnavtype()) {
					add = false; break;
				} // exclude the message and skip!
				else { add = true; } // ok
				++itB;
			}
		}
		if (!nav->valid()) add = false;    // validity test  

		if (add) {
			if (_log && _log->verb() >= 3) {
				ostringstream lg;
				lg << "add sat [" << nav->str_type() << "]: " << sat << " " << ep.str_ymdhms()
					<< " iod: " << fixed << setw(3) << nav->iod()
					<< " flg: " << fixed << setw(3) << nav->healthy();
				_log->comment(3, "gallnav", lg.str());
			}
			_mapsat[sat].insert(make_pair(ep, nav));
			if (sat.substr(0, 1) == "R") {
				_glo_freq_num[sat] = nav->freq_num();
			}
			

		}
		else if (_log && _log->verb() >= 3) {
			ostringstream lg;
			lg << "skip sat [" << nav->str_type() << "]: " << sat << " " << ep.str_ymdhms()
				<< " iod: " << fixed << setw(3) << nav->iod()
				<< " flg: " << fixed << setw(3) << nav->healthy();
			_log->comment(3, "gallnav", lg.str());
		}

		_gmutex.unlock(); return 0;
	}

	// return list of available satellites
	// ----------
	shared_ptr<t_geph> t_gallnav::_find(string sat, const t_gtime& t, bool chk_mask)
	{
		gtrace("t_gallnav::_find sat/time");

		if (_mapsat.find(sat) == _mapsat.end()) return _null; // make_shared<t_geph>();

		GSYS gnss = t_gsys::str2gsys(sat.substr(0, 1));

		if (_mapsat[sat].size() == 0) {
			if (_log) _log->comment(2, "gallnav", sat + " no gnav elements found");
			return _null;
		}

		auto it = _mapsat[sat].lower_bound(t);  // greater|equal  (can be still end())
		if (it == _mapsat[sat].end()) it--;                   // size() > 0 already checked above

		double maxdiff = t_gnav::nav_validity(gnss)*1.1;

		if (gnss == BDS) {
			// BeiDou ephemerides are valid for toc -> toc+60min
			for (int bck = 1; bck <= 5; bck++) {     // max eph for going back is 5
				t_gtime toc = it->second->epoch();
				if ((t < toc || t > toc + maxdiff) ||
					(_chk_tot && !it->second->chktot(t))) {        // check ToT for using past messages only
					if (_mapsat[sat].size() > 0 && it != _mapsat[sat].begin()) it--; // one more step back
					else break;
				}
				if ((t >= toc && t <= toc + maxdiff) &&
					(_chk_tot && it->second->chktot(t))) break;
			}
		}

		// tested found ephemeris
		if (fabs(t - it->second->epoch()) > maxdiff ||
			(_chk_tot && !it->second->chktot(t))) {   // simulation real-time epoch search 
			if (_log) {
				string lg(sat + " gnav element not found: " + t.str_ymdhms());
				_log->comment(2, "gallnav", lg);
			}
			return _null;
		}

		if (_chk_health && chk_mask) {
			if (it->second->healthy()) return it->second;
			else                      return _null;
		}
		return it->second;

	}
} // namespace
