
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

#ifndef GSETGNSS_H
#define GSETGNSS_H

#define XMLKEY_GNSS "gnss" ///< The defination of gnss node 

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "gio/glog.h"
#include "gutils/gsys.h"
#include "gutils/gobs.h"
#include "gutils/gtypeconv.h"
#include "gset/gsetbase.h"

using namespace std;
using namespace pugi;

namespace gnut
{
	/// The class for gnss module in xml file
	class GNUT_LIBRARY_EXPORT t_gsetgnss : public virtual t_gsetbase
	{
	public:
		/**@brief defalut constructor */
		t_gsetgnss();
		/**@brief defalut destructor */
		~t_gsetgnss();

		/**@brief settings check */
		void check();      
		/**@brief settings help */
		void help();                                  

		/**
		 * @brief get sats of all systems 
		 * @return set<string> : sats of all systems
		 */
		set<string>  sat();       

		/**
		 * @brief get sats of single system def=true (give default values at least)
		 * @param[in] gsys system
		 * @param[in] def  if empty, return default setting or not. default is true
		 * @return set<string> : sats of single system def=true (give default values at least)
		 */
		set<string>  sat(GSYS gsys, bool def = true);  

		/**
		 * @brief get obs(eg.C1C ..) of single system def=true (give default values at least)
	     * @param[in] gsys system
		 * @param[in] def  if empty, return default setting or not. default is true
		 * @return set<string> : obs of single system def=true (give default values at least)
		 */
		set<string>  obs(GSYS gsys, bool def = true);  

		/**
		 * @brief get nav of single system def=true (give default values at least)
		 * @param[in] gsys system
		 * @param[in] def  if empty, return default setting or not. default is true
		 * @return set<string> : nav of single system def=true (give default values at least)
		 */
		set<string>  nav(GSYS gsys, bool def = true); 

		/**
		 * @brief get extending gobs list with complete singals
		 * @param[in] gsys system
		 * @return set<string> : extending gobs list with complete singals
		 */
		set<string> gobs(GSYS gsys); 


		/**
		 * @brief get obs type of system:gsys
		 * @param[in] gsys system
		 * @return vector<GOBSTYPE> : obs type of system:gsys
		 */
		vector<GOBSTYPE> type(GSYS gsys);

		/**
		 * @brief get obs band of system:gsys
		 * @param[in] gsys system
		 * @return vector<GOBSBAND> : obs band of system:gsys
		 */
		vector<GOBSBAND> band(GSYS gsys);

		/**
		 * @brief get obs ATTR of system:gsys
		 * @param[in] gsys system
		 * @return vector<GOBSATTR> : obs ATTR of system:gsys
		 */
		vector<GOBSATTR> attr(GSYS gsys);

		/**
		 * @brief get the freq used in proc
		 * @param[in] gsys system
		 * @return vector<FREQ_SEQ> : GNSS freq Sequence
		 */
		vector<FREQ_SEQ> freqs(GSYS gsys);

		/**
		 * @brief get the band order
		 * @param[in] gsys system
		 * @return map<FREQ_SEQ, GOBSBAND> : band index
		 */
		map<FREQ_SEQ, GOBSBAND> band_index(GSYS gsys);

		/**
		* @brief get the freq order
		* @param[in] gsys system
		* @return map<FREQ_SEQ, GOBSBAND> : freq index
		*/
		map<GOBSBAND, FREQ_SEQ> freq_index(GSYS gsys);

	protected:
		/**
		 * @brief get obs type of system:gsys form XML
		 * @param[in] gsys system
		 * @return vector<GOBSTYPE> : obs type of system:gsys form XML
		 */
		vector<GOBSTYPE> _type(GSYS gsys);

		/**
		 * @brief get obs band of system:gsys form XML
		 * @param[in] gsys system
		 * @return vector<GOBSBAND> : obs band of system:gsys form XML
		 */
		vector<GOBSBAND> _band(GSYS gsys);

		/**
		 * @brief get obs attributes of system:gsys form XML
		 * @param[in] gsys system
		 * @return vector<GOBSATTR> : obs attributes of system:gsys form XML
		 */
		vector<GOBSATTR> _attr(GSYS gsys);

		/**
		 * @brief change GSYS to string::gsys
		 * @param[in] gsys system
		 * @return string : gsys
		 */
		string           _gsys(GSYS gsys);

		/**
		 * @brief get the freq used in proc
		 * @param[in] gsys system
		 * @return vector<FREQ_SEQ> : GNSS freq Sequence
		 */
		vector<FREQ_SEQ> _freqs(GSYS gsys);

	protected:
		map<GSYS, vector<string> > _band_str;           ///< default set
		map<GSYS, vector<string> > _type_str;           ///< default set
		map<GSYS, vector<string> > _attr_str;           ///< default set


	};

} // namespace

#endif
