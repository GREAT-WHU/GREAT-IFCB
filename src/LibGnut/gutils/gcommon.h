
#ifndef GCOMMON_H
#define GCOMMON_H 
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: 
  Version: $ Rev: $

  2011-02-14 /JD: created

-*/

#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <stdio.h> 
#include <sys/stat.h>
#include "gutils/ExportGnut.h"

#ifdef _WIN32
#include <direct.h>
#include <Shlwapi.h>
#define GET_CURRENT_PATH _getcwd
#pragma warning(disable:4503)         ///< suppress Visual Studio WARNINGS 4503 about truncated decorated names

#else
#include <unistd.h>
#define GET_CURRENT_PATH  getcwd
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif

using namespace std;

namespace gnut
{

	static const char lf = 0x0a; ///< = "\n" .. line feed

#if defined __linux__ || defined __APPLE__
	static const string crlf = string("") + lf;   ///< "\n"
#endif

#if defined _WIN32 || defined _WIN64
	static const string crlf = string("") + lf; ///< "\r\n"
#endif 


	inline void gtrace(const string& str)
	{
#ifdef TRACE
		cout << "#" << str << endl;
#endif
	}

#define SQR(x)   ((x)*(x))
#define SQRT(x)  ((x)<=0.0?0.0:sqrt(x))

} // namespace

#endif // # GCOMMON_H
