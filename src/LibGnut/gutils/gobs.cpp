
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

#include <iostream>
#include <iomanip>

#include "gutils/gobs.h"

using namespace std;

namespace gnut {

// -------------------------------------------------------------------------------------------
// class T_GATTR
// -------------------------------------------------------------------------------------------
bool t_gattr::valid() const
{
  return ( _gattr != ATTR );
}



void t_gattr::attr( const GOBSATTR& a )
{
  _gattr = a;
}



GOBSATTR t_gattr::attr()const
{ 
  return _gattr;
}


bool t_gattr::operator==(const t_gattr& g) const
{
  return ( _gattr == g.attr() );
}


// -------------------------------------------------------------------------------------------
// class T_GBAND
// -------------------------------------------------------------------------------------------
bool t_gband::valid() const
{
  return ( t_gattr::valid() && _gband != BAND );
}


void t_gband::band( const GOBSBAND& b )
{
  _gband = b;
}


GOBSBAND t_gband::band()const
{ 
  return _gband;
}


void t_gband::gattr( const t_gattr& g )
{
  _gattr = g.attr();
  _gband = BAND;
}


t_gattr t_gband::gattr()const
{
  t_gattr g(_gattr);
  return g;
}


bool t_gband::operator==(const t_gband& g) const
{
  return ( _gband == g.band() &&
	   _gattr == g.attr()
         );
}

// -------------------------------------------------------------------------------------------
// class T_GOBS
// -------------------------------------------------------------------------------------------


bool t_gobs::valid() const
{
  return ( t_gband::valid() && _gtype != TYPE );
}


void t_gobs::type( const GOBSTYPE& t )
{
  _gtype = t;
}


GOBSTYPE t_gobs::type()const
{
  return _gtype;
}


void t_gobs::gband( const t_gband& g )
{
  _gattr = g.attr();
  _gband = g.band();
  _gtype = TYPE;
}


t_gband t_gobs::gband()const
{
  t_gband g(_gband,_gattr);
  return g;
}


bool t_gobs::operator==(const t_gobs& g) const
{
  return ( _gtype == g.type() &&
	   _gband == g.band() &&
           _gattr == g.attr()
	 );
}


int t_gobs::gobs(const GOBS& g)
{
  string s = gobs2str( g );
  _gtype = str2gobstype( s );
  _gband = str2gobsband( s );
  _gattr = str2gobsattr( s );

  return 1;
}


int t_gobs::gobs(const string& s)
{
  _gtype = str2gobstype( s );
  _gband = str2gobsband( s );
  _gattr = str2gobsattr( s );

  return 1;
}


GOBS t_gobs::gobs()const
{
  string s = gobstype2str(_gtype) + 
             gobsband2str(_gband) + 
             gobsattr2str(_gattr);
  return str2gobs( s );
}


void t_gobs::gobs2to3(GSYS sys)
{
	if (_gattr != ATTR_NULL) {
		return;
	}
	GOBS obs2 = this->gobs();
	switch (sys)
	{
	case gnut::GPS:
		if (obs2 == L1 || obs2 == L2 || obs2 == L5 || obs2 == C1 || obs2 == C2 || obs2 == C5)
		{
			_gattr = ATTR_C;
		}
		else if (obs2 == D1 || obs2 == D2 || obs2 == D5 || obs2 == S1 || obs2 == S2 || obs2 == S5)
		{
			_gattr = ATTR_C;
		}
		else if (obs2 == P1 || obs2 == P2 || obs2 == P5) {
			_gtype = TYPE_C;
			_gattr = ATTR_P;
		}
	case gnut::GAL:
		break;
	case gnut::GLO:
		break;
	case gnut::BDS:
		break;
	case gnut::QZS:
		break;
	case gnut::SBS:
		break;
	case gnut::IRN:
		break;
	case gnut::GNS:
		break;
	default:
		break;
	}
	return;
}
  

bool t_gobs::is_code()const
{
  return( _gtype == TYPE_C || _gtype == TYPE_P );
}


bool t_gobs::is_phase()const
{
  return( _gtype == TYPE_L );
}

} // namespace
