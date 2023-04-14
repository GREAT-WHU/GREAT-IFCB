/* ----------------------------------------------------------------------
 * G-Nut - GNSS software development library
 *
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
  This file is part of the G-Nut C++ library.
-*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "gdata/gnavgps.h"
#include "gutils/gtypeconv.h"

using namespace std;

namespace gnut {

// constructor
// ----------
t_gnavgps::t_gnavgps()
  : t_gnav(),
    _iode(0),
    _iodc(0),
    _health(0),
    _toe(t_gtime::GPS),
    _toc(t_gtime::GPS),
    _tot(t_gtime::GPS),
    _a(0.0),
    _e(0.0),
    _m(0.0),
    _i(0.0),
    _idot(0.0),
    _omega(0.0),
    _OMG(0.0),
    _OMGDOT(0.0),
    _dn(0.0),
    _crc(0.0),
    _cic(0.0),
    _cuc(0.0),
    _crs(0.0),
    _cis(0.0),
    _cus(0.0),
    _f0(0.0),
    _f1(0.0),
    _f2(0.0),
    _acc(0),
    _fit(0.0),
    _code(0.0),
    _flag(0.0)
{ 
  gtrace("t_gnavgps::t_gnavgps");
   
  _tgd[0] = _tgd[1] = _tgd[2] = _tgd[3] = 0.0;

  id_type(t_gdata::EPHGPS);
  id_group(t_gdata::GRP_EPHEM);
}


// desctructor
// ----------
t_gnavgps::~t_gnavgps(){
  gtrace("t_gnavgps::~t_gnavgps");
}


// operator <
// .. priority: satid, tot, iode
// ----------
/*
int t_gnavgps::operator<(const t_gnavgps& data) const
{       
  int _dtot = int( tot.diff( data.tot ) );
  int _dsat = strcmp( sat, data.sat );
  int _diod = int( iode - data.iode );

  if( _dsat != 0 ) return (_dsat < 0);  // satellite name
  if( _dtot != 0 ) return (_dtot < 0);  // time of transmition
  if( _diod != 0 ) return (_diod < 0);  // iode of ephemeris

  return false;
}
*/

// operator ==
// .. distinguish: satid, tot, iode
// ----------
/*
int t_gnavgps_struct::operator==(const t_gnavgps& data) const
{       
  int _dtot = int( tot.diff( data.tot ) );
  int _dsat = strcmp( _sat, data.sat );
  int _diod = int( _iode - data.iode );
  
  return ( _dtot == 0 && _dsat == 0 && _diod == 0 );
}
*/

// print function
// ----------
string t_gnavgps::line() const
{
  gtrace("t_gnavgps::line");

  int w = 20;
  ostringstream tmp;

  tmp  << " " << setw(3) << sat()
       << " " << _toc.str("%Y-%m-%d %H:%M:%S")
//     << " " << _toe.str("%Y-%m-%d %H:%M:%S")
//     << " " << _tot.str("%Y-%m-%d %H:%M:%S")
       << scientific << setprecision(12)
       << setw(w) << _f0
       << setw(w) << _f1
       << setw(w) << _f2
       << setw(w) << _iode/1.0
       << setw(w) << _iodc/1.0
       << setw(w) << _acc
       << setw(w) << _health/1.0
       << setw(w) << _code
       << setw(w) << _flag
       << setw(w) << SQRT(_a)
       << setw(w) << _e
       << setw(w) << _i
       << setw(w) << _m
       << setw(w) << _idot
       << setw(w) << _omega
       << setw(w) << _OMG
       << setw(w) << _OMGDOT
       << setw(w) << _dn
       << setw(w) << _crc
       << setw(w) << _cic
       << setw(w) << _cuc
       << setw(w) << _crs
       << setw(w) << _cis
       << setw(w) << _cus
       << setw(w) << _fit
       << setw(w) << _tgd[0]
       << setw(w) << _tgd[1]
       << setw(w) << _tgd[2]
       << setw(w) << _tgd[3]
       << endl;

  return tmp.str();
}


// print function
// ----------
string t_gnavgps::linefmt() const
{
  gtrace("t_gnavgps::linefmt");
   
  ostringstream tmp;

  tmp  << " " << setw(3) << sat() << fixed
       << " " << _toc.str("%Y-%m-%d %H:%M:%S")
//     << " " << _toe.str("%Y-%m-%d %H:%M:%S")
//     << " " << _tot.str("%Y-%m-%d %H:%M:%S")
       << fixed    << setprecision(0)
       << setw( 8) << _tot - _toc
       << setw( 8) << _tot - _toe
       << setw( 4) << _iode
       << setw( 4) << _health
       << " |"
       << setw( 9) << setprecision(0) << _a      *1e0  // 1 [m]
       << setw( 8) << setprecision(3) << _e      *1e3  // 2
       << setw( 8) << setprecision(3) << _i      *1e3  // 3
       << setw( 7) << setprecision(3) << _m      *1e0  // 4
       << " |"
       << setw( 7) << setprecision(3) << _idot   *1e9  // 5
       << setw( 9) << setprecision(5) << _omega  *1e0  // 6
       << setw( 9) << setprecision(5) << _OMG    *1e0  // 7
       << setw( 9) << setprecision(5) << _OMGDOT *1e9  // 8
       << setw( 7) << setprecision(3) << _dn     *1e9  // 9
       << " |"
       << setw( 9) << setprecision(3) << _crc    *1e0  // 10
       << setw( 7) << setprecision(3) << _cic    *1e6  // 11
       << setw( 7) << setprecision(3) << _cuc    *1e6  // 12
       << setw( 9) << setprecision(3) << _crs    *1e0  // 13
       << setw( 7) << setprecision(3) << _cis    *1e6  // 14
       << setw( 7) << setprecision(3) << _cus    *1e6  // 15
       << " |"
       << setw( 9) << setprecision(3) << _tgd[0] *1e9  // 15
       << setw( 9) << setprecision(3) << _tgd[1] *1e9  // 15
       << setw( 9) << setprecision(3) << _tgd[2] *1e9  // 15
       << setw( 9) << setprecision(3) << _tgd[3] *1e9  // 15
    ;

  return tmp.str();
}


// check message // NEED TO IMPROVE !
// ----------
int t_gnavgps::chk(set<string>& msg)
{
  gtrace("t_gnavgps::chk");   
  _gmutex.lock(); 
  
  if( ! _healthy() ) {
    msg.insert("Mesg: "+_sat+" nav unhealthy satellite " + _toc.str_ymdhms());
  }   
  
  if( (_tot - _toc) > 86400 || (_toc - _tot) > t_gnav::nav_validity(GPS)){
    msg.insert("Issue: "+_sat+" nav large difference [tot-toc] " + dbl2str(_tot-_toc) + " s " + _tot.str_ymdhms() + _toc.str_ymdhms(" "));
    _validity = false;
  }

  if( (_tot - _toe) > 86400 || (_toe - _tot) > t_gnav::nav_validity(GPS)){
    msg.insert("Issue: "+_sat+" nav large difference [tot-toe] " + dbl2str(_tot-_toe) + " s " + _tot.str_ymdhms() + _toe.str_ymdhms(" "));
    _validity = false;
  }
  
  if( _toe == FIRST_TIME ){
    msg.insert("Issue: "+_sat+" nav invalid [toe] " + _toe.str_ymdhms());
    _validity = false;
  }

  if( _toe != _toc  ){  // just some cases are found, however several cases identified as problematic NAV
    msg.insert("Issue: "+_sat+" nav inequal [toe/toc] " + _toe.str_ymdhms() + " "  + _toc.str_ymdhms());
    _validity = false;
  }

  int sod_frac = _toc.sod()%7200;
  if(      sod_frac == 0    ||
           sod_frac == 7184 ||
           sod_frac == 7168 ||
           sod_frac == 7152 ||
           sod_frac == 7136 ||
           sod_frac == 7120 ){}
  else{
    msg.insert("Issue: "+_sat+" nav unexpected [toc] " + _toc.str_ymdhms());
//     cerr << "Issue: "+_sat+" nav unexpected [toc] " + _toc.str_ymdhms() << endl;
    _validity = false;
  }
  
  if( _iode < 0 ||  255 < _iode){
    msg.insert("Issue: "+_sat+" nav invalid [iode] " + _toe.str_ymdhms());
    _validity = false;
  }
   
  if( _iodc < 0 || 1023 < _iodc ){	
    msg.insert("Issue: "+_sat+" nav invalid [iodc] " + _toe.str_ymdhms());
    _validity = false;
  }  
  
  if( _a < 20000000.0 || _a > 30000000.0 ){ // really happened when e.g. file is corrupted
    msg.insert("Issue: "+_sat+" nav invalid [a] " + _toe.str_ymdhms());
    _validity = false;
  }

  if( _acc == -16 || _acc == 15 ){
    msg.insert("Issue: "+_sat+" nav no accuracy prediction available " + _toe.str_ymdhms());
    _validity = false;
  }   
  
  _gmutex.unlock(); return 0;
}   
   

// t is GPS time of transmission,
// i.e. GPS time corrected for transit time (range/speed of light)
// ----------
int t_gnavgps::pos( const t_gtime& t, double xyz[], double var[], double vel[], bool chk_health )
{
  gtrace("t_gnavgps::pos");

  if( sat().empty() ) return -1; // check if valid
  if( chk_health && _healthy() == false ){
     if( _log ) _log->comment(3,"gephgps","not healthy sat " + sat() + " excluded from pos calculation " + t.str_ymdhms());   
     return -1;  // HEALTH NOT OK
  }
   
            xyz[0] = xyz[1] = xyz[2] = 0.0;
  if( var ) var[0] = var[1] = var[2] = 0.0;
  if( vel ) vel[0] = vel[1] = vel[2] = 0.0;

#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  double Tk = t.diff(_toe);                         // T - toe difference
  while( Tk >  302400.0 ){ Tk -= 604800.0; }        // check the correct GPS week
  while( Tk < -302400.0 ){ Tk += 604800.0; }        // check the correct GPS week
 
  double Ek,dEk;
  _ecc_anomaly( Tk, Ek, dEk );                      // eccentric anomaly and its derivative

  double V = atan2(sqrt(1.0 - pow(_e,2.0))*sin(Ek), 
		                     cos(Ek) - _e); // true anomaly
  double U0 = V + _omega;                           // argument of latitude
   
  double sin2U = sin(2*U0);
  double cos2U = cos(2*U0);
   
  double Rk = _a*(1.0 - _e * cos(Ek));              // radius               
  double Uk = U0;                                   // argument of latitude 
  double Ik = _i + _idot*Tk;                        // inclination          
   
         Rk += _crs*sin2U + _crc*cos2U;             // corrected radius
         Ik += _cis*sin2U + _cic*cos2U;             // corrected inclination
         Uk += _cus*sin2U + _cuc*cos2U;             // corrected argument of latitude
    
  double cosU   = cos(Uk);
  double sinU   = sin(Uk);
  double cosI   = cos(Ik);
  double sinI   = sin(Ik);
   
  double x = Rk*cosU;                               // position in orbital plane
  double y = Rk*sinU;
    
  double OMG = _OMG + (_OMGDOT - OMGE_DOT)*Tk 
                      - OMGE_DOT*_toe.sow();        // corrected long. of asc. node
   
  double cosOMG = cos(OMG);
  double sinOMG = sin(OMG);
   
  xyz[0] = x*cosOMG - y*cosI*sinOMG;                // earth-fixed coordinates [m]
  xyz[1] = x*sinOMG + y*cosI*cosOMG;
  xyz[2] = y*sinI;

  // error variance
  if( var ){ 
        *var = SQRT( _acc );
  }

  // velocities
  if( vel ){
    double n  = _mean_motion();

    double tanv2 = tan(V/2);
    double dEdM  = 1 / (1 - _e*cos(Ek));
    double dotv  = sqrt((1.0 + _e)/(1.0 - _e)) / cos(Ek/2)/cos(Ek/2) / (1 + tanv2*tanv2) 
               * dEdM * n;
    double dotu  = dotv + (-_cuc*sin2U + _cus*cos2U)*2*dotv;
    double dotom = _OMGDOT - OMGE_DOT;
    double doti  = _idot + (-_cic*sin2U + _cis*cos2U)*2*dotv;
    double dotr  = _a * _e*sin(Ek) * dEdM * n
                + (-_crc*sin2U + _crs*cos2U)*2*dotv;
    double dotx  = dotr*cosU - Rk*sinU*dotu;
    double doty  = dotr*sinU + Rk*cosU*dotu;

    vel[0]  =     cosOMG*dotx  -    cosI*sinOMG*doty
             - x *sinOMG*dotom - y *cosI*cosOMG*dotom
                               + y *sinI*sinOMG*doti;

    vel[1]  =     sinOMG*dotx  +    cosI*cosOMG*doty
             + x *cosOMG*dotom - y *cosI*sinOMG*dotom
                               - y *sinI*cosOMG*doti;

    vel[2]  =     sinI*doty    + y *cosI*doti;     
  }
   
#ifdef DEBUG      
   cout << "gnavgps "<< sat()
        << " CRD " << fixed << setprecision(3)
        <<  "  "   << t.str_ymdhms()
        << " X "   << setw(14) << xyz[0]
        << " Y "   << setw(14) << xyz[1]
        << " Z "   << setw(14) << xyz[2]
        << endl;
#endif

  _gmutex.unlock(); return 0;
}


//
// t is GPS time of transmission,
// i.e. GPS time corrected for transit time (range/speed of light)
// NOT CORRECTED FOR 2nd-order relativistic effect
// ----------
int t_gnavgps::clk( const t_gtime& t, double* clk, double* var, double* dclk, bool chk_health )
{
  gtrace("t_gnavgps::clk");

  if( sat().empty() ) return -1;  // not valid !!!
   
  if( chk_health && _healthy() == false ){
     if( _log ) _log->comment(3,"gephgps","not healthy sat " + sat() + " excluded from clk calculation " + t.str_ymdhms());   
     return -1;  // HEALTH NOT OK
  }
   
#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  double Tk = t.diff(_toc);                      // difference from clk(!) epoch ref. time
  while( Tk >  302400.0 ){ Tk -= 604800.0; }     // check the correct GPS week
  while( Tk < -302400.0 ){ Tk += 604800.0; }     // check the correct GPS week

  double Ek, dEk;
  _ecc_anomaly( Tk, Ek, dEk );                   // eccentric anomaly
   
// SV time correction (including periodic relativity correction)
//  cout << "RELATIVISTIC EFFECT  uSec: " << - 2.0*sqrt(GM_WGS84*_a)*_e*sin(Ek)/CLIGHT/CLIGHT*1000000 << endl;

//  *clk = _f0 + Tk*_f1 + Tk*Tk*_f2  - 2.0*sqrt(GM_WGS84*_a)*_e*sin(Ek)/CLIGHT/CLIGHT;  // [sec]

// NOT CORRECTED FOR 2nd-order relativistic effect - at user side
  *clk = _f0 + Tk*_f1 + Tk*Tk*_f2;
   
  // check impossible clock correction
  if( clk && *clk > 1.0 ){
    cerr << "* warning: too large clock - skipped: " << fixed << setprecision(0) << *clk << endl; 
    _gmutex.unlock(); return -1;
  }
   
  // error variance
  if( var ){
     *var = SQRT( _acc );
  }

  // velocities
  if( dclk ){
     *dclk = 0.0;
  }

  _gmutex.unlock(); return 0;
}


// convert general gnavdata structure to gnavgps_struct
// 
//   ToC, ToE, ToT not definitely implemented!
// ----------
int t_gnavgps::data2nav( string sat, const t_gtime& ep, const t_gnavdata& data )
{
  gtrace("t_gnavgps::data2nav");

#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  char tmp[12];

  if( sat.substr(0,1) == "G" )  _sat =     sat;
  else                          _sat = "G"+sat;

  _toc = ep; // time of clocks (tot) .. RINEX reference time
  _epoch = ep;

  int data_21 = static_cast<int>(data[21]);    // toe week
  int data_11 = static_cast<int>(data[11]);    // toe sow
  int data_27 = static_cast<int>(data[27]);    // tot sow

  if( data_21 < 0 ){
   _toe = FIRST_TIME; // cerr << " incorrect GPS week number !\n";
   _gmutex.unlock(); return -1;
  }

  sprintf(tmp, "%4i %6i", data_21, data_11);  _toe.from_str("%W %v", tmp);   // time of ephemeris    (toe)
  // yqyuan changed
  //if (data_27 == 0.9999e9)   _tot = _toe;
  if (data_27 == 0.9999e9 || data_27 == 0)  _tot = _toe;
  else if (data_27 == 0.0) _tot = _toe; //20200321 lvhb if tot sow is empty,but there for novatel nav
  else { sprintf(tmp, "%4i %6i", data_21, data_27);  _tot.from_str("%W %v", tmp); }   // time of transmission (tot)

  if( fabs(_tot - _toe - 604800) < MAX_GPS_TIMEDIFF ) _tot.add_secs(-604800);   // adjust tot to toe gwk
  if( fabs(_tot - _toe + 604800) < MAX_GPS_TIMEDIFF ) _tot.add_secs(+604800);   // adjust tot to toe gwk

  _f0     =      data[0];
  _f1     =      data[1];
  _f2     =      data[2];
   
  _iode   = static_cast<int>(data[3]);
  _iodc   = static_cast<int>(data[26]);
  _health = static_cast<int>(data[24]);
   
  _a      =  SQR(data[10]);
  _e      =      data[8];
  _m      =      data[6];
  _i      =      data[15];
  _idot   =      data[19];
  _OMG    =      data[13];
  _OMGDOT =      data[18];
  _omega  =      data[17];
  _dn     =      data[5];
   
  _crs    =      data[4];
  _cus    =      data[9];
  _cis    =      data[14];
  _crc    =      data[16];
  _cuc    =      data[7]; 
  _cic    =      data[12];

  _acc    =      static_cast<int>(data[23]);
  _tgd[0] =      data[25];
  _fit    =      data[28];
  _code   =      data[20];
  _flag   =      data[22];

  _gmutex.unlock(); return 0;
}


// convert gnav_gps element to general gnavdata
// ----------
int t_gnavgps::nav2data( t_gnavdata& data )
{
  gtrace("t_gnavgps::nav2data");

#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gmutex.lock();

  if( ! this->_valid() ) return -1;
   
  data[0]  = _f0;
  data[1]  = _f1;
  data[2]  = _f2;   
  data[3]  = _iode;
  data[4]  = _crs;
  data[5]  = _dn;
  data[6]  = _m;
  data[7]  = _cuc;
  data[8]  = _e;
  data[9]  = _cus;
  data[10] = sqrt(_a);
  data[11] = _toe.sow();
  data[12] = _cic;
  data[13] = _OMG;
  data[14] = _cis;
  data[15] = _i;
  data[16] = _crc;
  data[17] = _omega;
  data[18] = _OMGDOT;
  data[19] = _idot;
  data[20] = _code;
  data[21] = _toe.gwk();
  data[22] = _flag;
  data[23] = _acc;           
  data[24] = _health;
  data[25] = _tgd[0];
  data[26] = _iodc;
  data[27] = _tot.sow();
  data[28] = _fit;

  while( data[27] <       0 ){ data[27] += 604800; }
  while( data[27] > +604800 ){ data[27] -= 604800; }

//cout << "=> DATA27: " << data[27] << endl;

  _gmutex.unlock(); return 0;
}

bool t_gnavgps::chktot(const t_gtime& t)
{
  if(t > _tot) return true;
  else return false;
}

// healthy check
// ----------
bool t_gnavgps::_healthy() const
{
   if( _health == 0 ) return true;
   return false;
}

// URA ED from broadcasted URA index
double t_gnavgps::_ura_ed() const
{
  if(_acc == 1)       return 2.8;
  else if(_acc == 3)  return 5.7;
  else if(_acc == 5)  return 11.3;
   
  double ura_nom = 0;
  if( _acc <= 6 && _acc > -16 ){
     ura_nom = pow(2, 1 + (double)_acc/2);
  }else if ( _acc > 6 && _acc < 15 ){
     ura_nom = pow(2, _acc - 2);
  }
   
   return ura_nom;
}
   
// mean_motion
// ----------
double t_gnavgps::_mean_motion()
{
  gtrace("t_gnavgps::_mean_motion");   
   
  double n0 = sqrt(GM_WGS84/(pow(_a,3.0)));      // computed mean motion [rad/sec]
  double n  = n0 + _dn;                          // corrected mean motion

  return n;
}


// eccentric anomaly
// ----------
void t_gnavgps::_ecc_anomaly( double dt, double& Ek, double& dEk )
{
  gtrace("t_gnavgps::_ecc_anomaly");   
   
  double n  = _mean_motion();                    // eccentric anomaly
  double Mk = _m + n*dt;                         // mean anomaly

  Ek = Mk;
  dEk = n;
  for( double E = 0; fabs(Ek-E)*_a > 0.001; ){
    E  = Ek;
    Ek = Mk + _e*sin(E);                         // kepler equation for eccentric anomaly [rad]
   dEk = n  + _e*cos(Ek)*dEk;                    // derivatives w.r.t. time
  }
}

} // namespace
