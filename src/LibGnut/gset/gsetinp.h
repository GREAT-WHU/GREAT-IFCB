/**
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

#ifndef GSETINP_H
#define GSETINP_H

#define XMLKEY_INP "inputs"  ///< The defination of input node in XML

#include <map>
#include <string>
#include <iostream>

#include "gio/glog.h"
#include "gutils/gtypeconv.h"
#include "gset/gsetbase.h"

using namespace std;
using namespace pugi;

namespace gnut
{

	/**@brief input file order is important here!*/
	enum IFMT
	{
		RINEXN_INP,
		RINEXO_INP,   	
		BIABERN_INP,
     
		AMBFLAG_INP, 
		AMBFLAG13_INP,          

		IFCB_INP,
		SATPARS_INP,
		UNDEF
	};


	class GNUT_LIBRARY_EXPORT t_gsetinp : public virtual t_gsetbase
	{
	public:
		/**@brief defalut constructor */
		t_gsetinp();
		/**@brief defalut destructor */
		~t_gsetinp();

		/**@brief settings check */
		void check();      
		/**@brief settings help */
		void help();                                  

		/**
		 * @brief change from string to IFMT
		 * @param[in] s file format
		 * @return IFMT : file format
		 */
		static IFMT   str2ifmt(const string& s);

		/**
		 * @brief change from IFMT to string
		 * @param[in] f file format
		 * @return string : file format
		 */
		static string ifmt2str(const IFMT&   f);

		/**
		 * @brief get format input size
		 * @param[in] fmt file format
		 * @return int : format input size
		 */
		int input_size(const string& fmt);

		/**
		 * @brief get format inputs (all in multimap)
		 * @return multimap<IFMT, string> : format inputs (all in multimap)
		 */
		multimap<IFMT, string> inputs_all();        

		/**
		 * @brief get format inputs (ordered)
		 * @param[in] fmt file format
		 * @return vector<string> : format inputs (ordered)
		 */
		vector<string> inputs(const string& fmt); 

		/**
		 * @brief get formats       (ordered)
		 * @return set<string> :    formats (ordered)
		 */
		set<string> iformats();   


		/**@brief checking navigation Rinex */
		bool chkNavig();            
		/**@brief checking satellite healthy status */
		bool chkHealth();                             

	protected:

		/**
		 * @brief get all the file name of ftm
		 * @param[in] ftm file format
		 * @return vector<string> : all the file name of ftm
		 */
		vector<string> _inputs(const string& fmt);

		/**
		 * @brief get all the ftm in input node
		 * @return set<string> : all the ftm in input node
		 */
		set<string> _iformats();

	protected:
		set<IFMT> _IFMT_supported;                    ///< vector of supported IFMTs (app-specific)

		bool      _chkNavig;                          ///< check navigation data or not
		bool      _chkHealth;                         ///< check data health or not

		string    _corrStream;                        ///< error message string

	private:

	};

} // namespace

#endif
