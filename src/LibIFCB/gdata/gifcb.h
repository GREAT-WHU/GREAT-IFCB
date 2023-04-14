/**
*
* @verbatim
History
-1.0 bwang  2019-02-23  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gifcb.h
* @brief	Storage the ifcb files' data
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/

#ifndef GIFCB_H
#define GIFCB_H

#include "gdata/gdata.h"
#include "gutils/gtime.h"
#include "gutils/ExportIFCB.h"

using namespace gnut;

namespace great
{
	/**
	*@brief	   Class for storaging one satellite ifcb data
	*/
	class IFCB_LIBRARY_EXPORT t_ifcbrec
	{
	public:

		/** @brief default constructor. */
		t_ifcbrec();

		/** @brief default destructor. */
		~t_ifcbrec() {};

		/** @brief get validity of ifcb data value */
		bool valid() { return this->npoint > 2 && this->sigma < 0.10; }

		string  obj;      ///< ifcb objection may be site or satellite
		int     npoint;   ///< site number
		double  value;    ///< ifcb value
		double  sigma;    ///< std
		bool    isRef;    ///< true set as a reference
	};

	/** map container using satellite name as a index for storaging t_ifcbrec ptr , one epoch/all satellite  */
	typedef map<string, shared_ptr<t_ifcbrec>> IFCB_LIBRARY_EXPORT one_epoch_ifcb;

	/**
	*@brief	 Class for storaging all epoch/all satellite ifcb data
	*/
	class IFCB_LIBRARY_EXPORT t_gifcb : public t_gdata
	{
	public:

		/** @brief default constructor. */
		t_gifcb();

		/** @brief default destructor. */
		virtual ~t_gifcb();

		/**
		* @brief add ifcb data of one epoch/one satellite.
		* @param[in]  epoch		  epoch.
		* @param[in]  prn		  satellite name.
		* @param[in]  one_sat_ifcb ifcb data of one epoch/one satellite
		* @return	  void
		*/
		void add_sat_ifcb(t_gtime epoch, string prn, t_ifcbrec one_sat_ifcb);

		/**
		* @brief add ifcb data of one epoch/all satellite.
		* @param[in]  epoch		  epoch.
		* @param[in]  one_sat_ifcb ifcb data of one epoch/all satellite
		* @return	  void
		*/
		void add_epo_ifcb(t_gtime epoch, one_epoch_ifcb one_epoch_ifcb);

		/**
		* @brief get ifcb data, all epoch/all satellite.
		* @return	 ifcb data
		*/
		map<t_gtime, one_epoch_ifcb>& get_ifcb(){ return _ifcb; };

		/**
		* @brief get validity of ifcb data's value in class t_ifcbrec,one epoch/one satellite.
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return the ifcb value of satellite in epoch
		*/
		bool get_ifcb_value_valid(const t_gtime& t, const string& str) { return _ifcb[t][str]->valid(); };

		/**
		* @brief get ifcb data's value in class t_ifcbrec,one epoch/one satellite.
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return the ifcb value of satellite in epoch
		*/
		double get_ifcb_value(const t_gtime& t, const string& str) { return _ifcb[t][str]->value; };

		/**
		* @brief get ifcb data's sigma in class t_ifcbrec,one epoch/one satellite.
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return the ifcb sigma of satellite in epoch
		*/
		double get_ifcb_sigma(const t_gtime& t, const string& str) { return _ifcb[t][str]->sigma; };

		/**
		* @brief get upd data's npoint in class t_updrec,one epoch/one satellite.
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return number of sites used in this sat's upd estimation
		*/
		double get_ifcb_npoint(const t_gtime& t, const string& str) { return _ifcb[t][str]->npoint; };


		/**
		* @brief get ifcb data of one epoch/all satellite.
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return the ifcb data in epoch
		*/
		one_epoch_ifcb& get_epo_ifcb(const t_gtime& t) { return _ifcb[t]; };

		/**
		* @brief reset ifcb data's value in class t_ifcbrec of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @param[in]  value	  value that ifcb data's value will reset.
		* @param[in]  sigma	  std that ifcb data's sigma will reset.
		* @param[in]  npoint  site number that ifcb data's npoint will reset.
		* @return	  void
		*/
		void reset_ifcb(const t_gtime& t, const string& str, const double& value,
			const double& sigma, const int& npoint);

		/**
		* @brief reset ifcb data's value in class t_ifcbrec of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @param[in]  value	  value that ifcb data's value will reset.
		* @return	  void
		*/
		void reset_ifcb_value(const t_gtime& t, const string& str, const double& value);

		/**
		* @brief reinitialize ifcb data of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @return	  void
		*/
		void re_init_ifcb(const t_gtime& t, string str);  //str maybe site/sats

		/**
		* @brief delete ifcb data of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @return	  void
		*/
		void delete_ifcb(const t_gtime& t, const string& str) { _ifcb[t].erase(str); };

		/**
		* @brief judge ifcb data is usable or not, one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @return	  void
		*/
		bool ifcb_usable(const t_gtime& t, const string& str);

		/**
		* @brief reset ifcb data's value in class t_ifcbrec of one epoch/one satellite.
		* @param[in]  pre_t	      previous epoch.
		* @param[in]  str	      satellite name.
		* @param[in]  current_t	  current epoch.
		* @param[in]  is_first	  if it's first epoch , re-init ifcb data.
		* @param[in]  is_site     if it's site ifcb, remove old ifcb,add new ifcb.
		* @return	  void
		*/
		void copy_ifcb(const t_gtime& pre_t, const string& str, const t_gtime& current_t,
			const bool& is_first, const bool& is_site);

		/**
		* @brief set the valid begin epoch.
		* @param[in]  t	    current epoch.
		* @return	  void
		*/
		void set_valid_beg(const t_gtime& t);

		/**
		* @brief get the valid begin epoch.
		* @return the valid begin epoch of ifcb
		*/
		t_gtime get_valid_beg(){ return _valid_beg; }

	protected:

		map<t_gtime, one_epoch_ifcb> _ifcb;       ///< ifcb map container of all epoch/all satellite
		t_gtime                     _valid_beg;   ///< valid begin epoch

	private:

	};

}
#endif // !GALLPLANETEPH_H