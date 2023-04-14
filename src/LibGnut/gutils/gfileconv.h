
#ifndef GFILECONV_H
#define GFILECONV_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: file conversion utilities
  Version: $ Rev: $

  2016-06-26 /JD: created

-*/

#include <string>
#include "gutils/ExportGnut.h"

#if defined _WIN32 || defined _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#define GFILE_PREFIX "file://"

using namespace std;

namespace gnut
{
	/**@brief extract file base name */
	string base_name(const string& path);              
	/**@brief extract file dir  name */
	string  dir_name(const string& path);              
	/**@brief get filename */
	string file_name(const string& path);
	/**@brief check existance of path */
	bool  dir_exists(const string& path);                
	/**@brief create path recursively */
	int    make_path(const string& path);                
	/**@brief create single directory */
	int    make_dir(const string& path);                 
} // namespace

#endif