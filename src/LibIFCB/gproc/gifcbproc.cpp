/**
*
* @verbatim
History
-1.0 hjzheng  2020-10-06  creat the file.
-1.1 hjzheng  2020-10-18  Adding Doxygen Style Code Remarks.
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gifcbproc.cpp
* @brief	compute Inter-frequency Clock Bias(IFCB)
* @author   hjzheng, Wuhan University
* @version	1.0.0
* @date		2020-10-18
*
*/

#include <algorithm>
#include "gall/gallproc.h"
#include "gproc/gifcbproc.h"
#include "gproc/gpreeditproc.h"
#include "gio/gfile.h"
#include "gcoders/rinexo.h"
#include "gcoders/ifcb.h"
#include "gutils/gstring.h"

namespace great
{
	t_gifcbproc::t_gifcbproc(t_gsetbase* set, t_gallproc* data, t_glog* log) :
		_bds_codebias_cor(set)
	{
		// set settings && log file
		_set = set;
		_glog = log;
		_data = data;


		_band_index[gnut::GPS] = dynamic_cast<t_gsetgnss*>(set)->band_index(gnut::GPS);
		_band_index[gnut::GAL] = dynamic_cast<t_gsetgnss*>(set)->band_index(gnut::GAL);
		_band_index[gnut::GLO] = dynamic_cast<t_gsetgnss*>(set)->band_index(gnut::GLO);
		_band_index[gnut::BDS] = dynamic_cast<t_gsetgnss*>(set)->band_index(gnut::BDS);
		_band_index[gnut::QZS] = dynamic_cast<t_gsetgnss*>(set)->band_index(gnut::QZS);

		_ifcb_data = make_shared<t_gifcb>();

	}


	t_gifcbproc::~t_gifcbproc()
	{
	}

	bool t_gifcbproc::GenerateProduct()
	{
		// Encoder IFCB files
		string path = dynamic_cast<t_gsetout*>(_set)->outputs("ifcb");

		t_gcoder* gencode = new t_ifcb(_set, "", 4096);
		gencode->clear();
		gencode->glog(_glog);
		gencode->path(path);
		gencode->add_data("IDX", _ifcb_data.get());

		t_gfile* gout = new t_gfile;
		gout->glog(_glog);
		gout->path(path);
		gout->coder(gencode);
		write_log_info(_glog, 1, " t_gifcbproc::GenerateProduct :  Writing IFCB file " + path);
		gout->run_write();
		delete gout; gout = NULL;
		delete gencode; gencode = NULL;

		return true;
	}

	bool t_gifcbproc::_initProcData(t_gallproc * data)
	{
		_sys = dynamic_cast<t_gsetgen*>(_set)->sys();


		_allobs = dynamic_cast<t_gallobs*>((*data)[t_gdata::ALLOBS]);
		_allbias = dynamic_cast<t_gallbias*>((*data)[t_gdata::ALLBIAS]);
		_allnav = dynamic_cast<t_gallnav*>((*data)[t_gdata::GRP_EPHEM]);

		_allambflag = make_shared<t_gallambflag>(t_gdata::AMBFLAG);
		_allambflag13 = make_shared<t_gallambflag>(t_gdata::AMBFLAG13);
		if (!_allobs || !_allbias || !_allambflag || !_allambflag13 || (!_allnav && _sys.find("BDS")!=_sys.end()))
		{
			write_log_info(_glog, 1, " ERROR[t_gifcbproc::_initProcData] : can not init IFCB process data.");
			return false;
		}

		write_log_info(_glog, 1, " INIT [t_gifcbproc::_initProcData] : process data in IFCB Estimation is prepared.");
		return true;
	}

	bool t_gifcbproc::_initSatSite()
	{

		set<string> sat_rm = dynamic_cast<t_gsetgen*>(_set)->sat_rm();

		// set sats1 according to log/log13
		set<string> sats1, sats2, sats3, tmp1, tmp2;
		tmp1 = _allambflag->getSatList();
		tmp2 = _allambflag13->getSatList();
		set_intersection(tmp1.begin(), tmp1.end(), tmp2.begin(), tmp2.end(), inserter(sats1, sats1.begin()));
		auto allsats = GNSS_SATS();

		// set sats according to system list
		set<string> syslist = dynamic_cast<t_gsetgen*>(_set)->sys();
		for (auto& item : syslist) {
			auto sys_item = t_gsys::str2gsys(item);
			sats2.insert(allsats[sys_item].begin(),allsats[sys_item].end());
		}

		// sat3 = sat1 + sat2
		set_intersection(sats1.begin(), sats1.end(), sats2.begin(), sats2.end(), inserter(sats3, sats3.begin()));
		// _sats = sat3 - sat_rm
		set_difference(sats3.begin(), sats3.end(), sat_rm.begin(), sat_rm.end(), inserter(_sats, _sats.begin()));

		// get site list _sites = sites1 + sites2 + sites4
		set<string> sites1, sites2, sites3, sites4;
		sites1 = dynamic_cast<t_gsetgen*>(_set)->recs();
		sites2 = _allambflag->getSiteList();
		sites4 = _allobs->getSiteList();
		set_intersection(sites1.begin(), sites1.end(), sites2.begin(), sites2.end(), inserter(sites3, sites3.begin()));
		set_intersection(sites3.begin(), sites3.end(), sites4.begin(), sites4.end(), inserter(_sites, _sites.begin()));

		return true;
	}

	bool t_gifcbproc::_initAmbiguityCycleSlip(const t_gtime& beg, const t_gtime& end, const double& intv)
	{
		t_gallproc cycleslip_data;
		cycleslip_data.Add_Data(_allobs);
		cycleslip_data.Add_Data(_allnav);
		cycleslip_data.Add_Data(_allambflag.get());
		cycleslip_data.Add_Data(_allambflag13.get());
		// init exeturobedit obsdata ambflagdata
		t_gpreeditproc check_cycleslip(_set, _glog, &cycleslip_data);
		return check_cycleslip.ProcessBatch(beg, end, intv);
	}


	bool t_gifcbproc::processBatch(const t_gtime& beg, const t_gtime& end, const double &intv)
	{
		/** Prepare data */
		if (!_initProcData(_data)) {
			write_log_info(_glog, 0, " ERROR[t_gifcbproc::processBatch]: Init Proc data fail!");
			return false;
		}
		write_log_info(_glog, 0, " INFO[t_gifcbproc::processBatch]: Init Proc data finish!");

		/** Ambiguity Cycle Slip */
		if (!_initAmbiguityCycleSlip(beg, end, intv)) {
			write_log_info(_glog, 0, " ERROR[t_gifcbproc::processBatch]: Init ambiguity cycle slip fail!");
			return false;
		}
		write_log_info(_glog, 0, " INFO[t_gifcbproc::processBatch]: Init ambiguity cycle slip finish!");

		/** Init sat/site info */
		if (!_initSatSite()) {
			write_log_info(_glog, 0, " ERROR[t_gifcbproc::processBatch]: Init sat/site info fail!");
			return false;
		}
		write_log_info(_glog, 0, " INFO[t_gifcbproc::processBatch]: Init sat/site info finish!");
		
		/** Main Process */
		if (!_processIFCB(beg, end, intv)) return false;

		return true;
	}

	bool t_gifcbproc::_processIFCB(const t_gtime& beg, const t_gtime& end, const double &intv)
	{

		_caculateEpoDifIFCB(beg, end, intv);
		write_log_info(_glog, 0, " INFO[t_gifcbproc::_processIFCB]: caculate ED IFCB of each site finish!");

		_cacualteAverageEpoDifIFCB(beg, end, intv);
		write_log_info(_glog, 0, " INFO[t_gifcbproc::_processIFCB]: caculate final ED IFCB finish!");
		

		_addPriConstraint(beg, end, intv);
		write_log_info(_glog, 0, " INFO[t_gifcbproc::_processIFCB]: add priority constraint finish!");

		_recoverAbsoluteIFCB(beg, end, intv);
		write_log_info(_glog, 0, " INFO[t_gifcbproc::_processIFCB]: recover absolute IFCB finish!");

		return true;
	}

	bool t_gifcbproc::_combineObsGFIF(string site, const t_gtime& T_beg, const t_gtime& T_end, double intv,
		t_epo_sat_amb& one_site_GFIF, t_all_sats_OW& OW)
	{

		int num_epo = round((T_end - T_beg) / intv);
		string site_lower = str2lower(site);
		t_gambflag one_site_flag = _allambflag->getOneAmbFlag(site_lower);
		t_gambflag one_site_flag13 = _allambflag13->getOneAmbFlag(site_lower);

		t_gtime it_time = T_beg;
		// each epoch
		for (int iepo = 1; iepo <= num_epo; iepo++)
		{
			vector<t_gobsgnss> obsdata = _allobs->obs(site, it_time);

			if (_sys.count("BDS"))
			{
				t_gtriple BDS_tmp(0.0, 0.0, 0.0);
				_bds_codebias_cor.apply_IGSO_MEO(it_time,site, BDS_tmp, _allnav, obsdata);
				if (obsdata.size() > 0 && BDS_tmp.zero())
				{
					BDS_tmp = _allobs->getsitecrd(site);
					_bds_codebias_cor.apply_IGSO_MEO(it_time,site, BDS_tmp, _allnav, obsdata);
				}
			}

			for (auto it_satdata = obsdata.begin(); it_satdata != obsdata.end(); it_satdata++)
			{
				it_satdata->apply_bias(*_allbias);
				
				// skip the sat
				if (_sats.count(it_satdata->sat()) == 0) {
					continue;
				}
				string sat = it_satdata->sat();
				one_site_GFIF[iepo][sat] = 0.0;

				int AMB_pos;
				int AMB_pos13;

				if (!one_site_flag.isValid(sat, it_time, AMB_pos) ||
					!one_site_flag13.isValid(sat, it_time, AMB_pos13)) continue;
				t_vec_ambflag one_sat_flag = one_site_flag.getAmbFlagData()[sat];
				t_vec_ambflag one_sat_flag13 = one_site_flag13.getAmbFlagData()[sat];

				// iflag = 0 means already used
				if (one_sat_flag[AMB_pos]->iflag != "0" || one_sat_flag13[AMB_pos13]->iflag != "0")
				{
					shared_ptr<t_oneway_ambiguity> temp_single_ow = make_shared<t_oneway_ambiguity>();
					(*temp_single_ow).beg_epo = iepo;
					(*temp_single_ow).sat = sat;
					OW[sat].push_back(temp_single_ow);

					// set iflag = 0 for marking used
					one_site_flag.reset_iflag(sat, "0", AMB_pos);
					one_site_flag13.reset_iflag(sat, "0", AMB_pos13);
				}
				if (OW[sat].size() != 0) OW[sat].back()->end_epo = iepo;

				GOBSBAND b1 = _band_index[it_satdata->gsys()][FREQ_1];
				GOBSBAND b2 = _band_index[it_satdata->gsys()][FREQ_2];
				GOBSBAND b3 = _band_index[it_satdata->gsys()][FREQ_3];

				t_gobs gobs1(it_satdata->select_phase(b1));
				t_gobs gobs2(it_satdata->select_phase(b2));
				t_gobs gobs3(it_satdata->select_phase(b3));

				double GFIF_obs = it_satdata->GFIF_meter(gobs1, gobs2, gobs3);

				if (double_eq(GFIF_obs, 0.0)) continue;

				one_site_GFIF[iepo][sat] = GFIF_obs;
			}
			it_time = it_time + intv;
		}
		return true;
	}

	void t_gifcbproc::_caculateEpoDifIFCB(const t_gtime & T_beg, const t_gtime & T_end, double intv)
	{
		//Caculate ED IFCBS of each station
		for (auto it_site = _sites.begin(); it_site != _sites.end(); it_site++)
		{
			t_all_sats_OW OW; // record all useful amb arcs of each sat
			t_epo_sat_amb one_site_GFIF; // record GFIF value of each epo each sat

			if (!_combineObsGFIF(*it_site, T_beg, T_end, intv, one_site_GFIF, OW)) {
				write_log_info(_glog, 1, " ERROR[t_gifcbproc::_caculateEpoDifIFCB]: Skip site " + *it_site + " for combining GFIF Error!");
				continue;
			}

			auto itsat = OW.begin();
			//  each sat
			for (itsat; itsat != OW.end(); itsat++)
			{
				auto itarc = itsat->second.begin();
				// each amb arc
				for (itarc; itarc != itsat->second.end(); itarc++)
				{
					int iepo = (*itarc)->beg_epo;
					_all_site_GFIF[*it_site][iepo][itsat->first] = 0.0;
					// ED_GFIF[iepo+1] = GFIF[iepo+1]-GFIF[iepo]
					for (; iepo < (*itarc)->end_epo; iepo++)
					{
						// Bad epochs
						if (double_eq(one_site_GFIF[iepo][itsat->first], 0.0) || double_eq(one_site_GFIF[iepo + 1][itsat->first], 0.0))
							_all_site_GFIF[*it_site][iepo + 1][itsat->first] = 0.0;
						else
							_all_site_GFIF[*it_site][iepo + 1][itsat->first] = one_site_GFIF[iepo + 1][itsat->first] - one_site_GFIF[iepo][itsat->first];
					}
				}
			}
			write_log_info(_glog, 1, " INFO[t_gifcbproc::_caculateEpoDifIFCB]: " + *it_site + " combine ED GFIF Finish!");
		}

	}

	void t_gifcbproc::_cacualteAverageEpoDifIFCB(const t_gtime & T_beg, const t_gtime & T_end, double intv)
	{
		_ifcb_data->set_valid_beg(T_beg);

		// epoch cycle caculate
		int iepo = 0;
		t_gtime epoch = T_beg;
		while (epoch <= T_end)
		{
			iepo = iepo + 1;


			// caculate final ED IFCBs of each sat
			for (auto it_sat = _sats.begin(); it_sat != _sats.end(); it_sat++)
			{
				_ifcb_data->re_init_ifcb(epoch, *it_sat);

				list<pair<double, double>> wx;
				double mean, sigma, mean_sig;
				for (auto it_site = _sites.begin(); it_site != _sites.end(); it_site++)
				{
					double  GFIF_value = _all_site_GFIF[*it_site][iepo][*it_sat];
					if (double_eq(GFIF_value, 0.0)) continue;
					wx.push_back(make_pair(GFIF_value - round(GFIF_value), 1.0));
				}
				// iteratively solve
				for (int i = 0; i < min(10.0, (double)(wx.size()) / 2.0); i++)
				{
					int wx_size = wx.size();
					getMeanWgt(true, wx, mean, sigma, mean_sig);
					if (wx_size == wx.size() || mean_sig == 0.0) break;
				}
				if (wx.size() > 0)
				{
					_ifcb_data->reset_ifcb(epoch, *it_sat, mean, sigma, wx.size());
				}
				else
				{
					_ifcb_data->re_init_ifcb(epoch, *it_sat);
				}
			}

			epoch = epoch + intv;
		}
	}

	void t_gifcbproc::_addPriConstraint(const t_gtime& T_beg, const t_gtime& T_end, double intv)
	{
		for (auto itsat = _sats.begin(); itsat != _sats.end(); itsat++)
		{
			int nepo = (T_end-T_beg) / intv + 1;
			//double ifcb_init  = 0;
			//t_gtime epoch = T_beg;
			//for (int iepo = 1; iepo < nepo; iepo++)
			//{
			//	epoch = epoch + intv;
			//	double value = _ifcb_data->get_ifcb_value(epoch, *itsat);
			//	ifcb_init += (nepo - iepo)*value;
			//}
			t_gtime epoch = T_end;
			double ifcb_init = 0;
			int epo_arc_end = nepo;
			bool arc_valid = false;
			for (int iepo = nepo; iepo >=1; iepo--) {
				if (_ifcb_data->get_ifcb_value_valid(epoch,*itsat)) {
					if (!arc_valid) {
						arc_valid = true;
						ifcb_init = 0;
						epo_arc_end = iepo;
					}
					double value = _ifcb_data->get_ifcb_value(epoch, *itsat);
					ifcb_init += (epo_arc_end - iepo + 1) * value;
				}
				else if (arc_valid) {
					arc_valid = false;
					_ifcb_data->reset_ifcb(epoch, *itsat, -ifcb_init / (epo_arc_end - iepo + 1), 0.0, 999);
				}
				epoch = epoch - intv;
			}
			//_ifcb_data->reset_ifcb(T_beg, *itsat, -ifcb_init / nepo, 0.0, 999);
		}
	}

	void t_gifcbproc::_recoverAbsoluteIFCB(const t_gtime& T_beg, const t_gtime& T_end, double intv)
	{
		t_gtime epoch = T_beg;
		while (epoch < T_end)
		{
			epoch.from_mjd(epoch.mjd(), epoch.sod() + intv, epoch.dsec());
			auto itsat = _sats.begin();
			double value1, value2;
			for (itsat; itsat != _sats.end(); itsat++)
			{
				if (_ifcb_data->get_ifcb_value_valid(epoch, *itsat) && _ifcb_data->get_ifcb_sigma(epoch, *itsat) != 0.0) {
					value1 = _ifcb_data->get_ifcb_value(epoch - intv, *itsat);
					value2 = _ifcb_data->get_ifcb_value(epoch, *itsat);
					_ifcb_data->reset_ifcb_value(epoch, *itsat, value1 + value2);
				}
			}
		}
	}
}
