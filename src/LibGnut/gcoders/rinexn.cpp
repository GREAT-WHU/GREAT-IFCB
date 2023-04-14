
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

#include "gdata/gnav.h"
#include "gdata/geph.h"
#include "gdata/gnavgps.h"
#include "gdata/gnavgal.h"
#include "gdata/grxnhdr.h"
#include "gall/gallnav.h"
#include "gutils/gtime.h"
#include "gcoders/rinexn.h"
#include "gutils/gtypeconv.h"
#include "gutils/gfileconv.h"
#include "gutils/gsys.h"

using namespace std;

namespace gnut {


t_rinexn::t_rinexn( t_gsetbase* s, string version, int sz )
: t_gcoder( s, version, sz )
{
  _gnsssys     = 'G';
  _check_dt = FIRST_TIME;
  
  _gset(s); // HAVE TO BE EXPLICITLY CALLED HERE (AT THE END OF CONSTRUCTOR)
}


int t_rinexn::decode_head( char* buff, int sz, vector<string>& errmsg)
{ 

#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _mutex.lock();

  if( t_gcoder::_add2buffer( buff, sz) == 0 ){ _mutex.unlock(); return 0; };  

  string line;
  int consume = 0;
  int tmpsize = 0;
  while( ( tmpsize = t_gcoder::_getline( line )) >= 0 ){
    
    consume += tmpsize;
    
    // -------- "RINEX VERSION" --------
    if( line.find("RINEX VERSION",60) != string::npos ){          // first line
      switch ( line[20] ){
        case 'N': _gnsssys = 'G'; break; // Navigation data - according to Rinex specification
        case 'G': _gnsssys = 'R'; break; // GLONASS NAVIGATION - occures sometimes in brdc
        default : { string lg("warning: not rinex navigation data file");
          if( _log )  _log->comment(0,"rinexn", lg);
          else  cerr << lg;
          _mutex.unlock(); return -1;
        }
      }
       
      switch ( line[40] ){
        case 'G': _gnsssys = 'G'; break; // GPS
        case 'R': _gnsssys = 'R'; break; // GLONASS
        case 'E': _gnsssys = 'E'; break; // GALILEO
        case 'J': _gnsssys = 'J'; break; // QZSS
        case 'S': _gnsssys = 'S'; break; // SBAS
        case 'I': _gnsssys = 'I'; break; // IRNSS
        case 'M': _gnsssys = 'M'; break; // MIXED
        case ' ': { 
          if( line[20] == 'N' ) _gnsssys = 'G';
          if( line[20] == 'G' ) _gnsssys = 'R';
          if( _log ) _log->comment(0,"rinexn","warning - RINEXN system not defined, used "+t_gsys::char2str(_gnsssys));
          break; 
        }
        default : { 
          string lg("warning: not supported satellite system "+line.substr(40,1));
          if( _log ) _log->comment(0,"rinexn", lg); 
          else       cerr << lg << endl;
        }
      }
       


      _version = trim(line.substr(0,9));

      _rxnhdr.path( _fname );

      if( _log && substitute(_version, " ", "") > 0 ){
        _log->comment(2,"rinexn", "reading VER: " + _version + " SYS: " + string(1,_gnsssys) );
      }

    // -------- "PGM / RUN BY / DATE" --------
    }else if( line.find("PGM / RUN BY / DATE",60) != string::npos ){
      _rxnhdr.program( trim(line.substr( 0,20)) );
      _rxnhdr.runby  ( trim(line.substr(20,20)) );
      t_gtime gtime(t_gtime::UTC);
      if( line.substr(56,3) != "UTC" ) gtime.tsys(t_gtime::LOC);
      
      if(      gtime.from_str("%Y%m%d %H%M%S",    line.substr(40,15)) == 0 ) ;
      else if( gtime.from_str("%Y-%m-%d %H-%M-%S",line.substr(40,20)) == 0 ) ;
      else{    gtime = FIRST_TIME; }
       _rxnhdr.gtime(gtime);
      
      if( _log ) _log->comment(2,"rinexn","PGM / RUN BY / DATE: " + _rxnhdr.program()
                                                            + " " + _rxnhdr.runby()
                                                            + " " + _rxnhdr.gtime().str_ymdhms() );

    // -------- "IONOSPHERIC CORR" --------
    }else if( line.find("IONOSPHERIC CORR",60) != string::npos ){
     
      IONO_CORR   IO = str2iono_corr(line.substr(0,4)); 
      t_iono_corr io;

      io.x0 = strSci2dbl(line.substr(5+ 0, 12));
      io.x1 = strSci2dbl(line.substr(5+12, 12));
      io.x2 = strSci2dbl(line.substr(5+24, 12));
      io.x3 = strSci2dbl(line.substr(5+36, 12));
                                     
      _rxnhdr.iono_corr( IO, io );

      if( _log ) _log->comment(2,"rinexn","IONOSPHERIC CORR " + iono_corr2str(IO)
                                                              + dbl2str(io.x0)
                                                              + dbl2str(io.x1)
                                                              + dbl2str(io.x2)
                                                              + dbl2str(io.x3)
                                                        + " " + base_name(_fname)
                              );


    // -------- "ION ALPHA" --------
    }else if( line.find("ION ALPHA",60) != string::npos ){
     
      IONO_CORR   IO = IO_GPSA;
      t_iono_corr io;

      io.x0 = strSci2dbl(line.substr(2+ 0, 12)); 
      io.x1 = strSci2dbl(line.substr(2+12, 12)); 
      io.x2 = strSci2dbl(line.substr(2+24, 12)); 
      io.x3 = strSci2dbl(line.substr(2+36, 12)); 
                                     
      _rxnhdr.iono_corr( IO, io );

      if( _log ) _log->comment(2,"rinexn","ION ALPHA " + iono_corr2str(IO)
                                                       + dbl2str(io.x0)
                                                       + dbl2str(io.x1)
                                                       + dbl2str(io.x2)
                                                       + dbl2str(io.x3)
                                                 + " " + base_name(_fname)
                              );

    // -------- "ION BETA" --------
    }else if( line.find("ION BETA",60) != string::npos ){
     
      IONO_CORR   IO = IO_GPSB;
      t_iono_corr io;

      io.x0 = strSci2dbl(line.substr(2+ 0, 12)); 
      io.x1 = strSci2dbl(line.substr(2+12, 12)); 
      io.x2 = strSci2dbl(line.substr(2+24, 12)); 
      io.x3 = strSci2dbl(line.substr(2+36, 12)); 
                                     
      _rxnhdr.iono_corr( IO, io );

      if( _log ) _log->comment(2,"rinexn","ION BETA "  + iono_corr2str(IO)
                                                       + dbl2str(io.x0)
                                                       + dbl2str(io.x1)
                                                       + dbl2str(io.x2)
                                                       + dbl2str(io.x3)
                                                 + " " + base_name(_fname)
                              );

    // -------- "TIME SYSTEM CORR" --------
    }else if( line.find("TIME SYSTEM CORR",60) != string::npos ){

      TSYS_CORR   TS = str2tsys_corr(line.substr(0,4)); 
      t_tsys_corr ts;
     
      if( line.substr(5,2) != "  " ){ // ELIMINATE INCORRECT HEADERS FROM SOME RECEIVERS!

        ts.a0 = strSci2dbl(line.substr(5,  17)); 
        ts.a1 = strSci2dbl(line.substr(22, 16)); 
        ts.T  =    str2int(line.substr(38,  7)); 
        ts.W  =    str2int(line.substr(45,  5)); 
                                     
        _rxnhdr.tsys_corr( TS, ts );

        if( _log ) _log->comment(2,"rinexn","TIME SYSTEM CORR " + tsys_corr2str(TS)
                                                                + dbl2str(ts.a0*1e9,6)
                                                                + dbl2str(ts.a1*1e9,6)
                                                          + " " + int2str(ts.T)
                                                          + " " + int2str(ts.W)
                                                          + " " + base_name(_fname)
                                 );
        }

    // -------- "DELTA-UTC" --------
    }else if( line.find("DELTA-UTC: A0,A1,T,W",60) != string::npos ){

      TSYS_CORR   TS = TS_GPUT;
      t_tsys_corr ts;
     
      ts.a0 = strSci2dbl(line.substr(3,  19)); 
      ts.a1 = strSci2dbl(line.substr(22, 19)); 
      ts.T  =    str2int(line.substr(41,  9)); 
      ts.W  =    str2int(line.substr(50,  9)); 

      _rxnhdr.tsys_corr( TS, ts );

      if( _log ) _log->comment(2,"rinexn","DELTA-UTC: A0,A1,T,W" + tsys_corr2str(TS)
                                                                 + dbl2str(ts.a0*1e9,6)
                                                                 + dbl2str(ts.a1*1e9,6)
                                                           + " " + int2str(ts.T)
                                                           + " " + int2str(ts.W)
                                                           + " " + base_name(_fname)
                               );

    // -------- "CORR TO SYSTEM" --------
    }else if( line.find("CORR TO SYSTEM",60) != string::npos ){      // <= RINEX v2.10
      
      TSYS_CORR   TS = TS_GLUT;
      t_tsys_corr ts;

      ts.a0 = strSci2dbl(line.substr(21, 19));
      ts.a1 = 0.0;
      ts.T  = 0;
      ts.W  = 0;

      _rxnhdr.tsys_corr( TS, ts );

      if( _log ) _log->comment(2,"rinexn","TIME SYSTEM CORR " + tsys_corr2str(TS)
                                                              + dbl2str(ts.a0*1e9,6)
                                                              + dbl2str(ts.a1*1e9,6)
                                                        + " " + int2str(ts.T)
                                                        + " " + int2str(ts.W)
                                                        + " " + base_name(_fname)
                               );

      if( _log ) _log->comment(2,"rinexn","reading CORR TO SYSTEM");


    // -------- "D-UTC" --------
    }else if( line.find("D-UTC A0,A1,T,W,S,U",60) != string::npos ){ // == RINEX v2.11

      TSYS_CORR   TS = TS_SBUT;
      t_tsys_corr ts;

      ts.a0 = strSci2dbl(line.substr(0,  19)); 
      ts.a1 = strSci2dbl(line.substr(20, 19)); 
      ts.T  =    str2int(line.substr(40,  7)); 
      ts.W  =    str2int(line.substr(48,  5)); 

      _rxnhdr.tsys_corr( TS, ts );

      if( _log ) _log->comment(2,"rinexn","D-UTC: A0,A1,T,W,S,U" + tsys_corr2str(TS)
                                                                 + dbl2str(ts.a0*1e9,6)
                                                                 + dbl2str(ts.a1*1e9,6)
                                                           + " " + int2str(ts.T)
                                                           + " " + int2str(ts.W)
                                                           + " " + base_name(_fname)
                               );

    // -------- "LEAP SECONDS" --------
    }else if( line.find("LEAP SECONDS",60) != string::npos ){
      _rxnhdr.leapsec( str2int(line.substr(0,6)) );
      if( _log ) _log->comment(2,"rinexn","reading LEAP SECONDS");


    // -------- "COMMENT" --------
    }else if( line.find("COMMENT",60) != string::npos ){
      _comment.push_back(line.substr(0,60));
      if( _log ) _log->comment(2,"rinexn","reading COMMENT");


    // -------- "END OF HEADER" --------
    }else if( line.find("END OF HEADER",60) != string::npos ){
      if( _log ) _log->comment(2,"rinexn","reading END OF HEADER ");
      t_gcoder::_consume(tmpsize);
      _mutex.unlock(); return -1;
    }
    t_gcoder::_consume(tmpsize);
  }

  _mutex.unlock(); return consume;
}

   

int t_rinexn::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
{

#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _mutex.lock();
   
  if( t_gcoder::_add2buffer(buff, sz) == 0 ){ _mutex.unlock(); return 0; };

  
  t_gtime epoch;
  int b, e, s, l, i;
  int maxrec = MAX_RINEXN_REC;  // implicite
  string timstr;
  t_gnavdata data = { 0 };

  // RINEX v2.xx   
  if( _version[0] == '2' ){        b = 0; e = 22; l = 19; s = 3; // timstr = "%2s %2d %02d %02d %02d %02d %02d";
    // RINEX v3.xx
  }else if( _version[0] == '3' ){  b = 0; e = 23; l = 19; s = 4; // timstr = "%3s %4d %02d %02d %02d %02d %02d";
    // RINEX ???
  }else{                           b = 0; e = 23; l = 19; s = 4; // timstr = "%3s %4d %02d %02d %02d %02d %02d";
  }

  string line;
  int consume = 0;
  int tmpsize = 0;
  int recsize = 0;

  while( ( tmpsize = t_gcoder::_getline( line, 0 ) ) >= 0 ){
    


    consume += tmpsize;
    recsize += tmpsize;
    string epostr = line.substr(b,e);
    
    istringstream istr(line.substr(b,e) );
    istr.clear();

    string prn;
    int yr,mn,dd,hr,mi;
    int svn = 0;
    int irc = 0;
    float sec=0.0;
    char tmpbuff[82];   // RINEX 2 (80) + RINEX 3 (81)
    int min_sz = 23; // minimum size to be decoded (timestamp)
     
    switch( _version[0] ){
      case 2  : min_sz = 23; break;  // RINEX 2
      case 3  : min_sz = 24; break;  // RINEX 3
    }
       
    if( line.size() > 82 || _decode_buffer.size() <= min_sz ){ // avoid decoding such case

      t_gcoder::_consume(tmpsize);
      recsize = consume = 0; break; // read buffer 
    }

    strncpy(tmpbuff, line.c_str(), min_sz );
    tmpbuff[line.size()] = '\0';
    
    if( _version[0] == '2' ){          // don't use '%2i' in scan, but '%2d' instead !
      irc = sscanf(tmpbuff, "%2i%*[ ] %2d%*[ ] %2d%*[ ] %2d%*[ ] %2d%*[ ] %2d%*[ ] %5f", &svn,&yr,&mn,&dd,&hr,&mi,&sec);

      if( irc < 7 ){ // not success - remove this data
        t_gcoder::_consume(tmpsize);
        recsize = consume = 0; continue;
      }
      prn = t_gsys::eval_sat(svn, t_gsys::char2gsys(_gnsssys));

    }else{ 
      int isec = 0; char sat[3 + 1]; sat[3] = '\0';  // don't use '%2i' in scan, but '%2d' instead !
      irc = sscanf(tmpbuff, "%c%2d%*[ ] %4d%*[ ] %2d%*[ ] %2d%*[ ]%2d%*[ ]%2d%*[ ]%2d", &sat[0],&svn,&yr,&mn,&dd,&hr,&mi,&isec);
      
      if( irc < 8 ){ // not success - remove this data
        t_gcoder::_consume(tmpsize);
        recsize = consume = 0; continue;
      }
      prn = t_gsys::eval_sat(svn, t_gsys::char2gsys(sat[0]));

      sec = isec;

    }
     
    shared_ptr<t_gnav> geph = make_shared<t_gnav>();
       
    // !!! SAT musi byt preveden na PRN (pro RINEX ver < 3) --> neni implementovano!
    if (prn[0] == 'G') {
        maxrec = MAX_RINEXN_REC_GPS;  geph = make_shared<t_gnavgps>();  geph->glog(_log); epoch.tsys(t_gtime::GPS);
    }
    else if (prn[0] == 'R') {
        maxrec = MAX_RINEXN_REC_GLO;  geph = make_shared<t_gnavglo>();  geph->glog(_log); epoch.tsys(t_gtime::UTC);
    }
    else if (prn[0] == 'E') {
        maxrec = MAX_RINEXN_REC_GAL;  geph = make_shared<t_gnavgal>();  geph->glog(_log); epoch.tsys(t_gtime::GAL);
    }
    else if (prn[0] == 'C') {
        maxrec = MAX_RINEXN_REC_BDS;  geph = make_shared<t_gnavbds>();  geph->glog(_log); epoch.tsys(t_gtime::BDS);
    }
    else {
        string lg("Warning: not supported satellite satellite system: " + prn); mesg(GWARNING, lg);
        if (_log)  _log->comment(0, "rinexn", lg);
        else cerr << lg << endl;
    }

    epoch.from_ymdhms(yr, mn, dd, hr, mi, sec);

    if( fabs(epoch - _check_dt) > 7*86400 && _check_dt != FIRST_TIME ){
      string lg(prn+" strange epoch ["+epoch.str_ymdhms()+"] or corrupted file ["+base_name(_fname)+"]"); mesg(GWARNING,lg);
      t_gcoder::_consume(tmpsize);
      recsize = consume = 0; continue;
    }
     
    if( tmpsize < 57+s ) break;

    data[0] = strSci2dbl(line.substr( 19+s, l ));
    data[1] = strSci2dbl(line.substr( 38+s, l ));
    data[2] = strSci2dbl(line.substr( 57+s, l ));

    i = 2;
    while( i < MAX_RINEXN_REC ){

      // incomplete record
      if( ( tmpsize = t_gcoder::_getline( line, recsize ) ) < 0 ){ break; }

      consume += tmpsize;
      recsize += tmpsize;
      if( ++i < maxrec ){ if( tmpsize>   s ) data[i] = strSci2dbl(line.substr(    s, l )); } 
      if( ++i < maxrec ){ if( tmpsize>19+s ) data[i] = strSci2dbl(line.substr( 19+s, l )); } 
      if( ++i < maxrec ){ if( tmpsize>38+s ) data[i] = strSci2dbl(line.substr( 38+s, l )); } 
      if( ++i < maxrec ){ if( tmpsize>57+s ) data[i] = strSci2dbl(line.substr( 57+s, l )); } 

      
      // is record complete and filter-out GNSS systems
      if( geph && i+1 >= maxrec ){
        t_gcoder::_consume(recsize);
        recsize = 0;

        // filter GNSS and SAT
        if( !_filter_gnss(prn) ){
          if( _log ) _log->comment( 4, "rinexn", "skip "+prn);
          break;
        }

        if( epoch < _beg - MAX_NAV_TIMEDIFF || epoch > _end + MAX_NAV_TIMEDIFF){
          if( _log ) _log->comment( 4, "rinexn", "skip "+prn+" "+epoch.str_ymdhms());
          break;
        }

        geph->data2nav( prn, epoch, data );
	 
	// reset check_dt (already filtered sat)
        if( geph->healthy() ){ _check_dt = geph->epoch(); };

        // collect 1-line messages
        if( _log ) _log->comment(-3,geph->linefmt()+" "+base_name(_fname) );
 
        // fill data
        map<string,t_gdata*>::iterator it = _data.begin();
        while( it != _data.end() ){
          
          if( it->second->id_type()  == t_gdata::ALLNAV ||
              it->second->id_group() == t_gdata::GRP_EPHEM){

            ((t_gallnav*)it->second)->add( geph );
            
          }
          it++;
        }
        cnt++;
        break;
      }
    }
    if( recsize != 0 ) break; // break the initialization loop if read not finished correctly
  }
  _mutex.unlock(); return consume;
}

 


} // namespace
