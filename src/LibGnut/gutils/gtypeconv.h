
#ifndef GTYPECONV_H
#define GTYPECONV_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: type conversion utilities
  Version: $ Rev: $

  2011-04-26 /PV: created
  2012-04-06 /JD: extracted type conversion utilities from previous utils.h

-*/

#include <cmath>
#include <string>
#include <iomanip>
#include <iostream>
#include "gutils/ExportGnut.h"

using namespace std;

namespace gnut
{
	/**@brief double equivalence according to machine capability */
	GNUT_LIBRARY_EXPORT bool double_eq(const double&,
		const double&);         
	/**@brief float equivalence according to machine capability */
	GNUT_LIBRARY_EXPORT bool float_eq(const float&,
		const float&);          
	/**@brief round double */
	GNUT_LIBRARY_EXPORT double dround(double d);                  
	/**@brief double to string conversion (width/digits by iomanip can be added !!!) */
	GNUT_LIBRARY_EXPORT string dbl2str(const double&, int p = 3); 
	/**@brief string to double conversion (avoiding blanks) */
	GNUT_LIBRARY_EXPORT double str2dbl(const string&);         
	/**@brief string (Scientific) to double conversion (including convert d,D to E!) */
	GNUT_LIBRARY_EXPORT double strSci2dbl(const string&);      
#ifdef STR2DBL
	double str2dbl(const char*);        ///< faster char* to double conversion (avoiding blanks)
#endif
	/**@brief integer to string conversion (widtht can be added !!!) */
	GNUT_LIBRARY_EXPORT string int2str(const int&);            
	/**@brief integer to string conversion (have width) */
	GNUT_LIBRARY_EXPORT string int2str(const int& num, const int& width);	
	/**@brief string to integer conversion (avoiding blanks) */
	GNUT_LIBRARY_EXPORT int    str2int(const string&);     

	/**@brief bool to string conversion  */
	GNUT_LIBRARY_EXPORT string  bl2str(const bool&);

	/**@brief string substitute */
	GNUT_LIBRARY_EXPORT size_t substitute(string& line, const string& a, const string& b, bool caseSensitive = true);
	/**@brief return trimmed string leading spaces */
	GNUT_LIBRARY_EXPORT string ltrim(const string&);            
	/**@brief return trimmed string trailing spaces */
	GNUT_LIBRARY_EXPORT string rtrim(const string&);            
	/**@brief returm trimmed string leading and trailing spaces */
	GNUT_LIBRARY_EXPORT string  trim(const string&);            
	/**@brief Frac part of double */
	GNUT_LIBRARY_EXPORT double frac(double x);

} // namespace

#endif