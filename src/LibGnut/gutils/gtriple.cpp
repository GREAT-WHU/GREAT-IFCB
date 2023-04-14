
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

#include "gutils/gtriple.h"
#include "gutils/gconst.h"
#include "gutils/gtypeconv.h"

using namespace std;

namespace gnut
{

	t_gtriple::t_gtriple()
	{
		_crd[0] = _crd[1] = _crd[2] = 0.0;
	}



	t_gtriple::t_gtriple(double x, double y, double z)
	{
		_crd[0] = x;
		_crd[1] = y;
		_crd[2] = z;
	}



	t_gtriple::t_gtriple(double crd[])
	{
		_crd[0] = crd[0];
		_crd[1] = crd[1];
		_crd[2] = crd[2];
	}


	t_gtriple::t_gtriple(const ColumnVector& crd)
	{
		_crd[0] = crd(1);
		_crd[1] = crd(2);
		_crd[2] = crd(3);
	}




	t_gtriple::~t_gtriple() {}



	double& t_gtriple::operator[](const size_t idx)
	{

		if (idx > 2) {
			cerr << "Not valid triple index [used 0]\n";
			return _crd[0];
		}
		return _crd[idx];
	}


	double t_gtriple::operator[](const size_t idx) const
	{

		if (idx < 3) return _crd[idx];

		return 0.0;
	}



	t_gtriple& t_gtriple::operator=(const t_gtriple& other)
	{
		if (this != &other) {
			_crd[0] = other.crd(0);
			_crd[1] = other.crd(1);
			_crd[2] = other.crd(2);
		}
		return *this;
	}


	t_gtriple& t_gtriple::operator+=(const t_gtriple& other)
	{
		if (this != &other) {
			_crd[0] += other.crd(0);
			_crd[1] += other.crd(1);
			_crd[2] += other.crd(2);
		}
		return *this;
	}


	t_gtriple& t_gtriple::operator-=(const t_gtriple& other)
	{
		if (this != &other) {
			_crd[0] -= other.crd(0);
			_crd[1] -= other.crd(1);
			_crd[2] -= other.crd(2);
		}
		return *this;
	}


	t_gtriple& t_gtriple::operator*=(const double& x)
	{
		_crd[0] *= x;
		_crd[1] *= x;
		_crd[2] *= x;
		return *this;
	}


	t_gtriple& t_gtriple::operator/=(const double& x)
	{
		_crd[0] /= x;
		_crd[1] /= x;
		_crd[2] /= x;
		return *this;
	}


	double t_gtriple::crd(int idx) const
	{

		if (idx >= 0 && idx < 3) return _crd[static_cast<unsigned int>(idx)];

		return 0.0;
	}



	t_gtriple t_gtriple::operator+(const t_gtriple& other) const
	{
		t_gtriple tmp(*this);
		tmp[0] += other[0];
		tmp[1] += other[1];
		tmp[2] += other[2];

		return tmp;
	}


	t_gtriple t_gtriple::operator-(const t_gtriple& other) const
	{
		t_gtriple tmp(*this);
		tmp[0] -= other[0];
		tmp[1] -= other[1];
		tmp[2] -= other[2];

		return tmp;
	}


	t_gtriple t_gtriple::operator*(const double& x) const
	{
		t_gtriple tmp(*this);
		tmp[0] *= x;
		tmp[1] *= x;
		tmp[2] *= x;

		return tmp;
	}


	t_gtriple t_gtriple::operator/(const double& x) const
	{
		t_gtriple tmp(*this);
		tmp[0] /= x;
		tmp[1] /= x;
		tmp[2] /= x;

		return tmp;
	}



	void t_gtriple::set(int idx, double newValue)
	{
		if (idx >= 0 && idx < 3) _crd[static_cast<unsigned int>(idx)] = newValue;

	}



	bool t_gtriple::operator==(const t_gtriple& tr) const
	{

		return (_crd[0] == tr.crd(0) &&
			_crd[1] == tr.crd(1) &&
			_crd[2] == tr.crd(2));
	}


	bool t_gtriple::operator!=(const t_gtriple& tr) const
	{

		return (!double_eq(_crd[0], tr.crd(0)) ||
			!double_eq(_crd[1], tr.crd(1)) ||
			!double_eq(_crd[2], tr.crd(2)));
	}


	bool t_gtriple::operator<(const t_gtriple& tr) const
	{
		return ((_crd[0] < tr.crd(0)) ||
			(_crd[0] == tr.crd(0) && _crd[1] < tr.crd(1)) ||
			(_crd[0] == tr.crd(0) && _crd[1] == tr.crd(1) && _crd[2] < tr.crd(2)));
	}



	double* t_gtriple::crd_array()
	{
 
		return _crd;
	}


	
	ColumnVector t_gtriple::crd_cvect()
	{
		ColumnVector tmp(3);
		tmp(1) = _crd[0];
		tmp(2) = _crd[1];
		tmp(3) = _crd[2];
		return tmp;
	}



	t_gtriple& t_gtriple::crd_tripl()
	{
		return *this;
	}


	void t_gtriple::set(const ColumnVector& crd)
	{
		_crd[0] = crd(1);
		_crd[1] = crd(2);
		_crd[2] = crd(3);
	}


	void t_gtriple::set(double crd[3])
	{
		_crd[0] = crd[0];
		_crd[1] = crd[1];
		_crd[2] = crd[2];
	}


	ColumnVector t_gtriple::unitary()
	{
		ColumnVector tmp(3);
		tmp = this->crd_cvect();
		double s = tmp.norm_Frobenius();
		tmp /= s;

		return tmp;

	}


	t_gpair t_gtriple::gpair() const
	{
		return t_gpair(_crd[0], _crd[1]);
	}



	ostream& operator<<(ostream& os, const t_gtriple& x)
	{
		os << fixed << setprecision(5)
			<< dbl2str(x[0]) + " " + dbl2str(x[1]) + " " + dbl2str(x[2]);
		return os;
	}


	double t_gtriple::norm() const
	{
		return sqrt(_crd[0] * _crd[0] + _crd[1] * _crd[1] + _crd[2] * _crd[2]);
	}


	double t_gtriple::sum() const
	{
		return _crd[0] + _crd[1] + _crd[2];
	}


	bool t_gtriple::zero()
	{
		if (double_eq(_crd[0], 0.0) &&
			double_eq(_crd[1], 0.0) &&
			double_eq(_crd[2], 0.0)) return true;
		else return false;
	}

} // namespace
