
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

#include <cmath>
#include <iomanip>

#include "gutils/gpair.h"
#include "gutils/gconst.h"
#include "gutils/gtypeconv.h"

using namespace std;

namespace gnut {

t_gpair::t_gpair()
{
  _crd[0] = _crd[1] = 0.0;
}



t_gpair::t_gpair(double x, double y)
{
  _crd[0] = x;
  _crd[1] = y;
}



t_gpair::t_gpair(double crd[])
{
  _crd[0] = crd[0];
  _crd[1] = crd[1];
}


t_gpair::t_gpair(const ColumnVector& crd)
{
  _crd[0] = crd(1);
  _crd[1] = crd(2);
}



t_gpair::~t_gpair(){}


double& t_gpair::operator[](const size_t idx)
{

  if( idx > 1 ){
    cerr << "Not valid pair index [used 0]\n";
    return _crd[0];
  }
   
  return _crd[idx];
}



double t_gpair::operator[](const size_t idx) const
{

  if( idx < 2 ) return _crd[idx];
   
  return 0.0;
}



t_gpair t_gpair::operator+(const t_gpair& other) const
{
  t_gpair tmp(*this);
  tmp[0] += other[0];
  tmp[1] += other[1];

  return tmp;   
}



double t_gpair::crd(int idx) const
{

  if( idx >= 0 && idx < 2 ) return _crd[static_cast<unsigned int>(idx)];
   
  return 0.0;
}



void t_gpair::set(int idx, double newValue)
{

  if( idx >= 0 && idx < 2 ) _crd[static_cast<unsigned int>(idx)] = newValue;
     
}



t_gpair& t_gpair::operator=(const t_gpair& other)
{

  if( this != &other ){
    _crd[0] = other.crd(0);
    _crd[1] = other.crd(1);
  }
  return *this;
}


bool t_gpair::operator==(const t_gpair& tr) const
{

  return ( _crd[0] == tr.crd(0) &&
           _crd[1] == tr.crd(1)     );
}



bool t_gpair::operator<(const t_gpair& tr) const
{
  return ( ( _crd[0]  < tr.crd(0) ) ||
           ( _crd[0] == tr.crd(0) && _crd[1]  < tr.crd(1) ) );
}



double* t_gpair::crd_array()
{

  return _crd;
}



ColumnVector t_gpair::crd_cvect()
{
 ColumnVector tmp(2);
 tmp(1) = _crd[0];
 tmp(2) = _crd[1];
 return tmp;
}



t_gpair& t_gpair::crd_pair()
{
 return *this;
}


void t_gpair::set(const ColumnVector& crd)
{
  _crd[0] = crd(1);
  _crd[1] = crd(2);
}


void t_gpair::set(double crd[2])
{
  _crd[0] = crd[0];
  _crd[1] = crd[1];
}


ColumnVector t_gpair::unitary()
{
  ColumnVector tmp(2);
  tmp = this->crd_cvect();
  double s = tmp.norm_Frobenius();
  tmp /= s;
   
  return tmp;
   
}


ostream& operator<<(ostream& os, const t_gpair& x)
{
   os << fixed << setprecision(3) 
      << dbl2str(x[0]) + " " + dbl2str(x[1]);
   return os;
}



bool t_gpair::zero()
{
   if ( double_eq(_crd[0], 0.0) &&
        double_eq(_crd[1], 0.0) ) return true;
   else return false;
}

} // namespace
