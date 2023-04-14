
/* ----------------------------------------------------------------------
 * G-Nut - GNSS software development library
 * 
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)

  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
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
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <math.h>

#include "gdata/gnavglo.h"
#include "gutils/gtypeconv.h"

using namespace std;

namespace gnut {


t_gnavglo::t_gnavglo()
  : t_gnav(),
    _iodc(0),
    _toc(t_gtime::UTC)
{
  gtrace("t_gnavglo::t_gnavglo");
   
  id_type(t_gdata::EPHGLO);
  id_group(t_gdata::GRP_EPHEM);
   
  _maxEphAge = MAX_GLO_TIMEDIFF; // a bit above 900; //[s]
  _min_step = 10;
}


t_gnavglo::~t_gnavglo(){
  gtrace("t_gnavglo::~t_gnavglo");
}


int t_gnavglo::channel() const
{
  gtrace("t_gnavglo::channel");   
   
  if( _freq_num < -7 || _freq_num > 13){
    if( _log ){
      cout << "t_gnavglo: GLONASS " << _sat << " channel not valid (return 256) " << _freq_num  << endl;
    }
    return 256;
  }
   
  /* if( nav.iodc < 0 || 1023 < nav.iodc ){	 */
  /*   cout << "rinex nav invalid iodc: iodc" << nav.iodc << endl; */
  /*   return -1; */
  /* } */
   
  return _freq_num;
}


int t_gnavglo::data2nav( string sat, const t_gtime& ep, const t_gnavdata& data ){

  gtrace("t_gnavglo::data2nav");
   
#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();
   
   if( sat.find("R") != string::npos ){
     _sat = sat;
   }else{
    ostringstream tmp;
    tmp << setw(1) << 'R' << setfill('0') << setw(2) << sat;
    _sat = tmp.str();
   }
  
   _epoch = ep;
   _toc   = ep;
   _iodc  = _iod();
   _tau   = -data[0];    // in RINEX is stored -tauN
   _gamma = data[1];
   _tki   = data[2];
    if (_tki < 0) _tki += 86400;

   _x     = data[3] * 1.e3;
   _x_d   = data[4] * 1.e3;
   _x_dd  = data[5] * 1.e3;
   
   _health = data[6]; 
     
   _y     = data[7] * 1.e3;
   _y_d   = data[8] * 1.e3;
   _y_dd  = data[9] * 1.e3;
   
   _freq_num = (int)data[10];
   
   _z     = data[11] * 1.e3;
   _z_d   = data[12] * 1.e3;
   _z_dd  = data[13] * 1.e3;
   
   _E     = data[14];      

   _gmutex.unlock(); return 0;
}


int t_gnavglo::nav2data( t_gnavdata& data ){
  
  gtrace("t_gnavglo::nav2data");
   
#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

   if( ! this->_valid() ) return -1;
   
   data[0]  = - _tau; // in RINEX is stored -tauN
   data[1]  = _gamma;
   data[2]  = _tki;   //  if (_tki < 0) _tki += 86400;

   data[3]  = _x     / 1.e3;
   data[4]  = _x_d   / 1.e3;
   data[5]  = _x_dd  / 1.e3;

   data[6]  = _health;
     
   data[7]  = _y     / 1.e3;
   data[8]  = _y_d   / 1.e3;
   data[9]  = _y_dd  / 1.e3;
   
   data[10] = _freq_num;
   
   data[11] = _z     / 1.e3;
   data[12] = _z_d   / 1.e3;
   data[13] = _z_dd  / 1.e3;
   
   data[14] = _E;

  _gmutex.unlock(); return 0;
}


ColumnVector t_gnavglo::_deriv(const ColumnVector& xx, const t_gtriple& acc)
{
  gtrace("t_gnavglo::_deriv");   
   
   t_gtriple crd(xx.rows(1,3));
   t_gtriple vel(xx.rows(4,6));   
   
   double r = crd.crd_cvect().norm_Frobenius();
   
   double k1 = -GM_PZ90/(r*r*r);
   double k2 = (3.0/2.0) * C20_PZ90 * (GM_PZ90 * Aell_PZ90 * Aell_PZ90) / pow(r,5);

   ColumnVector xxdot(6);
   
   xxdot(1) = vel[0];
   xxdot(2) = vel[1];
   xxdot(3) = vel[2];
   xxdot(4) = k1*crd[0] + k2*(1.0-5.0*crd[2]*crd[2]/(r*r))*crd[0] + OMEGA*OMEGA*crd[0] + 2*OMEGA*vel[1] + acc[0];
   xxdot(5) = k1*crd[1] + k2*(1.0-5.0*crd[2]*crd[2]/(r*r))*crd[1] + OMEGA*OMEGA*crd[1] - 2*OMEGA*vel[0] + acc[1];
   xxdot(6) = k1*crd[2] + k2*(3.0-5.0*crd[2]*crd[2]/(r*r))*crd[2] + acc[2];
     
   return xxdot;
}



int t_gnavglo::_iod() const
{
  gtrace("t_gnavglo::_iod()");
   
  t_gtime gloTime = _toc + 3*3600.0;  // toc in GLO (if toc in UTC +3*3600.0;)
   
  int iod = int(gloTime.sod() / 900);
   
  // doy is added for making iod unique over days
  int doy = _toc.doy();  iod += doy;
   
  return iod;
}


} // namespace
