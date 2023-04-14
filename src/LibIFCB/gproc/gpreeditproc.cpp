/**
*
* @verbatim
History
-1.0 hzjheng	2022-07-02 created
@endverbatim
* Copyright (c) 2020, Wuhan University. All rights reserved.
*
* @file		    gpreeditproc.cpp
* @brief		main control class for detecting ambiguity cycle slip
* @author       hjzheng, Wuhan University
* @version		1.0.0
* @date		    2022-07-02
*
*/

#include "gproc/gpreeditproc.h"
#include "gset/gsetpreedit.h"

namespace great
{

	t_gpreeditproc::t_gpreeditproc(t_gsetbase* set, t_glog* glog, t_gallproc* gdata) :
		_set(set),
		_log(glog),
		_check_short(true),
		_allambflag(nullptr),
		_outliers_proc(glog)
	{
		// get data 
		_allobs = dynamic_cast<t_gallobs*>((*gdata)[t_gdata::ALLOBS]);
		_allnav = dynamic_cast<t_gallnav*>((*gdata)[t_gdata::GRP_EPHEM]);
		_allambflag = dynamic_cast<t_gallambflag*>((*gdata)[t_gdata::AMBFLAG]);
		_allambflag13 = dynamic_cast<t_gallambflag*>((*gdata)[t_gdata::AMBFLAG13]);

		// common intialization
		_initSetting();

		// output setting infomation
		_outputSettingInfo();

	}

	t_gpreeditproc::~t_gpreeditproc()
	{
	}

	bool t_gpreeditproc::ProcessBatch(const t_gtime& begT, const t_gtime& endT, double sampling)
	{
		_beg_time = begT;
		_end_time = endT;
		_interval = sampling;

		// Check Time Duration
		if (begT > endT)
		{
			_log->comment(1, _crt_rec, "Error in time settings : beg time >= end time !");
			_log->comment(1, _crt_rec, "The beg time is : " + begT.str_ymdhms());
			_log->comment(1, _crt_rec, "The end time is : " + endT.str_ymdhms());
			return false;
		}

		// Check Ambiguity CycleSlip
		for (auto site : _rec_list) {
			if (!_mainProcess(site,begT, endT)) {
				_log->comment(0, "WARNING", "Prcess site: "+site+" fail!");
			}
			_log->comment(0, "CheckAmbiguityCycleSlip", "Process site: " + site + " finish!");
		}

		// Write Ambflagdata
		_writeAmbFlagData();

		return true;
	}

	void  t_gpreeditproc::_outputSettingInfo()
	{
		stringstream str2;
		if (_log) {
			_log->comment(0, "NOTE", "***********************************************");
			_log->comment(0, "NOTE", "+++ PreeditProc Setting Parameters from XML file  +++");
		}
		if (_log)_log->comment(0, "NOTE", "PreEdit(MW):  " + dbl2str(_min_elev));
		// MW
		if (_log)_log->comment(0, "NOTE", "PreEdit(MW):  " + bl2str(_check_mw));
		if (_check_mw)
		{
			str2.str(""); str2 << fixed << left << setw(7) << setprecision(2) << _mw_limit;
			if (_log)_log->comment(0, "NOTE", "MWLimitation :  " + str2.str() + " cycles");
		}
		// GF
		if (_log)_log->comment(0, "NOTE", "TurboEdit(GF):  " + bl2str(_check_gf));
		if (_check_gf)
		{
				str2.str(""); str2 << fixed << left << setw(7) << setprecision(2) << _gf_limit;
				if (_log)_log->comment(0, "NOTE", "GFLimitation :  " + str2.str() + " meters");
		}
		// check gap
		if (_log)_log->comment(0, "NOTE", "Check Gap    :  " + bl2str(_check_gap));
		if (_check_gap)
		{
			str2.str(""); str2 << fixed << left << setw(7) << _gap_limit;
			if (_log)_log->comment(0, "NOTE", "GapLimit     :  " + str2.str() + " epochs ");
		}
		// check short
		if (_log)_log->comment(0, "NOTE", "Check Short    :  " + bl2str(_check_short));
		if (_check_short)
		{
			str2.str(""); str2 << fixed << left << setw(7) << _short_limit;
			if (_log)_log->comment(0, "NOTE", "Short Limit     :  " + str2.str() + " epochs ");
		}
	}

	void t_gpreeditproc::_initSetting()
	{
		_beg_time = dynamic_cast<t_gsetgen*>(_set)->beg();
		_end_time = dynamic_cast<t_gsetgen*>(_set)->end();
		_sys_list = dynamic_cast<t_gsetgen*>(_set)->sys();      
		_rec_list = dynamic_cast<t_gsetgen*>(_set)->recs();     
		for (string it : _sys_list)                               
		{
			GSYS gsys = t_gsys::str2gsys(it);
			std::set<string> sat_temp = dynamic_cast<t_gsetgnss*>(_set)->sat(gsys);
			_sat_list.insert(sat_temp.begin(), sat_temp.end());  
			_band_index[gsys] = dynamic_cast<t_gsetgnss*>(_set)->band_index(gsys);
		}

		_min_elev = dynamic_cast<t_gsetpreedit*>(_set)->minimum_elev();
		_check_mw = dynamic_cast<t_gsetpreedit*>(_set)->checkMW(_mw_limit);            
		_check_gf = dynamic_cast<t_gsetpreedit*>(_set)->checkGF(_gf_limit); 
		_check_gap = dynamic_cast<t_gsetpreedit*>(_set)->checkGap(_gap_limit);
		_check_short = dynamic_cast<t_gsetpreedit*>(_set)->checkShort(_short_limit);

		if (!_check_gap)  _gap_limit = 999999;

		// get site Apr Crd from obs file
		for (const auto& it : _rec_list)
		{
			t_gtriple apr_crd = _allobs->getsitecrd(it);
			if (!apr_crd.zero()) {
				_rec_crds[it] = apr_crd;
			}
			else {
				_rec_crds[it] = t_gtriple();
			}
		}
	}

	bool t_gpreeditproc::_mainProcess(const string& site, const t_gtime& begT, const t_gtime& endT)
	{
		_crt_rec = site;
		_crt_time = begT;
		vector<shared_ptr<t_gobsgnss>>       epoData;
		map<string, map<GOBS, set<GOBS> > > markEndTable;
		while (_crt_time <= endT)
		{
			epoData = _allobs->obs_pt(_crt_rec, _crt_time);

			if (epoData.size() == 0)
			{
				_crt_time = _crt_time + _interval;
				continue;
			}

			for (auto& oneObs : epoData)
			{
				string sat = oneObs->sat();
				GSYS gsys = oneObs->gsys();
				string site = oneObs->site();

				if (_crt_time > _beg_time)
				{
					auto satdata_pre = _allobs->obs_prn_pt(_crt_rec, sat, _crt_time - _interval * _gap_limit, _crt_time - _interval);
					if (!satdata_pre.empty()) {
						auto epodata_pre = satdata_pre.rbegin();
						_outliers_proc.flagRangeOutliers(*epodata_pre, oneObs, _interval);

					}
				}
				GOBSBAND b1 = _band_index[oneObs->gsys()][FREQ_1];
				GOBSBAND b2 = _band_index[oneObs->gsys()][FREQ_2];
				GOBSBAND b3 = _band_index[oneObs->gsys()][FREQ_3];

				_detectMWGF(oneObs, b1, b2);
				_detectMWGF(oneObs, b1, b3);
			}
			// Next
			_crt_time = _crt_time + _interval;

		}
		return true;
	}

	void t_gpreeditproc::_detectMWGF(shared_ptr<t_gobsgnss> oneObs, GOBSBAND b1, GOBSBAND b2)
	{
		string sat = oneObs->sat();

		GOBS obs_L1 = oneObs->select_phase(b1);
		GOBS obs_L2 = oneObs->select_phase(b2);
		if (obs_L1 == GOBS::X || obs_L2 == GOBS::X) {
			return;
		}

		t_gobs gobs_P1 = t_gobs(oneObs->select_range(b1));
		t_gobs gobs_P2 = t_gobs(oneObs->select_range(b2));
		t_gobs gobs_L1 = t_gobs(obs_L1);
		t_gobs gobs_L2 = t_gobs(obs_L2);

		double sat_elev = _min_elev;
		double scale_factor = 1;
		double thres_MW = 0, thres_GF = 0;
		if (this->_getSatInfo(sat,sat_elev,oneObs))
		{
			if (sat_elev < _min_elev) sat_elev = _min_elev;
		}

		ostringstream os;
		os.str("");


		double crt_MW_val = oneObs->MW_cycle(gobs_L1, gobs_L2, gobs_P1, gobs_P2);     // MW combination [cycles]
		double crt_GF_val = oneObs->L4(gobs_L1, gobs_L2);                             // Geometry-free combination [meters]

		os << fixed << setw(10) << " MW[c] " << oneObs->sat() << "  " << setw(6) << _crt_time.sod()
			<< "  " << gobs2str(obs_L1) << "  " << gobs2str(obs_L2)
			<< "  " << gobs2str(gobs_P1.gobs()) << "  " << gobs2str(gobs_P2.gobs())
			<< setw(15) << setprecision(4) << crt_MW_val << endl;

		os << fixed << setw(10) << " GF[m] " << oneObs->sat() << "  " << setw(6) << _crt_time.sod()
			<< "  " << gobs2str(obs_L1) << "  " << gobs2str(obs_L2)
			<< setw(15) << setprecision(4) << crt_GF_val << endl;

		// check existence of observation
		if (_obsinfo_data[_crt_rec][sat][obs_L1].epo_num.find(obs_L2) == _obsinfo_data[_crt_rec][sat][obs_L1].epo_num.end()
			|| _obsinfo_data[_crt_rec][sat][obs_L1].mwslip[obs_L2] || _obsinfo_data[_crt_rec][sat][obs_L1].gfslip[obs_L2]
			|| oneObs->getlli(obs_L1) >= 1 || oneObs->getlli(obs_L2) >= 1)
		{
			_obsinfo_data[_crt_rec][sat][obs_L1].epo_num[obs_L2] = 1;
			_obsinfo_data[_crt_rec][sat][obs_L1].mwslip[obs_L2] = false;
			_obsinfo_data[_crt_rec][sat][obs_L1].gfslip[obs_L2] = false;
			_obsinfo_data[_crt_rec][sat][obs_L1].last_time[obs_L2] = _crt_time;
			_obsinfo_data[_crt_rec][sat][obs_L1].GF[obs_L2] = crt_GF_val;
			_obsinfo_data[_crt_rec][sat][obs_L1].statistic_MW[obs_L2] = make_pair(crt_MW_val, 0.0);

			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].clear();
			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].push_back(0.0);
			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].push_back(crt_MW_val);

			oneObs->addlli(obs_L1, 1);
			oneObs->addlli(obs_L2, 1);
			
			return;
		}

		double dt = _crt_time.diff(_obsinfo_data[_crt_rec][sat][obs_L1].last_time[obs_L2]);
		double dif_MW = crt_MW_val - _obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].back();
		bool badRange = oneObs->getoutliers(gobs_P1.gobs()) >= 1 || oneObs->getoutliers(gobs_P2.gobs()) >= 1;

		// check gap and range outliers
		if (dt > _interval && abs(dif_MW) > 8 && !badRange || _check_gap && dt > _gap_limit * _interval)
		{
			_obsinfo_data[_crt_rec][sat][obs_L1].epo_num[obs_L2] = 1;
			_obsinfo_data[_crt_rec][sat][obs_L1].mwslip[obs_L2] = false;
			_obsinfo_data[_crt_rec][sat][obs_L1].gfslip[obs_L2] = false;
			_obsinfo_data[_crt_rec][sat][obs_L1].last_time[obs_L2] = _crt_time;
			_obsinfo_data[_crt_rec][sat][obs_L1].GF[obs_L2] = crt_GF_val;
			_obsinfo_data[_crt_rec][sat][obs_L1].statistic_MW[obs_L2] = make_pair(crt_MW_val, 0.0);

			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].clear();
			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].push_back(0.0);
			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].push_back(crt_MW_val);


			ostringstream msg;
			msg << fixed << setw(10) << " Data Gap or Max_diff_MW : " << _crt_time.str_hms() << "   " << oneObs->sat() << "   " << gobs2str(obs_L1)
				<< "   " << gobs2str(obs_L2) << setw(16) << setprecision(3) << dif_MW << setw(16) << setprecision(3) << dt
				<< setw(16) << setprecision(3) << _gap_limit * _interval;
			_log->comment(1, _crt_rec, msg.str());

			oneObs->addlli(obs_L1, 1);
			oneObs->addlli(obs_L2, 1);

			return;
		}

		int gapNum = dt / _interval - 1;

		// check GF combination
		if (_check_gf)
		{
			double pre_GF_val = _obsinfo_data[_crt_rec][sat][obs_L1].GF[obs_L2];

			_obsinfo_data[_crt_rec][sat][obs_L1].GF[obs_L2] = crt_GF_val;

			if (sat_elev < 15)
			{
				thres_GF = -_gf_limit * 1.0 / 15.0 * sat_elev + 2 * _gf_limit;
			}
			else
			{
				thres_GF = _gf_limit;
			}

			if (dt >= 90 && gapNum >= 3)
			{
				thres_GF = min(thres_GF * gapNum, 0.35);
			}

			if (abs(crt_GF_val - pre_GF_val) > thres_GF)
			{
				_obsinfo_data[_crt_rec][sat][obs_L1].gfslip[obs_L2] = true;
				ostringstream msg;
				msg << fixed << setw(10) << " GF cycle slip : " << _crt_time.str_hms() << "   " << oneObs->sat() << "   " << gobs2str(obs_L1)
					<< "   " << gobs2str(obs_L2) << setw(16) << setprecision(3) << pre_GF_val << setw(16) << setprecision(3) << crt_GF_val
					<< setw(16) << setprecision(3) << (crt_GF_val - pre_GF_val) << setw(16) << setprecision(3) << thres_GF;
				_log->comment(1, _crt_rec, msg.str());
			}

		}

		// check MW combination
		if (_check_mw && !badRange)
		{
			_obsinfo_data[_crt_rec][sat][obs_L1].epo_num[obs_L2] += 1;
			_obsinfo_data[_crt_rec][sat][obs_L1].last_time[obs_L2] = _crt_time;

			int epo_num = _obsinfo_data[_crt_rec][sat][obs_L1].epo_num[obs_L2];
			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].push_back(crt_MW_val);

			double pre_MW_val = _obsinfo_data[_crt_rec][sat][obs_L1].statistic_MW[obs_L2].first;
			double pre_MW_std = _obsinfo_data[_crt_rec][sat][obs_L1].statistic_MW[obs_L2].second;
			
			double smooth_MW_val = pre_MW_val * (epo_num - 1) / epo_num + crt_MW_val / epo_num;
			_obsinfo_data[_crt_rec][sat][obs_L1].statistic_MW[obs_L2].first = smooth_MW_val;
			double smooth_MW_std = pre_MW_std * (epo_num - 1) / epo_num + pow(crt_MW_val - pre_MW_val, 2) / epo_num;
			_obsinfo_data[_crt_rec][sat][obs_L1].statistic_MW[obs_L2].second = smooth_MW_std;

			if (sat_elev < 20) thres_MW = -_mw_limit * 0.1 * sat_elev + 3 * _mw_limit;
			else               thres_MW = _mw_limit;

			if (gapNum <= 2)		scale_factor = 1.0;
			else if (gapNum <= 4)	scale_factor = 1.25;
			else if (gapNum <= 8)	scale_factor = 1.5;
			else					scale_factor = 2.0;

			if (abs(crt_MW_val - pre_MW_val) > thres_MW * scale_factor && !double_eq(thres_MW, 0.0))
			{
				_obsinfo_data[_crt_rec][sat][obs_L1].mwslip[obs_L2] = true;
				if (_log)
				{
					ostringstream msg;
					msg << fixed << setw(10) << " MW cycle slip : " << _crt_time.str_hms() << "   " << oneObs->sat() << "   " << gobs2str(obs_L1)
						<< "   " << gobs2str(obs_L2) << setw(16) << setprecision(3) << pre_MW_val << setw(16) << setprecision(3) << crt_MW_val
						<< setw(16) << setprecision(3) << (crt_MW_val - pre_MW_val) << setw(16) << setprecision(3) << thres_MW;
					_log->comment(1, _crt_rec, msg.str());
				}
			}
		}

		// mark ambiguity cycle slip flag
		if (_obsinfo_data[_crt_rec][sat][obs_L1].gfslip[obs_L2] || _obsinfo_data[_crt_rec][sat][obs_L1].mwslip[obs_L2])
		{
			_obsinfo_data[_crt_rec][sat][obs_L1].epo_num[obs_L2] = 1;
			_obsinfo_data[_crt_rec][sat][obs_L1].mwslip[obs_L2] = false;
			_obsinfo_data[_crt_rec][sat][obs_L1].gfslip[obs_L2] = false;
			_obsinfo_data[_crt_rec][sat][obs_L1].last_time[obs_L2] = _crt_time;
			_obsinfo_data[_crt_rec][sat][obs_L1].GF[obs_L2] = crt_GF_val;
			_obsinfo_data[_crt_rec][sat][obs_L1].statistic_MW[obs_L2] = make_pair(crt_MW_val, 0.0);

			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].clear();
			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].push_back(0.0);
			_obsinfo_data[_crt_rec][sat][obs_L1].origin_MWs[obs_L2].push_back(crt_MW_val);

			oneObs->addlli(obs_L1, 1);
			oneObs->addlli(obs_L2, 1);
		}
	}


	bool t_gpreeditproc::_getSatInfo(string sat, double& elev, t_spt_gobs oneobs)
	{
		bool valid = false;
		double sat_crd[3];
		t_gtriple sat_crd_triple, site_sat_vector;

		double delay = 0.0,distance;
		t_gtime sat_time;
		shared_ptr<t_geph> geph;
		while (true)
		{
			sat_time = _crt_time - delay;

			if (_allnav->pos(sat, sat_time, sat_crd, nullptr, nullptr, false) < 0) break;
			sat_crd_triple = t_gtriple(sat_crd);

			//  rec crd don't exists
			if (_rec_crds[_crt_rec].zero()) break;

			site_sat_vector = sat_crd_triple - _rec_crds[_crt_rec];
			distance = site_sat_vector.norm();

			if (abs(distance / CLIGHT - delay) < 1E-9)
			{
				valid = true;
				oneobs->addcrd(sat_crd_triple);
				break;
			}

			delay = distance / CLIGHT;
		}

		// Compute elevation & sat clks
		if (valid)
		{
				elev = (_rec_crds[_crt_rec][0] * site_sat_vector[0] + _rec_crds[_crt_rec][1] * site_sat_vector[1]
					+ _rec_crds[_crt_rec][2] * site_sat_vector[2]) / _rec_crds[_crt_rec].norm() / distance;
				elev = 90.0 - acos(elev) * 180.0 / G_PI;

				return true;
		}

		elev = 0.0;
		_log->comment(2, _crt_rec, sat + " lack of suitable broadcast ephemeris at Time " + _crt_time.str_ymdhms());
		return false;
	}

	void t_gpreeditproc::_writeAmbFlagData()
	{
		// Site Loop
		for (auto site : _rec_list) {
			_crt_time = _beg_time;
			_crt_rec = site;
			t_gambflag site_ambflag_data12,site_ambflag_data13;
			ambflag_hd ambflag_hd12, ambflag_hd13;
			for (auto item_hd : vector<ambflag_hd*>({ &ambflag_hd12,&ambflag_hd13 })) {
				item_hd->beg_time = _beg_time;
				item_hd->end_time = _end_time;
				item_hd->interval = _interval;
				item_hd->site_name = site;
			}
			for (auto& band_item : _band_index) {
				ambflag_hd12.proc_band[band_item.first].insert(gobsband2str(band_item.second[FREQ_1]));
				ambflag_hd12.proc_band[band_item.first].insert(gobsband2str(band_item.second[FREQ_2]));

				ambflag_hd13.proc_band[band_item.first].insert(gobsband2str(band_item.second[FREQ_1]));
				ambflag_hd13.proc_band[band_item.first].insert(gobsband2str(band_item.second[FREQ_3]));
			}
			int iepo = 0;
			// Epoch Loop
			while (_crt_time <= _end_time) {
				iepo++;
				vector<shared_ptr<t_gobsgnss> > epoData = _allobs->obs_pt(_crt_rec, _crt_time);
				// Satellite Loop
				for (auto& satObs : epoData) {
					string sat = satObs->sat();
					GOBSBAND b1 = _band_index[satObs->gsys()][FREQ_1];
					GOBSBAND b2 = _band_index[satObs->gsys()][FREQ_2];
					GOBSBAND b3 = _band_index[satObs->gsys()][FREQ_3];

					vector<t_vec_ambflag*> vec_ambflag_data;
					vec_ambflag_data.push_back(&site_ambflag_data12.getAmbFlagData()[sat]);
					vec_ambflag_data.push_back(&site_ambflag_data13.getAmbFlagData()[sat]);

					vector<GOBS> vec_gobs;
					vec_gobs.push_back(satObs->select_phase(b1));
					vec_gobs.push_back(satObs->select_phase(b2));
					vec_gobs.push_back(satObs->select_phase(b3));

					for (int band_i = 1; band_i < 3; band_i++) {
						if (vec_gobs[0] != GOBS::X && vec_gobs[band_i] != GOBS::X) {
							// check cycle slip
							if (satObs->getlli(vec_gobs[0]) == 1
								|| satObs->getlli(vec_gobs[band_i]) == 1) {
								if (_check_short && !vec_ambflag_data[band_i - 1]->empty()) {
									auto last_data = (*(vec_ambflag_data[band_i - 1]->rbegin()));
									if (last_data->end_epo - last_data->beg_epo < _short_limit) {
										last_data->identify = "DEL";
										last_data->iflag = "3";
									}
								}
								shared_ptr<ambflag_data> new_arc_record = make_shared<ambflag_data>();
								new_arc_record->identify = "AMB";
								new_arc_record->iflag = "1";
								new_arc_record->reason = "RN_biggap";
								new_arc_record->beg_epo = iepo;
								new_arc_record->end_epo = iepo;

								vec_ambflag_data[band_i - 1]->push_back(new_arc_record);
							}
							else {
								// exist obs data and no cycle slip
								if (!vec_ambflag_data[band_i - 1]->empty()) {
									auto last_data = (*(vec_ambflag_data[band_i - 1]->rbegin()));
									last_data->end_epo = iepo;
								}
							}
						}
					}
				}
				_crt_time = _crt_time + _interval;
			}

			// check end epo
			if (!site_ambflag_data12.getAmbFlagData().empty()) {
				for (auto& item : site_ambflag_data12.getAmbFlagData()) {
					if (!item.second.empty() && item.second[item.second.size() - 1]->end_epo - item.second[item.second.size() - 1]->beg_epo < 10) {
						item.second[item.second.size() - 1]->identify = "DEL";
						item.second[item.second.size() - 1]->iflag = "3";
					}
				}
				_allambflag->addAmbFlag(site, site_ambflag_data12);
				_allambflag->addAmbFlagHead(site, ambflag_hd12);
			}
			if (!site_ambflag_data13.getAmbFlagData().empty()) {
				for (auto& item : site_ambflag_data13.getAmbFlagData()) {
					if (!item.second.empty() && item.second[item.second.size() - 1]->end_epo - item.second[item.second.size() - 1]->beg_epo < 10) {
						item.second[item.second.size() - 1]->identify = "DEL";
						item.second[item.second.size() - 1]->iflag = "3";
					}
				}
				_allambflag13->addAmbFlag(site, site_ambflag_data13);
				_allambflag13->addAmbFlagHead(site, ambflag_hd13);
			}
		}


		return;
		for (auto& site : _rec_list)
		{
			t_giof ambflag_file(site + "_rt.ambflag");
			ostringstream os;

			if (_allambflag->getSiteList().count(site)) {
				os << _allambflag->getOneAmbFlag(site).getAmbFlagHead()->beg_time.str() << endl;
				os << _allambflag->getOneAmbFlag(site).getAmbFlagHead()->end_time.str() << endl;
				os << _allambflag->getOneAmbFlag(site).getAmbFlagHead()->interval << endl;
				os << _allambflag->getOneAmbFlag(site).getAmbFlagHead()->site_name << endl;
				for (auto& item : _allambflag->getOneAmbFlag(site).getAmbFlagHead()->proc_band) {
					os << t_gsys::gsys2str(item.first);
					for (auto& sys_item : item.second) {
						os << setw(7) << sys_item;
					}
					os << endl;
				}
				for (auto& sat : _allambflag->getSatList()) {
					for (auto& item : _allambflag->getOneAmbFlag(site).getAmbFlagData()[sat]) {
						os << item->identify << setw(8) << sat << setw(10) << item->beg_epo << setw(10) << item->end_epo << endl;
					}
				}
			}
		
			if (_allambflag13->getSiteList().count(site)) {
				os << _allambflag13->getOneAmbFlag(site).getAmbFlagHead()->beg_time.str() << endl;
				os << _allambflag13->getOneAmbFlag(site).getAmbFlagHead()->end_time.str() << endl;
				os << _allambflag13->getOneAmbFlag(site).getAmbFlagHead()->interval << endl;
				os << _allambflag13->getOneAmbFlag(site).getAmbFlagHead()->site_name << endl;
				for (auto& item : _allambflag13->getOneAmbFlag(site).getAmbFlagHead()->proc_band) {
					os << t_gsys::gsys2str(item.first);
					for (auto& sys_item : item.second) {
						os << setw(7) << sys_item;
					}
					os << endl;
				}
				for (auto& sat : _allambflag13->getSatList()) {
					for (auto& item : _allambflag13->getOneAmbFlag(site).getAmbFlagData()[sat]) {
						os << item->identify << setw(8) << sat << setw(10) << item->beg_epo << setw(10) << item->end_epo << endl;
					}
				}
			}
			ambflag_file.write(os.str());
		}
	}
}


