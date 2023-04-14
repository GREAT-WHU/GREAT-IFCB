
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
#include <stdio.h>
#include <cstring>
#include "gdata/gdata.h" 
 
using namespace std;

namespace gnut {


t_gdata::t_gdata()
  : t_gmonit("gdata"),
    _type(NONE),
    _group(GRP_NONE)
{ 
  _log   = 0;
  _gnote = 0;
}


t_gdata::t_gdata( const t_gdata& data )
  : t_gmonit( data )
{ 
  _log   = data.glog();
  _gnote = data.gnote();
  _type  = data.id_type();
  _group = data.id_group();
}



t_gdata::~t_gdata(){

}


t_gdata& t_gdata::operator=( const t_gdata& data )
{ 
  _log   = data.glog();
  _gnote = data.gnote();
  _type  = data.id_type();
  _group = data.id_group();
  return *this;
}


int t_gdata::id_type( ID_TYPE t )
{
  unsigned int last = LAST;
  for( unsigned int i = 0; i < last ; ++i ){
    if( t == ID_TYPE(i) ){
      return _type = t;
    }
  }
   
  if( _log ){
    string tmp("Warning: Unknown data id. Reset to 0!\n"); // t !
    _log->comment(1,"gdata",tmp);
  }
  _type = t_gdata::NONE;
  _moni_id = str_type();
  return 0;
}


int t_gdata::id_group( ID_GROUP g )
{
  unsigned int last = GRP_LAST;
  for( unsigned int i = 0; i < last ; ++i ){
    if( g == ID_GROUP(i) ){
      return _group = g;
    }
  }
  if( _log ){
    string tmp("Warning: Unknown group type. Reset to 0!\n"); // g !
    _log->comment(1,"gdata",tmp);
  }
  _group = t_gdata::GRP_NONE;
  return 0;
}


string t_gdata::type2str(ID_TYPE type)
{
   string str = "";
   
  switch( type ){
   case  NONE    :  str = "NONE";    break;
   case  OBS     :  str = "OBS";     break;
   case  OBSGNSS :  str = "OBSGNSS"; break;

   case  EPH     :  str = "EPH";     break;
   case  EPHGPS  :  str = "EPHGPS";  break;
   case  EPHGLO  :  str = "EPHGLO";  break;
   case  EPHGAL  :  str = "EPHGAL";  break;
   case  EPHQZS  :  str = "EPHQZS";  break;
   case  EPHBDS  :  str = "EPHBDS";  break;
   case  EPHIRN  :  str = "EPHIRN";  break;     
   case  EPHSBS  :  str = "EPHSBS";  break;
   case  ALLNAV  :  str = "ALLNAV" ; break;
   case  ALLOBS  :  str = "ALLOBS" ; break; 
   case  BIAS    :  str = "BIAS";    break;
   case  LAST    :  str = "UNDEF";   break;
   default       :  str = "UNDEF";
  }
   return str;
}
   
   
string t_gdata::str_type() const
{
   string type = type2str(_type);   

   return type;
}


string t_gdata::str_group() const
{
  string group;
  switch( _group ){
   case GRP_NONE    : group = "GRP_NONE";    break;
   case GRP_OBSERV  : group = "GRP_OBSERV";  break;
   case GRP_EPHEM   : group = "GRP_EPHEM";   break;
   case GRP_MODEL   : group = "GRP_MODEL";   break; 
   case GRP_LAST    : group = "GRP_UNDEF";   break;
   default          : group = "GRP_UNDEF";
  }
return group;
}


} // namespace
