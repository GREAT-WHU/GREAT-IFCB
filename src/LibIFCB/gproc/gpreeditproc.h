/**
*
* @verbatim
History
-1.0 hzjheng	2022-07-02 created
@endverbatim
* Copyright (c) 2020, Wuhan University. All rights reserved.
*
* @file		    gpreeditproc.h
* @brief		main control class for detecting ambiguity cycle slip
* @author       hjzheng, Wuhan University
* @version		1.0.0
* @date		    2022-07-02
*
*/


#ifndef GPREEDITPROC_H
#define GPREEDITPROC_H

#include "gutils/ExportIFCB.h"
#include "gutils/gfileconv.h"

#include "gset/gsetgen.h"
#include "gset/gsetgnss.h"
#include "gset/gsetbase.h"
#include "gset/gsetinp.h"

#include "gio/glog.h"
#include "gio/gfile.h"

#include "gall/gallambflag.h"
#include "gall/gallobs.h"
#include "gall/gallnav.h"
#include "gall/gallproc.h"

#include "gcoders/ambflag.h"

#include "gproc/goutlierproc.h"

#include <algorithm>
#include <stdint.h>
#include <string>

using namespace gnut;

namespace great
{

	/** @brief information of observations used to detecting ambiguity cycle slip . */
	class IFCB_LIBRARY_EXPORT t_gobsinfo
	{
	public:

		map<GOBS, bool>       mwslip;						///< ambiguity cycle slip flag based on MW combination
		map<GOBS, bool>       gfslip;						///< ambiguity cycle slip flag based on GF combination
		map<GOBS, t_gtime>    last_time;					///< store the last exsiting epoch of the observation
		map<GOBS, int>        epo_num;                      ///< store the number of consecutive epochs
		map<GOBS, double>     GF;                           ///< store GF combination values
		map<GOBS, pair<double, double> >   statistic_MW;    ///< store the mean and std of MW combination 
		map<GOBS, vector<double>>          origin_MWs;      ///< store MW combination values
	};

	/** @brief class for detecting ambiguity cycle slip . */
	class IFCB_LIBRARY_EXPORT t_gpreeditproc
	{
	public:
		/**
		* @brief default constructor, init some internal variables.
		* @param[in] set	ifcb setting of xml.
		* @param[in] data   ifcb used data
		* @param[in] log    log for ifcb process
		*/
		t_gpreeditproc(t_gsetbase* set, t_glog* glog, t_gallproc* gdata);

		/** @brief default destructor. */
		virtual ~t_gpreeditproc();

		/**
		* @brief batch processing loop over epoch.
		* @param[in] begT		begin epoch.
		* @param[in] endT		end epoch.
		* @param[in] sampling	sampling interval.
		* @return	 true - successfully; false - failure
		*/
		bool ProcessBatch(const t_gtime& begT, const t_gtime& endT, double sampling);

	protected:		
		t_gsetbase*                    _set;			///< base setting
		t_glog*                        _log;			///< general log output
		t_gtime                        _beg_time;		///< begin time of process
		t_gtime                        _end_time;		///< end time of process
		set<string>                    _sys_list;		///< system list
		set<string>                    _sat_list;		///< satellites list
		set<string>                    _rec_list;		///< station list
		int                            _interval;		///< sampling interval
		map<string, t_gtriple>         _rec_crds;		///< station coordinates
		map< GSYS, map<FREQ_SEQ, GOBSBAND> > _band_index;   ///< band used on xml

		t_gtime	_crt_time;			///< current time    
		string  _crt_rec;			///< current station

		bool   _check_mw;           ///< check MW combination 
		bool   _check_gf;           ///< check GF combination
		bool   _check_gap;          ///< check data gap, ambiguity cycle slip is detected if data missing longer
		bool   _check_short;        ///< check short arc, short data arc would be deleted

		double _mw_limit;           ///< MW combination limit, default 4 cycles
		double _gf_limit;           ///< GF combination limit, default 0.15 meters
		double _min_elev;           ///< cutoff elevation, default is 7
		int    _gap_limit;          ///< gapped epoch number, default 20 epoch
		int    _short_limit;        ///< short epoch number, default 10 epoch
		

		t_gallobs*       _allobs;						///< observation data
		t_gallnav*       _allnav;						///< navigation ephemeris
		t_gallambflag* _allambflag;						///< ambflag data for frequency 1 and 2
		t_gallambflag* _allambflag13;					///< ambflag data for frequency 1 and 3
		map<string, map<string, map<GOBS, t_gobsinfo>>>    _obsinfo_data;			///< information of observations

		t_goutliers_process                                _outliers_proc;			///< detect range outliers

		/**
		* @brief  output setting informations
		*/
		void _outputSettingInfo();

		/**
		* @brief  initial setting parameters
		*/
		void _initSetting();

		/**
		* @brief  main process function of detcting ambiguity cycle slip
		* @param[in] site   station name
		* @param[in] begT   begin epoch
		* @param[in] endT	end epoch
		* @return	 true - successfully; false - failure
		*/
		bool _mainProcess(const string& site, const t_gtime& begT, const t_gtime& endT);

		/**
		* @brief  detect ambiguity cycle slip based on MW and GF combination 
		* @param[in] oneObs	one satellite observation of the current site
		* @param[in] b1		band of frequency 1
		* @param[in] b2		band of frequency 2
		*/
		void _detectMWGF(shared_ptr<t_gobsgnss> oneObs, GOBSBAND b1, GOBSBAND b2);

		/**
		* @brief  get satellite elevation and position infomation
		* @param[in]  sat	 satllite name
		* @param[out] elev	 the sat elevation of current stie
		* @param[out] oneObs add sat position on the current observation
		*/
		bool _getSatInfo(string sat, double& elev, t_spt_gobs oneObs);

		/**
		* @brief  write the information of ambiguity cycle slip to the _allambflag and _allambflag13
		*/
		void _writeAmbFlagData();

	};
	
}

#endif /*GPREEDITPROC_H*/