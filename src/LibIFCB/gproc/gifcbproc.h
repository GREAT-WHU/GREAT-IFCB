/**
*
* @verbatim
History
-1.0 hjzheng  2020-10-06  creat the file.
-1.1 hjzheng  2020-10-18  Adding Doxygen Style Code Remarks.
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gifcbproc.h
* @brief	compute Inter-frequency Clock Bias(IFCB)
* @author   hjzheng, Wuhan University
* @version	1.0.0
* @date		2020-10-18
*
*/
#ifndef GIFCBPROC_H
#define GIFCBPROC_H

#include <algorithm>
#include "gutils/ExportIFCB.h"
#include "gutils/gmutex.h"
#include "gset/gsetbase.h"
#include "gall/gallobs.h"
#include "gall/gallambflag.h"
#include "gall/gallproc.h"
#include "gall/gallnav.h"
#include "gdata/gifcb.h"
#include "gall/gallbias.h"
#include "gdata/gobsgnss.h"
#include "gambfix/gambcommon.h"
#include "gset/gcfg_ifcb.h"
#include "gcoders/ambflag.h"
#include "gio/gfile.h"
#include "gutils/ginfolog.h"
#include "gproc/gcorbds.h"
namespace great
{
	typedef map<string, map<int, map<string, double>>> t_site_epo_sat_GFIF;
	/**
	* @brief class for ifcb main process.
	*/
	class IFCB_LIBRARY_EXPORT t_gifcbproc 
	{
	public:

		/**
		* @brief default constructor, init some internal variables.
		* @param[in] set	ifcb setting of xml.
		* @param[in] data   ifcb used data
		* @param[in] log    log for ifcb process
		*/
		t_gifcbproc (t_gsetbase* set, t_gallproc* data, t_glog* log);

		/** @brief default destructor. */
		virtual ~t_gifcbproc();

		/** @brief output IFCB File. */
		bool GenerateProduct();

		/**
		* @brief batch processing loop over epoch.
		* @param[in] beg   begin epoch.
		* @param[in] end   end epoch.
		* @param[in] intv  sampling interval.
		* @return	 true - successfully; false - failure
		*/
		bool processBatch(const t_gtime& beg, const t_gtime& end,const double &intv);

	protected:

		/** @brief General log output. */
		t_glog*				_glog;         

		 /** @brief Base setting. */
		t_gsetbase*			_set; 

		/** @brief Data used to process ifcb. */
		t_gallproc*         _data = nullptr;           ///< all data
		t_gallobs*          _allobs = nullptr;         ///< obs data
		shared_ptr<t_gallambflag>      _allambflag = nullptr;     ///< ambflag data
		shared_ptr<t_gallambflag>      _allambflag13 = nullptr;   ///< ambflag13 data
		t_gallbias*			_allbias = nullptr;        ///< dcb data
		t_gallnav*          _allnav = nullptr;         ///< used in glonass process

		shared_ptr<t_gifcb>			_ifcb_data;		   ///< storage ifcb result


		set<string>     _sites;  ///< sites list
		set<string>     _sats;   ///< satellites list
		set<string>     _sys;    ///< system list

		map< GSYS, map<FREQ_SEQ, GOBSBAND> > _band_index;   ///< band used on xml
		t_gbds_codebias_cor   _bds_codebias_cor;			///< correct bds code bias
		t_site_epo_sat_GFIF _all_site_GFIF;					///< storage Epoch-Diff GFIF for each site/epo/sat


	protected:


		/**
		* @brief batch processing IFCB
		* @param[in] beg   begin epoch.
		* @param[in] end   end epoch.
		* @param[in] intv  sampling interval.
		* @return	 true - successfully; false - failure
		*/
		bool _processIFCB(const t_gtime& beg, const t_gtime& end, const double &intv);


		/**
		* @brief Init data.
		* @param[in]
		* @return    return false if no data
		*/
		bool _initProcData(t_gallproc * data);

		/**
		* @brief set sites and satellites and init params used in IFCB process.
		* @return	 true - successful, false failure
		*/
		bool _initSatSite();

		/**
		* @brief init information of ambiguity cycle slip
		* @param[in] beg	begin time
		* @param[in] end	end time
		* @param[in] intv	sampling interval
		* @return	 true - successful, false failure
		*/
		bool _initAmbiguityCycleSlip(const t_gtime& beg,const t_gtime& end,const double& intv);

		/**
		* @brief combine GFIF observations for one site
		* @param[in] site          site name.
		* @param[in] T_beg         start epoch.
		* @param[in] T_end         end epoch.
		* @param[in] intv          sampling rate in seconds.
		* @param[out] one_site_amb storage the GFIF result 
		* @param[out] OW		   storage the ambiguity arc
		* @return    true form combine observations successfully, false failure
		*/
		bool _combineObsGFIF(string site, const t_gtime& T_beg, const t_gtime& T_end, double intv,
			t_epo_sat_amb& one_site_GFIF, t_all_sats_OW& OW);

		/**
		* @brief caculate Epoch-difference IFCB value for all sites
		* @param[in] site          site name.
		* @param[in] T_beg         start epoch.
		* @param[in] T_end         end epoch.
		* @param[in] intv          sampling rate in seconds.
		*/
		void _caculateEpoDifIFCB(const t_gtime& T_beg, const t_gtime& T_end, double intv);

		/**
		* @brief caculate final Epoch-difference IFCB by estimate iteratively
		* @param[in] site          site name.
		* @param[in] T_beg         start epoch.
		* @param[in] T_end         end epoch.
		* @param[in] intv          sampling rate in seconds.
		*/
		void _cacualteAverageEpoDifIFCB(const t_gtime& T_beg, const t_gtime& T_end, double intv);

		/**
		* @brief add zero-mean constraint
		* @param[in] site          site name.
		* @param[in] T_beg         start epoch.
		* @param[in] T_end         end epoch.
		* @param[in] intv          sampling rate in seconds.
		*/
		void _addPriConstraint(const t_gtime& T_beg, const t_gtime& T_end, double intv);

		/**
		* @brief recover absolute IFCB value
		* @param[in] site          site name.
		* @param[in] T_beg         start epoch.
		* @param[in] T_end         end epoch.
		* @param[in] intv          sampling rate in seconds.
		*/
		void _recoverAbsoluteIFCB(const t_gtime& T_beg, const t_gtime& T_end, double intv);

	};


}
#endif //
