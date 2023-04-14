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

#ifndef GSETOUT_H
#define GSETOUT_H

#define XMLKEY_OUT "outputs"        ///< The defination of outputs module in xml file

#define DEFAULT_FILE_VER ""         ///< default version for file format
#define DEFAULT_FILE_UPD     0      ///< default auto update [min] for file saving
#define DEFAULT_FILE_LEN     0      ///< default auto length [min] for file saving
#define DEFAULT_FILE_SMP     0      ///< default auto sample [sec] for file saving
#define DEFAULT_FILE_OFF     0      ///< default file offset [min] for file saving
#define DEFAULT_FILE_SYS  "UTC"     ///< default file system       for file saving

#include <string>
#include <iostream>

#include "gio/glog.h"
#include "gutils/gtime.h"
#include "gutils/gtypeconv.h"
#include "gsetbase.h"

using namespace std;
using namespace pugi;

namespace gnut
{

	///< the order is important here!
	enum OFMT
	{
		XXX_OUT,
		LOG_OUT,
		IFCB_OUT
	};


	class GNUT_LIBRARY_EXPORT t_gsetout : public virtual t_gsetbase
	{
	public:
		/**@brief defalut constructor */
		t_gsetout();
		/**@brief defalut destructor */
		~t_gsetout();

		/**
		 * @brief change from string to OFMT
		 * @param[in] s file format
		 * @return OFMT : file format
		 */
		static OFMT   str2ofmt(const string& s);

		/**
		 * @brief change from OFMT to string
		 * @param[in] f file format
		 * @return string : file format
		 */
		static string ofmt2str(const OFMT&   f);

		/**@brief settings check */
		void check();         
		/**@brief settings help */
		void help();                               

		// attributes
		/**
		 * @brief  get verbosity attribute
		 * @return int : verbosity attribute
		 */
		int verb();   

		/**
		 * @brief  get append request
		 * @return bool : append request
		 */
		bool append();                      
               

		// elements
		/**
		 * @brief  get format output size
		 * @param[in] fmt file format
		 * @return int : format output size
		 */
		int output_size(const string& fmt);    

		/**
		 * @brief  get string outputs
		 * @param[in] fmt file format
		 * @return string : string outputs
		 */
		string outputs(const string& fmt);

		/**
		 * @brief  get formats
		 * @return set<string> : all the outputs
		 */
		set<string> oformats();                     

		/**
		 * @brief  get string output version
		 * @param[in] fmt file format
		 * @return string : string output version
		 */
		string version(const string& fmt);  



	protected:

		/**
		 * @brief  get string output file 
		 * @param[in] fmt file format
		 * @return string : string output file 
		 */
		string _outputs(const string& fmt);

		/**
		 * @brief  get all string output file
		 * @return set<string> : all string output file
		 */
		set<string> _oformats();

		set<OFMT> _OFMT_supported;                  ///< vector of supported OFMTs (app-specific)
		bool  _append;                              ///< append mode
		int   _verb;                                ///< output verbosity   

	private:

	};

} // namespace


#endif
