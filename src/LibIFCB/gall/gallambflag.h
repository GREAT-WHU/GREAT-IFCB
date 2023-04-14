/**
*
* @verbatim
History
-1.0 bwang  2019-02-20  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gallambflag.h
* @brief	Storage the XXXXddd0.yyo.log/log13 files' data(more than one site)
*				XXXX  ---- SITE name
*				 ddd  ---- Doy of the file
*				  yy  ---- year
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/

#ifndef GALLAMBFLAG_H
#define GALLAMBFLAG_H

#include "gdata/gambflag.h"
#include "gutils/gtime.h"

using namespace std;
using namespace gnut;

namespace great
{

	/** 
	* @brief map container using station name as a index for storaging t_vec_ambflag data of all station 
	*/
	typedef map<string, t_gambflag>  t_map_allambflag;

	/**
	*@brief	   Class for all XXXXddd0.yyo.ambflag file data storaging
	*/
	class IFCB_LIBRARY_EXPORT t_gallambflag : public t_gdata
	{

	public:

		/** @brief default constructor. */
		t_gallambflag(ID_TYPE t);

		/** @brief default destructor. */
		virtual ~t_gallambflag();

		/**
		* @brief add ambflag head of one station.
		* @param[in]  sta		station's name.
		* @param[in]  ambflag	ambflag head of one station.
		* @return	  void
		*/
		void addAmbFlagHead(string site_name, const ambflag_hd& header_data);

		/**
		* @brief add ambflag data of one station.
		* @param[in]  sta		station's name.
		* @param[in]  ambflag	ambflag data of one station.
		* @return	  void
		*/
		void addAmbFlag(string site_name, const t_gambflag& amb_flag);

		/**
		* @brief add ambflag data of one satellite in one station.
		* @param[in]  sat		satellite's name.
		* @param[in]  sta		station's name.
		* @param[in]  ambflag	ambflag data of one satellite in one station.
		* @return	  void
		*/
		void addAmbFlagData(string site_name, string sat_name, const ambflag_data& data);

		/**
		* @brief return ambflag data of one station.
		* @param[in]  sta		station's name.
		* @return ambflag data of one station
		*/
		t_gambflag& getOneAmbFlag(string site_name);

		/**
		* @brief return ambflag data of all station.
		* @return ambflag data of all station
		*/
		t_map_allambflag& getAllAmbFlag();

		/**
		* @brief  return all system designated satellites' prn of all station.
		* @return all system designated satellites' prn of all station
		*/
		set<string> getSatList();
		
		/**
		* @brief  return all station.
		* @return all station
		*/
		set<string> getSiteList();

	protected:
		t_map_allambflag _map_allambflag; ///< map of station name and ambflag data for all sites

	};

} // namespace

#endif
