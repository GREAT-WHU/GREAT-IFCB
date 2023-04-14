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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "gdata/gobsgnss.h"
#include "gutils/gconst.h"
#include "gutils/gtypeconv.h"

using namespace std;

namespace gnut {

t_gobsgnss::t_gobsgnss()
  : t_gdata(),
    _channel(DEF_CHANNEL),
    _health(true),
	_bds_code_bias_mark(false)
{ 
  id_type(t_gdata::OBSGNSS);
  id_group(t_gdata::GRP_OBSERV);
}


t_gobsgnss::t_gobsgnss(const string& sat)
	: t_gdata(),
	_channel(DEF_CHANNEL),
	_health(true),
	_bds_code_bias_mark(false),
	_satid(sat),
	_gsys(t_gsys::char2gsys(sat[0]))
{
	id_type(t_gdata::OBSGNSS);
	id_group(t_gdata::GRP_OBSERV);
}


t_gobsgnss::t_gobsgnss(const string& site, const string& sat, const t_gtime& t)
  : t_gdata(),
     _staid(site),
     _satid(sat),
     _gsys(t_gsys::char2gsys( _satid[0] )),
     _epoch(t),
     _channel(DEF_CHANNEL),
     _health(true),
	 _bds_code_bias_mark(false)
 { 
  id_type(t_gdata::OBSGNSS);
  id_group(t_gdata::GRP_OBSERV);
}




t_gobsgnss::~t_gobsgnss()
{
}


void t_gobsgnss::addobs(const GOBS& obs, const double& d)
{
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  _gobs[obs] = d;
  _gmutex.unlock();
  return;
}


void t_gobsgnss::addlli(const GOBS& obs, const int& i)
{  
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();
  if( i != 0 ) _glli[obs] = i;
  _gmutex.unlock();
   return;
}


void t_gobsgnss::addoutliers(const GOBS& obs, const int& i)
{
#ifdef BMUTEX   
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();
	if (i != 0) _goutlier[obs] = i;
	_gmutex.unlock();
	return;
}
   

void t_gobsgnss::setrangestate(string name, bool b)
{
#ifdef BMUTEX   
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();
	if (name == "bds_code_bias")  _bds_code_bias_mark = b;
	_gmutex.unlock();
	return;
}

bool t_gobsgnss::getrangestate(string name)
{
#ifdef BMUTEX   
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();
	bool tmp = false;
	if      (name == "bds_code_bias") tmp = _bds_code_bias_mark;
	_gmutex.unlock(); return tmp;
}
vector<GOBS> t_gobsgnss::obs()
{  
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  vector<GOBS> tmp;
  map<GOBS,double>::iterator it = _gobs.begin();  
  while( it != _gobs.end() ){
    tmp.push_back( it->first );
    it++;
  }
   
  _gmutex.unlock(); return tmp;
}


double t_gobsgnss::getobs(const GOBS& obs)
{
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();
   
  double tmp = NULL_GOBS;
  if( _gobs.find(obs) != _gobs.end() ) tmp = _gobs[obs];
   
  _gmutex.unlock(); return tmp;
}

// get LLI - lost of lock indicator (int)
// -----------
int t_gobsgnss::getlli(const GOBS& obs)
{
#ifdef BMUTEX   
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();

	int tmp = 0;
	if (_glli.find(obs) != _glli.end()) tmp = _glli[obs];

	_gmutex.unlock(); return tmp;
}

// get range outliers
// -----------
int t_gobsgnss::getoutliers(const GOBS& obs)
{
#ifdef BMUTEX   
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();

	int tmp = 0;
	if (_goutlier.find(obs) != _goutlier.end()) tmp = _goutlier[obs];

	_gmutex.unlock(); return tmp;
}

// reset OBS - code [m], phase [whole cycles], dopler[cycles/sec], snr [DBHZ], ...  observations (double)
// -----------
void t_gobsgnss::resetobs(const GOBS& obs, double v)
{
#ifdef BMUTEX   
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();

	if (_gobs.find(obs) != _gobs.end()) _gobs[obs] = v;

	_gmutex.unlock();
}
void t_gobsgnss::channel(int ch)
{
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();
  _channel = ch;
  _gmutex.unlock();
   return;
}



int t_gobsgnss::channel() const
{
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  int tmp = _channel;
   
  _gmutex.unlock(); return tmp;
}

double t_gobsgnss::frequency( GOBSBAND b )
{

  switch( _gsys ){
     
   case GPS  : switch( b ){
                 case BAND_1 : return G01_F;
                 case BAND_2 : return G02_F;
                 case BAND_5 : return G05_F;
                default : return 0.0; }
     
   case GLO  : switch( b ){
                 case BAND_1 : return R01_F(_channel); 
                 case BAND_2 : return R02_F(_channel); 
                 case BAND_3 : return R03_F_CDMA;
                 case BAND_5 : return R05_F_CDMA;
                default : return 0.0; }
     
   case GAL  : switch( b ){
                 case BAND_1 : return E01_F;
                 case BAND_5 : return E05_F;
                 case BAND_6 : return E06_F;
                 case BAND_7 : return E07_F;
                 case BAND_8 : return E08_F;
                default : return 0.0; }
     
   case BDS  : switch( b ){
                 case BAND_2 : return C02_F;
                 case BAND_6 : return C06_F;
                 case BAND_7 : return C07_F;
				 case BAND_5 : return C05_F;  
				 case BAND_8 : return C08_F;
				 case BAND_9 : return C09_F;
				 case BAND_1 : return C01_F;
                default : return 0.0; }
     
   case QZS  : switch( b ){
                 case BAND_1 : return J01_F;
                 case BAND_2 : return J02_F;
                 case BAND_5 : return J05_F;
                 case BAND_6 : return J06_F;
                default : return 0.0; }
   
   case IRN  : switch( b ){
                 case BAND_5 : return I05_F;
                default : return 0.0;}
	  
     
   case SBS  : switch( b ){
                 case BAND_1 : return S01_F;
                 case BAND_5 : return S05_F;
                default : return 0.0; }
     
   case GNS  : return 0.0;
  }

  return 0.0;
}



double t_gobsgnss::wavelength(GOBSBAND b)
{
  double frq = this->frequency( b );
  if( frq != 0.0 ) return CLIGHT/frq;
     
  return 0.0;
}



double t_gobsgnss :: wavelength_WL(GOBSBAND b1, GOBSBAND b2)
{

	double f1 = this->frequency(b1);
	double f2 = this->frequency(b2);

	if (!double_eq(f1, 0.0) && !double_eq(f2, 0.0)){
		double lamb_WL = CLIGHT / (f1 - f2);
		return lamb_WL;
	}
	return 0.0;
}


double t_gobsgnss::wavelength_NL(GOBSBAND b1, GOBSBAND b2)
{

	double f1 = this->frequency(b1);
	double f2 = this->frequency(b2);

	if (!double_eq(f1, 0.0) && !double_eq(f2, 0.0)){
		double lamb_WL = CLIGHT / (f1 + f2);
		return lamb_WL;
	}
	return 0.0;
}

// get GNSS system from satellite IDOC
// ----------
GSYS t_gobsgnss::gsys() const
{
  return _gsys;
}

// code observations [m]
// ----------
double t_gobsgnss::obs_C(const t_gobs& go)
{ 
  double tmp = this->_obs_range( go );      
   
  return tmp;
}
  

double t_gobsgnss::_obs_range(const t_gobs& go)
{
  // Glonass has 255 channel number 
  if ( ! _valid_obs() ) return NULL_GOBS;

  // AUTO SELECTION
  if( go.attr() == ATTR )
  {
    GOBS gobs = _id_range(go.band());
    if( gobs == X ) return  NULL_GOBS;
    else            return _gobs[gobs];
    return NULL_GOBS;
  }

  // FIXED SELECTION
  // here avoid a PROBLEM WITH the legacy TYPE_P (only when NULL_ATTR)
  // -----------------------------
  map<GOBS,double>::const_iterator it = _gobs.end();

  if( go.attr() == ATTR_NULL &&
      ( go.type() == TYPE ||
        go.type() == TYPE_P )
    )
    {   
      t_gobs gobs(TYPE_P, go.band(), go.attr() );
      it = _gobs.find( gobs.gobs() );
    }  
  
  if( it == _gobs.end() && 
      go.type() != TYPE_P
    ){
    t_gobs gobs(TYPE_C, go.band(), go.attr() );
    it = _gobs.find( gobs.gobs() );
  }

  if( it == _gobs.end() ) return NULL_GOBS;

  return it->second;
}


double t_gobsgnss::_obs_range(const t_gband& gb)
{
  t_gobs gobs(TYPE, gb.band(), gb.attr() );
   
  return _obs_range( gobs );
}



double t_gobsgnss::obs_L(const t_gobs& go)
{
  double tmp = this->_obs_phase( go.gband() );
  return tmp;
}

double t_gobsgnss::_obs_phase(const t_gband& gb)
{
  // Glonass has 255 channel number 
  if ( ! _valid_obs() ) return NULL_GOBS; 

  // AUTO SELECTION
  if( gb.attr() == ATTR )
  {
    GOBS gobs = _id_phase(gb.band());
    if( gobs == X ) return  NULL_GOBS;
    else            return _gobs[gobs]*this->wavelength( gb.band() ); // transfer from whole cycles to meters!;
  }

  // FIXED SELECTION
  t_gobs go(TYPE_L, gb.band(), gb.attr() );

  map<GOBS,double>::const_iterator it;
  it = _gobs.find( go.gobs() );
   
  if( it == _gobs.end() ) return NULL_GOBS;
  return it->second*this->wavelength( gb.band() ); // transfer from whole cycles to meters!
}


// Ionosphere-linear code combination [m]
// ----------
double t_gobsgnss::P3(const t_gobs& g1, const t_gobs& g2) // const
{
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  double coef1, coef2;
   
  _coef_ionofree( g1.band(), coef1, g2.band(), coef2 );

  double lc = _lc_range( &g1, coef1, &g2, coef2 ); // still dual-frequency only

//  cout << "t_gobsgnss::P3() " << sat() << " " << fixed << setprecision(3) << lc << endl;

  _gmutex.unlock(); return lc;
}
// Ionosphere-free linear phase combination [m] !
// ----------
double t_gobsgnss::L3(const t_gobs& g1, const t_gobs& g2) // const
{
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

   if( _gsys == GLO && double_eq(_channel, DEF_CHANNEL)) {
//      cout << epoch().str_hms() << " " << sat() << " " << _channel << endl;      
      _gmutex.unlock(); 
      return 0.0;
   }   
   
  double coef1, coef2;

  _coef_ionofree( g1.band(), coef1, g2.band(), coef2 );

  double lc = _lc_phase( &g1, coef1, &g2, coef2 ); // still dual-frequency only

//  cout << "t_gobsgnss::L3() " << sat() << " " << fixed << setprecision(3) << lc << endl;

  _gmutex.unlock(); return lc;
}
// get Geometry-free combination [m] !
// ----------
double t_gobsgnss::L4(const t_gobs& g1, const t_gobs& g2) // const
{
#ifdef BMUTEX   
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();

	if (_gsys == GLO && double_eq(_channel, DEF_CHANNEL)) {
		_gmutex.unlock();
		return 0.0;
	}

	double coef1, coef2;

	_coef_geomfree(g1.band(), coef1, g2.band(), coef2);

	//double lc =  phase_lc( &g1, coef1, &g2, coef2 ); // still dual-frequency only
	double lc = _lc_phase(&g1, coef1, &g2, coef2); // still dual-frequency only

#ifdef DEBUG   
	cout << " L4 = " << fixed << setprecision(3)
		<< setw(16) << lc
		<< setw(16) << _lc_phase(&g1, 1, &g2, 0)  // first  band
		<< setw(16) << _lc_phase(&g1, 0, &g2, 1)  // second band
		<< endl;
#endif   

	_gmutex.unlock(); return lc;
	}
double t_gobsgnss::GFIF_meter(const t_gobs& gL1, const t_gobs& gL2, const t_gobs& gL3)
{
#ifdef BMUTEX
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();

	double frequency1 = frequency(gL1.band());
	double frequency2 = frequency(gL2.band());
	double frequency3 = frequency(gL3.band());
	if (double_eq(frequency1, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	if (double_eq(frequency2, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	if (double_eq(frequency3, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }

	double coef_1 = frequency1 * frequency1 / (frequency1 * frequency1 - frequency2 * frequency2);
	double coef_2 = -frequency2 * frequency2 / (frequency1 * frequency1 - frequency2 * frequency2);
	double coef_3 = frequency1 * frequency1 / (frequency1 * frequency1 - frequency3 * frequency3);
	double coef_4 = -frequency3 * frequency3 / (frequency1 * frequency1 - frequency3 * frequency3);

	double L1 = NULL_GOBS;
	double L2 = NULL_GOBS;
	double L3 = NULL_GOBS;

	L1 = obs_L(gL1); if (double_eq(L1, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	L2 = obs_L(gL2); if (double_eq(L2, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	L3 = obs_L(gL3); if (double_eq(L3, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }

	double GFIF_obs = (coef_1 * L1 + coef_2 * L2) - (coef_3 * L1 + coef_4 * L3);

	_gmutex.unlock();
	return GFIF_obs;
}
double t_gobsgnss::MW_cycle(const t_gobs& gL1, const t_gobs& gL2,
	const t_gobs& gC1, const t_gobs& gC2)
{
#ifdef BMUTEX
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();

	if (gsys() == GLO && double_eq(_channel, DEF_CHANNEL)) {
		_gmutex.unlock();
		return 0.0;
	}

	double wlength1 = wavelength(gL1.band());
	double wlength2 = wavelength(gL2.band());
	double fact = wlength1 / wlength2;

	double C1 = NULL_GOBS;
	double C2 = NULL_GOBS;
	double L1 = NULL_GOBS;
	double L2 = NULL_GOBS;

    C1 = obs_C(gC1); if (double_eq(C1, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	C2 = obs_C(gC2); if (double_eq(C2, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	L1 = obs_L(gL1) / wlength1; if (double_eq(L1, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	L2 = obs_L(gL2) / wlength2; if (double_eq(L2, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }

	double mw = L1 - L2 - (C1 / wlength1 + C2 / wlength2) * (1.0 - fact) / (1.0 + fact);


	_gmutex.unlock(); return mw;
}


double t_gobsgnss::EWL_cycle(const t_gobs& gL1, const t_gobs& gL2, const t_gobs& gL3,
	const t_gobs& gC1, const t_gobs& gC2)
{
#ifdef BMUTEX
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_gmutex.lock();

	if (gsys() == GLO && double_eq(_channel, DEF_CHANNEL)) {
		_gmutex.unlock();
		return 0.0;
	}

	double wlength1 = wavelength(gL1.band());
	double wlength2 = wavelength(gL2.band());
	double wlength3 = wavelength(gL3.band());
	if (double_eq(wlength1, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	if (double_eq(wlength2, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	if (double_eq(wlength3, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	double alpha = (wlength2*wlength3 - wlength1*wlength1) / (wlength2*wlength2 - wlength1*wlength1);

	double C1 = NULL_GOBS;
	double C2 = NULL_GOBS;
	double L2 = NULL_GOBS;
	double L3 = NULL_GOBS;

	C1 = obs_C(gC1); if (double_eq(C1, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	C2 = obs_C(gC2); if (double_eq(C2, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	L2 = obs_L(gL2) / wlength2; if (double_eq(L2, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }
	L3 = obs_L(gL3) / wlength3; if (double_eq(L3, NULL_GOBS)) { _gmutex.unlock(); return NULL_GOBS; }

	double ewl = L2 - L3 - (C1 * (1 - alpha) + C2 * alpha) * (wlength3 - wlength2) / (wlength2 * wlength3);


	_gmutex.unlock(); return ewl;
}
bool t_gobsgnss::apply_bias(t_gallbias& allbias)
{
	if (&allbias == NULL)
	{
		return true;
}

	if (allbias.get_used_ac() == "SGG_A") return apply_code_phase_bias(allbias); // Temporarily reserved
	else                       return apply_dcb(allbias);
	}

double t_gobsgnss::interFreqDcb(t_gallbias& allbias, GOBSBAND band1, GOBSBAND band2)
{

	auto gsys = this->gsys();
	auto gsat = this->sat();
	auto grec = this->site();
	auto gepo = this->epoch();

	t_gobs g1 = this->select_range(band1);
	t_gobs g2 = this->select_range(band2);

	g1.gobs2to3(this->gsys());
	g2.gobs2to3(this->gsys());

	double dcb_value = 0.0;

	GOBS obs1 = t_gsys::gobs_priority(gsys, g1.gobs());
	GOBS obs2 = t_gsys::gobs_priority(gsys, g2.gobs());

	auto default_band1 = GNSS_BAND_PRIORITY.at(gsys)[1];
	auto default_band2 = GNSS_BAND_PRIORITY.at(gsys)[2];
	auto default_band3 = GNSS_BAND_PRIORITY.at(gsys)[3];


	if (obs1 == GOBS::X && obs2 == GOBS::X)
	{
		return 0.0;
	}

	if (obs1 == GOBS::X)
	{
		obs1 = t_gsys::gobs_defaults(gsys, obs2, band1);
	}

	if (obs2 == GOBS::X)
	{
		obs2 = t_gsys::gobs_defaults(gsys, obs1, band2);
	}


	dcb_value = allbias.get(gepo, gsat, obs1, obs2);
	if (double_eq(dcb_value, 0.0))
	{
		string logtmp = gobs2str(obs1) + " " + gobs2str(obs2) + " " + " " + gsat + " " + " " + grec + " " + gepo.str_mjdsod("wrong type of GNSS sys, only support GPS and GAL.");
		if (_log)_log->comment(3, "interFreqDcb", logtmp);
		return 0.0;
	}

	return dcb_value;
}


bool t_gobsgnss::apply_code_phase_bias(t_gallbias& allbias)
{
	double bias1, bias2, c1, c2, biasif;
	GOBSBAND b1, b2;
	// skip already correct dcb obsdata
	if (this->_dcb_correct_mark) {
		return true;
	}

	string gsat = this->sat();
	t_gtime gepo = this->epoch();
	GSYS gsys = this->gsys();

	for (GOBS obs_type : this->obs())
	{

		t_gobs gobs_type(obs_type);
		gobs_type.gobs2to3(gsys);

		double bias = 0.0;
		if (t_gobs(obs_type).is_code())
		{
			bias = allbias.get(gepo, gsat, gobs_type.gobs(), gobs_type.gobs(), "WHU");

		}
		else if (t_gobs(obs_type).is_phase())
		{
			bias = -1 * allbias.get(gepo, gsat, gobs_type.gobs(), gobs_type.gobs(), "WHU_PHASE") / wavelength(t_gobs(obs_type).band());
			if (t_gobs(obs_type) == L1W)
			{
				bias1 = allbias.get(gepo, gsat, gobs_type.gobs(), gobs_type.gobs(), "WHU_PHASE");
				b1 = t_gobs(obs_type).band();

			}
			else if (t_gobs(obs_type) == L2W)
			{
				bias2 = allbias.get(gepo, gsat, gobs_type.gobs(), gobs_type.gobs(), "WHU_PHASE");
				b2 = t_gobs(obs_type).band();

			}
		}
		else
		{
			continue;
		}

		// apply dcb to obsdata
		double obs_value = this->getobs(gobs_type.gobs());
		if (t_gobs(obs_type) == L2W)
		{
			_coef_ionofree(b1, c1, b2, c2);
			biasif = c1 * bias1 + c2 * bias2;
			std::cout << gsat << " " << t_gobs(obs_type).gobs() << " " << biasif << endl;
			printf("");
		}
		//		obs_value -= bias;
		this->resetobs(gobs_type.gobs(), obs_value);
	}
	this->_dcb_correct_mark = true;
	return true;
}

bool t_gobsgnss::apply_dcb(t_gallbias& allbias)
{
	if (&allbias == NULL)
	{
		_log->comment(1, this->site() + " : NO DCB FILE");
		return true;
	}
	// skip already correct dcb obsdata
	if (this->_dcb_correct_mark)
	{
		return true;
	}

	string gsat = this->sat();
	t_gtime gepo = this->epoch();
	GSYS gsys = this->gsys();

	// two band is bound to precise clock 
	auto band1 = GNSS_BAND_PRIORITY.at(gsys)[1];
	auto band2 = GNSS_BAND_PRIORITY.at(gsys)[2];
	if (gsys == GSYS::BDS)
	{
		band1 = GNSS_BAND_PRIORITY.at(gsys)[1];
		band2 = GNSS_BAND_PRIORITY.at(gsys)[3];

	}

	// caculate interdcb band1-band2
	double alfa12 = 0.0;
	double alfa13 = 0.0;
	double beta12 = 0.0;
	double beta13 = 0.0;
	this->_coef_ionofree(band1, alfa12, band2, beta12);
	double inter_dcb_12 = this->interFreqDcb(allbias, band1, band2);
	double inter_dcb_1x = 0.0;
	double inter_dcb_2x = 0.0;
	for (GOBS obs_type : this->obs())
	{

		// skip not code obs
		if (!t_gobs(obs_type).is_code()) {
			continue;
		}
		double obs_value = this->getobs(obs_type);

		if (double_eq(obs_value, 0.0))continue;

		// apply intra dcb correct [GPS]
		t_gobs gobs_type(obs_type); 
		gobs_type.gobs2to3(gsys); 

		double dcb_value = 0.0;
		if (gsys == GSYS::GPS)
		{
			switch (gobs_type.gobs())
			{
			case C1C: dcb_value = allbias.get(gepo, gsat, C1W, C1C); break;
			case C1P: dcb_value = 0.0;
			case C1Y: dcb_value = 0.0;
			case C1W: dcb_value = 0.0;                               break;
			case C2C: dcb_value = allbias.get(gepo, gsat, C2W, C2C); break;
			case C2L: dcb_value = allbias.get(gepo, gsat, C2W, C2L); break;
			case C2X: dcb_value = allbias.get(gepo, gsat, C2W, C2X); break;
			case C2P: dcb_value = 0.0;
			case C2Y: dcb_value = 0.0;
			case C2W: dcb_value = 0.0;                               break;
			case C5Q: dcb_value = allbias.get(gepo, gsat, C5X, C5Q); break;
			case C5X: dcb_value = 0.0;                               break;
			default:  dcb_value = 0.0;                               break;
			}
		}
		else if (gsys == GSYS::GLO)
		{
			switch (gobs_type.gobs())
			{
			case C1C: dcb_value = allbias.get(gepo, gsat, C1P, C1C); break;
			case C1P: dcb_value = 0.0; break;
			case C2C: dcb_value = allbias.get(gepo, gsat, C2P, C2C); break;
			case C2P: dcb_value = 0.0; break;
			}
		}

		// apply inter dcb correct
		if (gobs_type.band() == band1)
		{
			dcb_value += -beta12 * inter_dcb_12;
		}
		else if (gobs_type.band() == band2)
		{
			dcb_value += +alfa12 * inter_dcb_12;
		}
		else
		{
			GOBSBAND band_x = gobs_type.band();
			inter_dcb_1x = this->interFreqDcb(allbias, band1, band_x);
			inter_dcb_2x = this->interFreqDcb(allbias, band2, band_x);
			dcb_value += alfa12 * inter_dcb_1x + beta12 * inter_dcb_2x;
		}

		// apply dcb to obsdata
		obs_value += dcb_value;
		this->resetobs(obs_type, obs_value);

	}
	this->_dcb_correct_mark = true;
	return true;
}

bool t_gobsgnss::valid()
{
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();
   
  bool tmp = this->_valid();
   
  _gmutex.unlock(); return tmp;
   
}

bool t_gobsgnss::obs_empty()
{
	return _gobs.empty();
}



void t_gobsgnss::clear()
{ 
#ifdef BMUTEX   
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  this->_clear();
   
  _gmutex.unlock();
   return;
}



void t_gobsgnss::_clear()
{
  _staid.clear();
  _epoch = FIRST_TIME;
}


bool t_gobsgnss::_valid() const
{   
  if( _staid.empty() || 
      _staid == ""   || 
      _epoch == FIRST_TIME ) return false;

  return true; 
}


bool t_gobsgnss::_valid_obs() const
{
   if( _gsys == GLO && _channel == 255)
        return false;
   else return true;
}


// return value of general carrier-phase linear combination
// for 2 or 3 bands with given coefficients
// ----------
double t_gobsgnss::_lc_range(const t_gobs* g1, const double& coef1,
	                     const t_gobs* g2, const double& coef2,
	                     const t_gobs* g3, const double& coef3 )
{
  double C1 = NULL_GOBS;
  double C2 = NULL_GOBS;
  double C3 = NULL_GOBS;

  if( g1 && coef1 != 0.0 ){ C1 = obs_C( *g1 ); if( double_eq(C1,NULL_GOBS) ) return NULL_GOBS; }
  if( g2 && coef2 != 0.0 ){ C2 = obs_C( *g2 ); if( double_eq(C2,NULL_GOBS) ) return NULL_GOBS; }
  if( g3 && coef3 != 0.0 ){ C3 = obs_C( *g3 ); if( double_eq(C3,NULL_GOBS) ) return NULL_GOBS; }

  if (fabs(C1 - C2) > 100)
  {
	  if (_log)
		  _log->comment(2, "t_gobsgnss", "Inconsistencies of the code observations " + _epoch.str_ymdhms() );
	  return 0.0;
  }

  string sat = this->sat();

  double lc = ( coef1*C1 + coef2*C2 + coef3*C3 );

#ifdef DEBUG
   int b1, b2, b3;
   b1 = b2 = b3 = 0;
   if(g1) b1 = g1->band();    if(g2) b2 = g2->band();    if(g2) b2 = g2->band();   
    cout << _satid << " " << _staid
         << " _lc_range BAND(" << b1   << "," <<  b2 << " ," << b3 << ") "     
         <<          "  COEF(" << coef1 << "," << coef2 << "," << coef3 << ") "
         << fixed << setprecision(3)
         << setw(16) << C1 << setw(16) << C2 << setw(16) << C3 
         << "  lc = " << setw(16) << lc << endl;
#endif
  return lc;
}

// return value of general carrier-phase linear combination
// for 2 or 3 bands with given coefficients
// ----------
double t_gobsgnss::_lc_phase(const t_gobs* g1, const double& coef1,
	                     const t_gobs* g2, const double& coef2,
	                     const t_gobs* g3, const double& coef3 )
{
  double L1 = NULL_GOBS;
  double L2 = NULL_GOBS;
  double L3 = NULL_GOBS;

  if( g1 && coef1 != 0.0 ){ L1 = obs_L( *g1 ); if( double_eq(L1,NULL_GOBS) ) return NULL_GOBS; }
  if( g2 && coef2 != 0.0 ){ L2 = obs_L( *g2 ); if( double_eq(L2,NULL_GOBS) ) return NULL_GOBS; }
  if( g3 && coef3 != 0.0 ){ L3 = obs_L( *g3 ); if( double_eq(L3,NULL_GOBS) ) return NULL_GOBS; }

  double lc = ( coef1*L1 + coef2*L2 + coef3*L3 );

#ifdef DEBUG
   int b1, b2, b3;
   b1 = b2 = b3 = 0;
   if(g1) b1 = g1->band();    if(g2) b2 = g2->band();    if(g2) b2 = g2->band();
    cout << _satid << " " << _staid
         << " _lc_phase BAND(" << b1   << "," <<  b2 << " ," << b3 << ") "
         <<          "  COEF(" << coef1 << "," << coef2 << "," << coef3 << ") "
         << fixed << setprecision(3)
         << setw(16) << L1/wavelength(g1->band()) << setw(16) << L2/wavelength(g2->band()) << setw(16) << L3 
         << "  lc = " << setw(16) << lc << endl;
#endif
  return lc;
}
GOBS t_gobsgnss::_id_range(GOBSBAND b)
{ 
  map<GOBS,double>::const_iterator it = _gobs.begin();

  while( it != _gobs.end() ){
    GOBS gobs = it->first;
    if( gobs_code( gobs ) ){
      GOBSBAND tmp = str2gobsband( gobs2str(gobs) );
      if( tmp == b ) return gobs;
    }
    it++;
  }
  return X;
}
   


GOBS t_gobsgnss::_id_phase(GOBSBAND b)
{ 
  map<GOBS,double>::const_iterator it = _gobs.begin();

  while( it != _gobs.end() ){
    GOBS gobs = it->first;
    if( gobs_phase( gobs ) ){
      GOBSBAND tmp = str2gobsband( gobs2str(gobs) );
      if( tmp == b ) return gobs;
    }
    it++;
  }
  return X;
}


GOBS t_gobsgnss::select_range(GOBSBAND b) const
{
	static map<GSYS, map<GOBSBAND, string> > _order_attr = 
	{
		{ GPS, { {BAND_1,"CPW" },{BAND_2,"CLXPW"}, {BAND_5,"QX"} } },
		{ GAL, { {BAND_1,"CX"  },{BAND_5,"IQX"  }, {BAND_7,"IQX"},{BAND_8,"IQX"},{BAND_6,"ABCXZ"}} },
		{ BDS, { {BAND_2,"IQX" },{BAND_7,"IQX"  }, {BAND_6,"IQX"},{BAND_5,"DPX"},{BAND_9,"DPZ"},{BAND_8,"DPX"},{BAND_1,"DPX"}} },
		{ GLO, { {BAND_1,"CP"  },{BAND_2,"CP"   }                       } },
		{ QZS, { {BAND_1,"CSLX"},{BAND_2,"LX"   },{BAND_5,"IQX"}} }
	};

	int max = -1;
	GOBS gobs_ans;
	map<GOBS, double>::const_iterator it = _gobs.begin();
	while (it != _gobs.end()) 
	{
		GOBS gobs = it->first;
		if (gobs_code(gobs)) 
		{
			GOBSBAND tmp = str2gobsband(gobs2str(gobs));
			if (tmp == b) 
			{
				t_gobs gobs_temp(gobs); gobs_temp.gobs2to3(_gsys);
				int loc = _order_attr[_gsys][b].find(gobs2str(gobs_temp.gobs())[2]);
				if (loc > max)
				{
					gobs_ans = gobs;
					max = loc;
				}
			}
		}
		it++;
	}
	if (max == -1) 
	{
		return X;
	}
	else 
	{
		return gobs_ans;
	}
}


GOBS t_gobsgnss::select_phase(GOBSBAND b) const
{
	static map<GSYS, map<GOBSBAND, string> > _order_attr = 
	{
	{GPS,{{BAND_1,"CSLXPWYM"},{BAND_2,"CDLXPWYM"},{BAND_5,"IQX"}}},
	{GAL,{{BAND_1,"ABCXZ"},{BAND_5,"IQX"},{BAND_7,"IQX"},{BAND_8,"IQX"},{BAND_6,"ABCXZ"}}},
	{BDS,{{BAND_2,"XIQ"},{BAND_7,"IQX"},{BAND_6,"IQX"},{BAND_5,"DPX"},{BAND_9,"DPZ"},{BAND_8,"DPX"},{BAND_1,"DPX"}}},
	{GLO,{{BAND_1,"PC"},{BAND_2,"CP"}}}, 
	{QZS,{{BAND_1,"CSLX"},{BAND_2,"LX"},{BAND_5,"IQX"}}}
	};

	int max = -1;
	GOBS gobs_ans;
	map<GOBS, double>::const_iterator it = _gobs.begin();
	while (it != _gobs.end()) {
		GOBS gobs = it->first;
		if (gobs_phase(gobs)) {
			GOBSBAND tmp = str2gobsband(gobs2str(gobs));
			if (tmp == b) {
				t_gobs gobs_temp(gobs); gobs_temp.gobs2to3(_gsys);
				int loc = _order_attr[_gsys][b].find(gobs2str(gobs_temp.gobs())[2]);
				if (loc > max) {
					gobs_ans = gobs;
					max = loc;
				}
			}
		}
		it++;
	}
	if (max == -1) {
		return X;
	}
	else {
		return gobs_ans;
	}
}


   
// return coefficients (c1,c2) of the ionosphere-free linear combination
// ----------
int t_gobsgnss::_coef_ionofree(GOBSBAND b1, double& c1, 
                               GOBSBAND b2, double& c2 )
{
  if( b1 == b2 ){ c1 = c2 = 0.0; return -1; }
   
  double fac1 = this->frequency( b1 );
  double fac2 = this->frequency( b2 );
  
  c1 =   fac1*fac1 / (fac1*fac1 - fac2*fac2);
  c2 = - fac2*fac2 / (fac1*fac1 - fac2*fac2);
   
#ifdef DEBUG
  cout << " coefs (D) = " << fixed << setprecision(3)
       << setw(15) << fac1
       << setw(15) << fac2
       << setw(15) << c1
       << setw(15) << c2
       << endl;
#endif

  return 0;
}

// return coefficients (c1,c2) of the geometry-free linear combination
// ----------
int t_gobsgnss::_coef_geomfree(GOBSBAND b1, double& c1,
	GOBSBAND b2, double& c2)
{
	if (b1 == b2) { c1 = c2 = 0.0; return -1; }
	c1 = 1.0;
	c2 = -1.0;

	return 0;
}
// -------------------------------------------------------------------------------------------
// t_obscmb class 
// -------------------------------------------------------------------------------------------

bool t_obscmb::operator<(const t_obscmb& t) const
{
  return ( this->first.type() < t.first.type() &&
           this->first.band() < t.first.band() &&
           this->first.attr() < t.first.attr() );
}

} // namespace
