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

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "gutils/gsys.h"
#include "gutils/gcommon.h"

using namespace std;

namespace gnut {

// ---------------------------------------------------------------------------------
// class GSYS
// ---------------------------------------------------------------------------------

t_gsys::t_gsys(GSYS sys) : _gsys(sys)
{}


t_gsys::t_gsys(string sys)
 : _gsys(GNS)
{
 if( sys.size() > 0 ) _gsys = str2gsys(sys);
}


t_gsys::t_gsys(char c)
{
  _gsys = char2gsys(c);
}


// --------------------------------------------------------
// STATIC FUNCTIONS
// --------------------------------------------------------

GOBS t_gsys::gobs_priority(GSYS gs, GOBS gobs)
{
	if (gs == GPS)
	{
		switch (gobs)
		{
		case C1C:
		case C1P:
		case C1W:
		{
			return C1W;
		}
		case C2C:
		case C2L:
		case C2X:
		case C2P:
		case C2W:
		{
			return C2W;
		}
		case C5Q:
		case C5X:
		{
			return C5X;
		}
		default:  return GOBS::X;
		}
	}
	else if (gs == GLO)
	{
		switch (gobs)
		{
		case C1C:
		case C1P: return C1P;
		case C2C:
		case C2P: return C2P;
		}
	}
	else
	{
		return gobs;
	}
}

GOBS t_gsys::gobs_priority(GSYS gs, GOBSBAND band)
{
	if (gs == GPS)
	{
		switch (band)
		{
		case BAND_1:
		{
			return C1W;
		}
		case BAND_2:
		{
			return C2W;
		}
		case BAND_5:
		{
			return C5X;
		}
		default:  return GOBS::X;
		}
	}
	else if (gs == GLO)
	{
		switch (band)
		{
		case BAND_1: return C1P;
		case BAND_2: return C2P;
		default:  return GOBS::X;
		}
	}
	else if (gs = GAL)
	{
		switch (band)
		{
		case BAND_1: return C1X;
		case BAND_5: return C5X;
		case BAND_7: return C7X;
		default:  return GOBS::X;
		}
	}
	else
	{
		return GOBS::X;
	}
}

GOBS t_gsys::gobs_defaults(GSYS gs, GOBS  gobs1, GOBSBAND band)
{
	if (gs == GPS)
	{
		switch (band)
		{
		case BAND_1:
		{
			return C1W;
		}
		case BAND_2:
		{
			return C2W;
		}
		case BAND_5:
		{
			return C5X;
		}
		default:  return GOBS::X;
		}
	}
	else if (gs == GLO)
	{
		switch (band)
		{
		case BAND_1:  return C1P;
		case BAND_2:  return C2P;
		default:  return GOBS::X;
		}
	}
	else if (gs == GAL)
	{
		string strGOBS = gobs2str(gobs1);
		switch (band)
		{
		case BAND_1:
		{
			if (strGOBS[strGOBS.size() - 1] == 'X')
			{
				return C1X;
			}
			else
			{
				return C1C;
			}
		}
		case BAND_5:
		{
			if (strGOBS[strGOBS.size() - 1] == 'X')
			{
				return C5X;
			}
			else
			{
				return C5Q;
			}
		}
		case BAND_7:
		{
			if (strGOBS[strGOBS.size() - 1] == 'X')
			{
				return C7X;
			}
			else
			{
				return C7Q;
			}
		}
		default:  return GOBS::X;
		}
	}
	else
	{
		return GOBS::X;
	}
}
GOBSBAND t_gsys::gfrq2band(GSYS gs, GFRQ gfreq )
{
  t_map_freq m = GNSS_FREQ_PRIORITY;
  if( gs == GNS || gfreq == LAST_GFRQ ) return BAND;

  for( size_t i=0; i < m[gs].size(); ++i ){
    if( m[gs].at(i) == gfreq ) return band_priority(gs, FREQ_SEQ(i));
  }
  return BAND;
}


GFRQ t_gsys::band2gfrq(GSYS gs, GOBSBAND gband )
{
  t_map_band m = GNSS_BAND_PRIORITY;
  if( gs == GNS || gband == BAND ) return LAST_GFRQ;

  for( size_t i=0; i < m[gs].size(); ++i ){
    if( m[gs].at(i) == gband ) return freq_priority(gs, FREQ_SEQ(i));
  }
  return LAST_GFRQ;
}


FREQ_SEQ t_gsys::gfrq2freq(GSYS gs, GFRQ gfreq )
{
  t_map_freq m = GNSS_FREQ_PRIORITY;
  if( gs == GNS || gfreq == LAST_GFRQ ) return FREQ_X;

  for( size_t i=0; i < m[gs].size(); ++i ){
    if( m[gs].at(i) == gfreq ) return FREQ_SEQ(i);
  }
  return FREQ_X;
}


GSYS t_gsys::gfrq2gsys(GFRQ freq)
{
   string s = t_gfreq::gfreq2str(freq);
   return t_gsys::char2gsys(s[0]);
}


FREQ_SEQ t_gsys::band2freq(GSYS gs, GOBSBAND gband )
{
  static t_map_band m = GNSS_BAND_PRIORITY;
  if( gs == GNS || gband == BAND ) return FREQ_X;

  for( size_t i=0; i < m[gs].size(); ++i ){
    if( m[gs].at(i) == gband ) return FREQ_SEQ(i);
  }
  return FREQ_X;
}

   

GOBSBAND t_gsys::band_priority(GSYS gs, FREQ_SEQ iseq)  // FREQ_SEQ priority band
{
  static t_map_band m = GNSS_BAND_PRIORITY;

  if( gs == GNS || iseq >= m[gs].size() ) return BAND;

  return m[gs][iseq];
}


GFRQ t_gsys::freq_priority(GSYS gs, FREQ_SEQ iseq)  // FREQ_SEQ priority frequency
{
  static t_map_freq m = GNSS_FREQ_PRIORITY;

  if( gs == GNS || iseq >= m[gs].size() ) return LAST_GFRQ;

  return m[gs][iseq];
}


GOBSATTR t_gsys::attr_priority(GSYS gs, GOBSBAND gb, GOBSTYPE gt, unsigned int iseq)  // iseq priority sequence
{
  t_map_gnss m = GNSS_DATA_PRIORITY();

  if( gs == GNS  ||
      gt == TYPE ||
      gb == BAND || iseq > m[gs][gb][gt].size() ) return ATTR;

  return m[gs][gb][gt][iseq];
}



string t_gsys::gsys2str( GSYS sys )
{
  gtrace("t_gsys::gsys2str");



  switch( sys ){
   case GPS  : return "GPS";
   case GLO  : return "GLO";
   case GAL  : return "GAL";
   case BDS  : return "BDS";
   case SBS  : return "SBS";
   case QZS  : return "QZS";
   case IRN  : return "IRN";
   case GNS  : return "GNS";
     
   default : { cout << "*** warning: unknown GNSS system!\n"; cout.flush(); }
   

  }
   
  return "GNS";
}



char t_gsys::gsys2char( GSYS sys )
{
  gtrace("t_gsys::gsys2char");

  switch( sys ){
   case GPS  : return 'G';
   case GLO  : return 'R';
   case GAL  : return 'E';
   case BDS  : return 'C';
   case SBS  : return 'S';
   case QZS  : return 'J';
   case IRN  : return 'I';
   case GNS  : return 'X';
     
   default : { cout << "*** warning: unknown GNSS system \n"; cout.flush(); }

  }
   
  return 'X';
}



GSYS t_gsys::str2gsys( string s )
{
  gtrace("t_gsys::str2gsys [" + s + "]" );
   

  if( s.size() == 0 ){
    cout << "*** warning: not defined GNSS system code [NULL]\n"; cout.flush();
    return GNS;
  }

  transform(s.begin(), s.end(), s.begin(), ::toupper);
  
  if(      s == "G"    || s == "GPS"  || s == "NAVSTAR"   ) return GPS;
  else if( s == "R"    || s == "GLO"  || s == "GLONASS"   ) return GLO;
  else if( s == "E"    || s == "GAL"  || s == "GALILEO"   ) return GAL;
  else if( s == "C"    || s == "COMP" || s == "COMPASS"   ) return BDS;
  else if( s == "C"    || s == "BDS"  || s == "BEIDOU"    ) return BDS;
  else if( s == "S"    || s == "SBS"  || s == "EGNOS"     ) return SBS;
  else if( s == "S"    || s == "SBAS"                     ) return SBS;
  else if( s == "J"    || s == "QZS"  || s == "JAXA"      ) return QZS;
  else if( s == "J"    || s == "QZSS"                     ) return QZS;
  else if( s == "I"    || s == "IRN"  || s == "IRNSS"     ) return IRN;
  else if( s == "X"    || s == "GNS"  || s == "GNSS"      ) return GNS;
  else if( s == "M"                                       ) return GNS;
  else{ cout << "*** warning: not defined GNSS system code [" << s[0] << "]\n"; cout.flush(); }

  return GNS;
}

GSYS t_gsys::sat2gsys(const string& s)
{
    string tmp = s.substr(0, 1);
    if (s.empty()) return GNS;
    else return str2gsys(tmp);
}



char t_gsys::str2char(string s)
{
  if( s.size() > 0 ) return gsys2char( str2gsys(s) );
  return 'X';
}	



GSYS t_gsys::char2gsys(char c)
{
  gtrace("t_gsys::char2gsys");

  if(      c == 'G' ) return GPS;
  else if( c == 'R' ) return GLO;
  else if( c == 'E' ) return GAL;
  else if( c == 'C' ) return BDS;
  else if( c == 'S' ) return SBS;
  else if( c == 'J' ) return QZS;
  else if( c == 'I' ) return IRN;
  else if( c == 'M' ) return GNS;
  else if( c == 'X' ) return GNS;
  else{ cout << "*** warning: not defined GNSS system char [" << c << "]\n"; cout.flush(); }

  return GNS;
}



string t_gsys::char2str(char c)
{
  return gsys2str( char2gsys(c) );
}	



string t_gsys::eval_sat( string sat, GSYS sys )
{
  istringstream is( sat ); // .substr(0,3) NE!
  GSYS gnss = sys;
  int  svn  =  0;
  char chr  = 'G';        // if empty, use GPS
  size_t l  = is.str().length();
   
  if( l == 0 ) return "X00";

  if (l < 3 || sat[0] == ' ')  sscanf(sat.c_str(),"%d",&svn);
  else                         sscanf(sat.c_str(),"%c%d",&chr,&svn);


  if( is.fail() ){ return "X00"; }
  if( chr != 'G' ) gnss = char2gsys( chr );

  switch( gnss ){
    case GPS       : chr = 'G'; break;
    case GLO       : chr = 'R'; break;
    case GAL       : chr = 'E'; break;
    case BDS       : chr = 'C'; break;
    case SBS       : chr = 'S'; break;
    case QZS       : chr = 'J'; break;
    case IRN       : chr = 'I'; break;
    case GNS       : chr = 'X'; break;
    default        : chr = 'X'; break;
  }

  if( svn > QZS_OFFSET ){ svn -= QZS_OFFSET; }
  if( svn > SBS_OFFSET ){ svn -= SBS_OFFSET; }


  char tmp[4];
  sprintf(tmp, "%c%02d", chr, svn);


  string tmp_svn(tmp);
  return tmp_svn;
}



string t_gsys::eval_sat( int svn, GSYS sys )
{
  char chr;
  switch( sys ){
    case GPS       : chr = 'G'; break;
    case GLO       : chr = 'R'; break;
    case GAL       : chr = 'E'; break;
    case BDS       : chr = 'C'; break;
    case SBS       : chr = 'S'; break;
    case QZS       : chr = 'J'; break;
    case IRN       : chr = 'I'; break;
    case GNS       : chr = 'X'; break;
    default        : chr = 'X'; break;
  }
  if( svn > QZS_OFFSET ){ svn -= QZS_OFFSET; }
  if( svn > SBS_OFFSET ){ svn -= SBS_OFFSET; }
    
  ostringstream os;
  os << setw(1) << chr << setfill('0') << setw(2) << svn;

  return os.str();
}


void t_gsys::from_string( string sys )
{
#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gsys = str2gsys( sys );
}


void t_gsys::from_gsys( GSYS sys )
{
#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif
  _gsys = sys;
}



bool t_gsys::operator==(const string& sys) const
{
  if( _gsys == str2gsys(sys)  ) return true;
  return false;
}


bool t_gsys::operator==(const GSYS& sys) const
{

  return false;
}


// ---------------------------------------------------------------------------------
// class FREQ
// ---------------------------------------------------------------------------------


GFRQ t_gfreq::str2gfreq( string freq )
{
  gtrace("t_gsys::str2gfreq");
   

  transform(freq.begin(), freq.end(), freq.begin(), ::toupper);
   
  if(      freq == "G01" ) return G01;
  else if( freq == "G02" ) return G02;
  else if( freq == "G05" ) return G05;
  else if( freq == "R01" ) return R01;
  else if( freq == "R02" ) return R02;
  else if( freq == "R03" ) return R03_CDMA;
  else if( freq == "R05" ) return R05_CDMA;
  else if( freq == "E01" ) return E01;
  else if( freq == "E05" ) return E05;
  else if( freq == "E07" ) return E07;
  else if( freq == "E08" ) return E08;
  else if( freq == "E06" ) return E06;
  else if( freq == "C02" ) return C02;
  else if( freq == "C07" ) return C07;  
  else if( freq == "C06" ) return C06;  
  else if( freq == "J01" ) return J01;
  else if( freq == "J02" ) return J02;
  else if( freq == "J05" ) return J05;
  else if( freq == "J06" ) return J06;
  else if( freq == "S01" ) return S01;
  else if( freq == "S05" ) return S05;
  else if( freq == "I05" ) return I05;

  else if( freq == "LAST_GFRQ" ) return LAST_GFRQ;
  

  return LAST_GFRQ;
}



string t_gfreq::gfreq2str( GFRQ freq )
{
  gtrace("t_gsys::gfreq2str");


  switch( freq ){
   case G01 : return "G01";
   case G02 : return "G02";
   case G05 : return "G05";
   case R01 : return "R01";
   case R02 : return "R02";
   case R01_CDMA : return "R01";
   case R02_CDMA : return "R02";
   case R03_CDMA : return "R03";
   case R05_CDMA : return "R05";
   case E01 : return "E01";
   case E05 : return "E05";
   case E07 : return "E07";
   case E08 : return "E08";
   case E06 : return "E06";
   case C02 : return "C02";
   case C06 : return "C06"; 
   case C07 : return "C07"; 
   case J01 : return "J01";
   case J02 : return "J02";
   case J05 : return "J05";
   case J06 : return "J06";
   case S01 : return "S01";
   case S05 : return "S05";
   case I05 : return "I05";
       
   case LAST_GFRQ : return "LAST_GFRQ";
     
   default : {  cout << "*** warning: unknown frequency code \n"; cout.flush(); }
   

  }
   
  return "LAST_GFRQ";
}
   

bool t_gsys::bds_geo(const string& sat)
{
   set<string> geo;                                  // prns of geost. satellites
   geo.insert("C01");
   geo.insert("C02");
   geo.insert("C03");
   geo.insert("C04");
   geo.insert("C05");      
   if( geo.find(sat) != geo.end() ) return true;
     
   return false;
}
   
} // namespace
