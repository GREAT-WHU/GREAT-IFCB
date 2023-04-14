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

#ifndef GSETGEN_H
#define GSETGEN_H

#include <set>
#include <string>
#include <iostream>
#include <vector>

#include "gio/glog.h"
#include "gutils/gsys.h"
#include "gutils/gtime.h"
#include "gutils/gtypeconv.h"
#include "gset/gsetbase.h"

#define XMLKEY_GEN   "gen" ///< The defination of gen node
#define DEF_RECEIVER "   " ///< Default receiver : all !
#define DEF_SAMPLING 30    ///< Default sampling : 30s !

using namespace std;

namespace gnut
{

	class GNUT_LIBRARY_EXPORT t_gsetgen : public virtual t_gsetbase
	{
	public:

		/**
		 * @brief default constructor, distinguish GNSS/nonGNSS app
		 * @param[in] gnss : gnss process or not, default is true
		 */
		t_gsetgen(bool gnss = true);                 

		/**@brief defalut desconstructor */
		~t_gsetgen();

		/**@brief settings check */
		void check();  

		/**@brief settings help */
		void help();                                   

		/**
		 * @brief get the beg time of process
		 * @return t_gtime : the beg time of process
		 */
		t_gtime beg(bool conv = true);

		/**
		 * @brief get the end time of process
		 * @return t_gtime : the end time of process
		 */
		t_gtime end(bool conv = true);

		/**
		 * @brief get the sampling time of process
		 * @return double : the sampling time of process
		 */
		double sampling();

		/**
		 * @brief get the default sampling time of process
		 * @return double : the default sampling time of process
		 *  @retval DEF_SAMPLING default sampling 
		 */
		double sampling_default() { return DEF_SAMPLING; }       

		/**
		 * @brief get the List of system names
		 * @return set<string> : List of system names
		 */
		virtual set<string> sys();
		/**
		 * @brief get the List of sites
		 * @return set<string> : List of sites
		 */
		set<string> recs();


		/**
		* @brief add for remove unused satellites
		* @return set<string> : satellites which will be removed
		*/
		virtual set<string> sat_rm();

	protected:

		bool   _gnss; ///< gnss or not
		string _sys;  ///< sys name
		int    _dec;  ///< sampling 

	private:
	};

} // namespace

#endif
