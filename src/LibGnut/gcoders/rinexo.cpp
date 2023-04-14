
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <memory>
#include <algorithm>

#include "gcoders/rinexo.h"
#include "gdata/gobsgnss.h"
 
using namespace std;

namespace gnut {

t_rinexo::t_rinexo( t_gsetbase* s, string version, int sz )
  : t_rinexo3( s, version, sz )
{}


int t_rinexo::decode_head( char* buff, int sz, vector<string>& errmsg)
{ 
  gtrace("t_rinexo::decode_head");

  _mutex.lock();

  int add = t_gcoder::_add2buffer(buff, sz);     
  if( add <  0 ){ _mutex.unlock(); return -1; };
  if( add == 0 ){ _mutex.unlock(); return  0; };

  _complete = true;
  _consume = 0;
  _tmpsize = 0;
  _line = "";

  this->_decode_head();

  _mutex.unlock(); return _consume;
}

   
int t_rinexo::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
{  
  gtrace("t_rinexo::decode_data");

  if( _hdr ) return -1; // thin execution, header only
 
  // filter-out unknown marker/receiver
  if( _rec.size() == 0 ){ _rec.insert(_site); }
  if( _rec.find( _site             ) == _rec.end() &&
      _rec.find( _site.substr(0,4) ) == _rec.end() ){
     if( _log ) _log->comment(2,"rinexo", _site + " marker name not found in configuration [skipped]." );
                           mesg(GMESSAGE, _site + " marker name not found in configuration!");
     ++_irc;
     return -1;
  }

  _flag     = '0';                   // special event flag
  _line     = "";                    // working line
  _nsat     = 0;                     // special event flag
  _tmpsize  = 0;                     // currently processing byte counter
  _consume  = 0;                     // total byte counter
  _complete = true;                  // flag for completed block
   
  _mutex.lock();

  // complete main buffer
  int add = t_gcoder::_add2buffer(buff, sz);
  if( add <  0 ){ _mutex.unlock(); return -1; };
  if( add == 0 ){ _mutex.unlock(); return  0; };
 
  this->_decode_data();   

  cnt = _count;
  _mutex.unlock(); return _consume;
}


int t_rinexo::_decode_head()
{ 
  gtrace("t_rinexo::_decode_head");
   
  while( _complete && ( ( _tmpsize = t_gcoder::_getline( _line )) >= 0 ) ){

#ifdef DEBUG
    cout << "LINE: " << _tmpsize << " " << _consume << " :" << _line; cout.flush();
#endif

    if( _tmpsize <= 61 ) break;
     
    _complete = true;

    // -------- "RINEX VERSION" --------
    if( _line.find("RINEX VERSION",60) != string::npos ){          // first line
      if( _line[20] != 'O' ){      // A1
        if( _log ){ _log->comment(0,"rinexo","Error: not RINEX observation file");  }
        else{               cerr << "rinexo - Error: not RINEX observation file\n"; }
                                 mesg(GERROR,"Error: not RINEX observation file!");  ++_irc;
        return (_consume = -1);
      }
      _version = trim(_line.substr(0,9)); // F9.2
      substitute(_version, " ", "");

      _rnxhdr.rnxver(_version);
      _rnxhdr.rnxsys( toupper(_line[40]) );     // A1 (G=GPS, R=GLO, E=GAL, S=SBAS, M=Mix)

      if( _log && substitute(_version, " ", "") > 0 ){ 
        _log->comment(2,"rinexo", "reading VER: " + _version + " SYS: " + string(1,_rnxhdr.rnxsys()) );
      }
      
      if( _rnxhdr.rnxsys() == ' ' ){
        if( _log )  _log->comment(0, "rinexo", "Warning: RINEX SYS not defined, GPS set as default" );
        if( _version < "3.00" ){ mesg(GWARNING,"Warning: RINEX SYS not defined!");          }
        else{                    mesg(GERROR,    "Error: RINEX SYS not defined!");  ++_irc; }
        _rnxhdr.rnxsys( 'G' );
      }
      _csys = string(1,_rnxhdr.rnxsys());

    // -------- "END OF HEADER" --------
    }else if( _line.find("END OF HEADER",60) != string::npos ){
      if( _log ) _log->comment(2,"rinexo","END OF HEADER ");
      t_gcoder::_consume(_tmpsize);
      _rnxhdr.mapobs(_mapobs);
      _rnxhdr.mapcyc(_mapcyc);
      _rnxhdr.glofrq(_glofrq);
      _rnxhdr.globia(_globia);
      _fill_head();
      if(      _version < "3.00" ) t_rinexo2::_check_head();
      else if( _version < "4.00" ) t_rinexo3::_check_head();
      return(_consume = -1);
    }

    else if( _version < "3.00" ) t_rinexo2::_decode_head();
    else if( _version < "4.00" ) t_rinexo3::_decode_head();

    _rnxhdr.comment(_comment); 


    // -------- CONSUME --------
    if( _complete ) _consume += t_gcoder::_consume(_tmpsize);    
    else break;
  }

  return _consume;
}


int t_rinexo::_decode_data()
{  
  gtrace("t_rinexo::_decode_data");
   
  while( _complete && ( ( _tmpsize = t_gcoder::_getline( _line ) ) >= 0 ) ){
    
    _complete = true;
    _vobs.clear();


    // -------- DECODE DATA -------
    if(      _version < "3.00" ) t_rinexo2::_decode_data();
    else if( _version < "4.00" ) t_rinexo3::_decode_data();

  } // loop over lines
   

  return _consume;
}

} // namespace
