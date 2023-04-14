
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

#include "grnxhdr.h"


namespace gnut {

t_rnxhdr::t_rnxhdr()
{
}

t_rnxhdr::~t_rnxhdr()
{
}


void t_rnxhdr::clear()
{
  _gtime = t_gtime::current_time(t_gtime::GPS);
  _first = FIRST_TIME;
  _last  = LAST_TIME;

  _path     = "";
  _rnxsys   = 'G';
  _rnxver   = 'x';
  _program  = "";
  _runby    = "";
  _markname = "";
  _marknumb = "";
  _marktype = "";
  _observer = "";
  _agency   = "";
  _recnumb  = "";
  _rectype  = "";
  _recvers  = "";
  _antnumb  = "";
  _anttype  = "";
  _strength = "DBHZ";
  _interval = 0;
  _leapsec  = 0;
  _numsats  = 0;
   
  _aprxyz = t_gtriple(0,0,0);
  _antxyz = t_gtriple(0,0,0);
  _antneu = t_gtriple(0,0,0);   

  _mapobs.clear();
   
  _comment.clear();
  
}


ostream& operator<<(ostream& os, const t_rnxhdr& x)
{
  os << "program: "  << x.program()   << endl;
  os << "runby: "    << x.runby()     << endl;
  os << "gtime: "    << x.gtime().str_hms()     << endl;    
  os << "markname: " << x.markname()  << endl;
  os << "marknumb: " << x.marknumb()  << endl;
  os << "marktype: " << x.marktype()  << endl; 
  os << "observer: " << x.observer()  << endl; 
  os << "agency: "   << x.agency()    << endl;
  os << "recnumb: "  << x.recnumb()   << endl;
  os << "rectype: "  << x.rectype()   << endl;
  os << "recvers: "  << x.recvers()   << endl;
  os << "antnumb: "  << x.antnumb()   << endl;
  os << "anttype: "  << x.anttype()   << endl;
  os << "aprxyz: "   << x.aprxyz()    << endl;
  os << "antxyz: "   << x.antxyz()    << endl;
  os << "antneu: "   << x.antneu()    << endl;
  os << "strength: " << x.strength()  << endl;
  os << "interval: " << x.interval()  << endl;
  os << "first: "    << x.first().str_hms()     << endl;
  os << "last: "     << x.last().str_hms()      << endl;
  os << "leapsec: "  << x.leapsec()   << endl;
  os << "numsats: "  << x.numsats()   << endl;
   return os;			
}

} // namespace
