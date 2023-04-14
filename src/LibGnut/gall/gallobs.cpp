
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
#include <sstream>
#include <algorithm>
#include "gall/gallobs.h"

using namespace std;

namespace gnut {  


t_gallobs::t_gallobs() 
  : t_gdata(),
    _set(0),
    _nepoch(0),
    _overwrite(false)
{
  gtrace("t_gallobs::constructor");
  id_type(  t_gdata::ALLOBS );
  id_group( t_gdata::GRP_OBSERV );
}

t_gallobs::~t_gallobs()
{
  gtrace("t_gallobs::destructor");
  _mapobj.clear();
  _filter.clear();
}

void t_gallobs::gset(t_gsetbase* gset)
{  
  _set = gset;
  _sys = dynamic_cast<t_gsetgen*>(_set)->sys();
  _smp = dynamic_cast<t_gsetgen*>(_set)->sampling();

  return;
}
   
vector<t_gobsgnss> t_gallobs::obs(const string& site,
                                 const t_gtime& t)
{
  gtrace("t_gallobs::obs");

#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();
  
  vector<t_gobsgnss> all_obs = _gobs(site,t);

  _gmutex.unlock(); return all_obs;
}

vector<t_spt_gobs> t_gallobs::obs_pt(const string& site, const t_gtime& t)
{
    gtrace("t_gallobs::obs_pt");

#ifdef BMUTEX   
    boost::mutex::scoped_lock lock(_mutex);
#endif
    _gmutex.lock();

    vector<t_spt_gobs> all_obs;
    t_gtime tt(t_gtime::GPS);

    if (_find_epo(site, t, tt) < 0) {
        _gmutex.unlock(); return all_obs;
    }

    t_map_osat::iterator itSAT = _mapobj[site][tt].begin();
    while (itSAT != _mapobj[site][tt].end()) {

        string sat = itSAT->first;

        // TESTING NEW METHOD
        all_obs.push_back(dynamic_pointer_cast<t_gobsgnss>(itSAT->second));

        itSAT++;
    }

    _gmutex.unlock(); return all_obs;
}

vector<t_spt_gobs> t_gallobs::obs_prn_pt(const string& site, const string& prn,
    const t_gtime& beg, const t_gtime& end)
{
    gtrace("t_gallobs::obs_prn_pt");

#ifdef BMUTEX
    boost::mutex::scoped_lock lock(_mutex);
#endif
    _gmutex.lock();

    vector<t_spt_gobs> all_obs;
    t_gtime tt(t_gtime::GPS);

    if (_mapobj.find(site) == _mapobj.end()) {
#ifdef DEBUG     
        cout << "site not found: " << site << t.str_ymdhms() << endl; cout.flush();
#endif
        _gmutex.unlock(); return all_obs;
    }

    t_map_oref::iterator it1 = _mapobj[site].lower_bound(beg);  // greater || equal
    t_map_oref::iterator it2 = _mapobj[site].lower_bound(end);  // greater || equal   

    for (; it1 != it2; ++it1)
    {
        tt = it1->first;
        if (_mapobj[site].find(tt) != _mapobj[site].end())
        {
            auto itSAT = _mapobj[site][tt].find(prn);
            if (itSAT != _mapobj[site][tt].end()) all_obs.push_back(itSAT->second);
        }
    }
    _gmutex.unlock(); return all_obs;
}

vector<t_gobsgnss> t_gallobs::_gobs(const string& site,
                                    const t_gtime& t)
{
  vector<t_gobsgnss> all_obs;
  t_gtime tt(t_gtime::GPS);

  if (_find_epo(site, t, tt) < 0) {
     return all_obs;
  }

  t_map_osat::iterator itSAT = _mapobj[site][tt].begin();
  while( itSAT != _mapobj[site][tt].end() ){    
    all_obs.push_back( *itSAT->second );
    itSAT++;
  }

  return all_obs;
}

int t_gallobs::addobs(t_spt_gobs obs)
{
  gtrace("t_gallobs::addobs");
   
  t_gtime t(obs->epoch()),   tt = t;
  string site = obs->site();
  string sat  = obs->sat();

  if (_map_sites.find(site) == _map_sites.end()) _map_sites.insert(site);

#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  int  epo_found = _find_epo(site, t, tt);
  auto itSAT     = _mapobj[site][tt].find(sat);


  // add new observations (or overwrite)
  // ===================================
  if( _overwrite || epo_found < 0                     // epoch exists (smart search)
                 || itSAT == _mapobj[site][tt].end()  // satellite exists
  ){
#ifdef DEBUG
    cout << " ADDING " << site << " " <<  _mapobj[site].size() << " " << _nepoch // << " " <<  nepochs(site) // MUTEX !!!
         << " sat "    <<  sat << " " << tt.str_ymdhms() << " " << "\n";cout.flush();
#endif

    // delete old if exists
    // ====================
    if( _overwrite && epo_found > 0                     // epoch exists (smart search)
                   && itSAT != _mapobj[site][tt].end()  // satellite exists    
    ){
        _mapobj[site][tt].erase(sat);
        if( _log && _log->verb() >= 2 ) _log->comment(2, "gallobs", site + tt.str_ymdhms(" obs replaced ") + sat);
    }

    // too many epochs (remove old)
    // ============================
    if( _nepoch > 0 && _mapobj[site].size() > _nepoch + 10 ){  // +10 .. reduce number of removals to every tenths

      auto itEPO = _mapobj[site].begin();
      while( itEPO != _mapobj[site].end() ){

        if( _mapobj[site].size() <= _nepoch ){ break; }
	
        t_gtime t = itEPO->first;
        if( _log && _log->verb() >= 2 )
        {
           string satells(" ");
           for(auto itSAT = _mapobj[site][t].begin(); itSAT != _mapobj[site][t].end(); ++itSAT ){
             satells = satells + " " + itSAT->first;
           }
       
           _log->comment(2, "gallobs", site + t.str_ymdhms(" obs removed ") + satells);
        }

        _mapobj[site][t].erase(_mapobj[site][t].begin(),_mapobj[site][t].end());
        _mapobj[site].erase(itEPO++);
      }
    }

    // distinguish GNSS specific message
    // =================================
    if( obs->id_type() == t_gdata::OBSGNSS ){

      _mapobj[site][tt][sat] = obs;

    }else{
       if( _log ){  _log->comment(0,"gallobs","warning: t_gobsgnss record not identified!"); }
       _gmutex.unlock(); return 1;
    }
  }else{
    if( _log && _log->verb() >= 2 ) _log->comment(2,"gallobs", site + tt.str_ymdhms(" skipped ") + " " + sat);
    _gmutex.unlock(); return 0;
  }

  // comments
  if( _log && _log->verb() >= 3 ){

    vector<GOBS> v_obs = obs->obs();
    vector<GOBS>::iterator itOBS = v_obs.begin();
    ostringstream lg; lg << fixed << setprecision(3);
    for( ; itOBS != v_obs.end(); ++itOBS ) 
      lg << " " << gobs2str( *itOBS ) << ":" << obs->getobs( *itOBS );

    _log->comment(3,"gallobs",site + tt.str_ymdhms(" add obs: ") + " " + sat + lg.str() );
  }
  _gmutex.unlock(); return 0;
}

t_gallobs::t_map_osat t_gallobs::find( string site, const t_gtime& t)
{
  gtrace("t_gallobs::find");

#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  t_gtime tt(t_gtime::GPS);
  t_map_osat  tmp;
  if(_find_epo(site, t, tt) < 0){
     _gmutex.unlock(); return tmp;
  }

  _gmutex.unlock(); return _mapobj[site][tt]; // tmp;
}
 
double t_gallobs::find( string site, const t_gtime& t, const string& prn, const GOBS& gobs)
{
   gtrace("t_gallobs::find");

#ifdef BMUTEX   
   boost::mutex::scoped_lock lock(_mutex);
#endif
   _gmutex.lock();
   
   double obs = 0.0;   
   
   t_gtime tt(t_gtime::GPS);
   t_map_osat  tmp;
   if(_find_epo(site, t, tt) < 0){
      _gmutex.unlock(); return obs;
   }   
   
   t_map_osat mdata = _mapobj[site][tt];
   t_map_osat::iterator it = mdata.find(prn);

   if( it != mdata.end() ){
      t_spt_gobs satdata = it->second;
      vector<GOBS> vobs = satdata->obs(); 
      for(vector<GOBS>::iterator it2 = vobs.begin(); it2 != vobs.end(); it2++){
        if(*it2 == gobs) obs = satdata->getobs(*it2);
        else continue;
      }
   } 
   _gmutex.unlock(); return obs;
}

void t_gallobs::xdata(string site, string file, t_xfilter xflt)
{
  gtrace("t_gallobs::xdata");

  _gmutex.lock();
  
  _filter[site][file].xdat = xflt.xdat;
  _filter[site][file].beg  = xflt.beg;
  _filter[site][file].end  = xflt.end;

  _gmutex.unlock();
}

int t_gallobs::_find_epo(const string& site, const t_gtime& epo, t_gtime& tt)
{
  gtrace("t_gallobs::_find_epo");

  if( _mapobj.find(site) == _mapobj.end() ){
     if( _log && _log->verb() >= 2 ){
        _log->comment(3,"gallobs","site not found: " + site + " " + epo.str_ymdhms());
      }
      return -1;
  }
   
  t_map_oref::iterator it1 = _mapobj[site].lower_bound(epo);  // greater || equal
  t_map_oref::iterator it0 = it1;                             // previous value

  if( it0 != _mapobj[site].begin() ) it0--;                   // set previous value  
  if( it1 == _mapobj[site].end() ) it1 = it0;

  if( it1 == _mapobj[site].end() && it0 == _mapobj[site].end() ){
    cout << site << " " << epo.str_ymdhms() << " " << epo.dsec() << " no observations !\n";
    return -1;
  }
     
#ifdef DEBUG
    cout << site << " " << epo.str_ymdhms() << " " << epo.dsec() << fixed << setprecision(6)
         << " dif_sec: " << DIFF_SEC(_smp)
         << " t1_diff: " << setw(14) << it1->first.diff(epo)
         << " t0_diff: " << setw(14) << it0->first.diff(epo) << endl;
#endif

  if(      fabs(it1->first - epo) <= DIFF_SEC(_smp) ) tt = it1->first;  // it = it1;                 // set closest value
  else if( fabs(it0->first - epo) <= DIFF_SEC(_smp) ) tt = it0->first;  // it = it0;                 // set closest value
  else{
#ifdef DEBUG
     if( _log ){
       ostringstream lg;
       lg << site << tt.str_ymd()  << tt.dsec()
          << " it0 = " << it0->first.str_ymdhms() << " diff: " << it1->first.diff(epo)
          << " it1 = " << it1->first.str_ymdhms() << " diff: " << it0->first.diff(epo)
          << " not close observations found!";
       _log->comment(0,"gallobs",lg.str());
     }     
#endif
     return -1;                                     // not found !
  }
   
   return 1;
}

   
} // namespace
