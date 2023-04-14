/**
*
* @verbatim
History
-1.0 glfeng    2020-10-06 creat the file.
-1.1 hjzheng   2020-10-18 add gsys in band_index
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gcorbds.cpp
* @brief	   correct code bias on bds system
* @author      glfeng, Wuhan University
* @version	   1.1.0
* @date		   2020-10-18
*
*/

#include <iomanip>
#include <memory>

#include "gproc/gcorbds.h"
#include "gmodels/gbancroft.h"

namespace great {

	t_gbds_codebias_cor::t_gbds_codebias_cor(t_gsetbase* settings)
	{
		_correct_bds_code_bias = true;
		set<string> sys_list = dynamic_cast<t_gsetgen*>(settings)->sys();
		if (sys_list.find("BDS") == sys_list.end())
		{
			_correct_bds_code_bias = false;
		}

		_band_index[gnut::BDS] = dynamic_cast<t_gsetgnss*>(settings)->band_index(gnut::BDS);
		_band_index[gnut::GPS] = dynamic_cast<t_gsetgnss*>(settings)->band_index(gnut::GPS);
		_band_index[gnut::GAL] = dynamic_cast<t_gsetgnss*>(settings)->band_index(gnut::GAL);
		_band_index[gnut::GLO] = dynamic_cast<t_gsetgnss*>(settings)->band_index(gnut::GLO);
		_band_index[gnut::QZS] = dynamic_cast<t_gsetgnss*>(settings)->band_index(gnut::QZS);
		_set = settings;

		// set Wanninger & Beer 
		_IGSO_MEO_Corr[BAND_2]["IGSO"] = { {0,-0.55}, {1,-0.40}, {2,-0.34}, {3,-0.23}, {4,-0.15}, {5,-0.04}, {6,0.09}, {7,0.19}, {8,0.27}, {9,0.35} }; // B1
		_IGSO_MEO_Corr[BAND_7]["IGSO"] = { {0,-0.71}, {1,-0.36}, {2,-0.33}, {3,-0.19}, {4,-0.14}, {5,-0.03}, {6,0.08}, {7,0.17}, {8,0.24}, {9,0.33} }; // B2
		_IGSO_MEO_Corr[BAND_6]["IGSO"] = { {0,-0.27}, {1,-0.23}, {2,-0.21}, {3,-0.15}, {4,-0.11}, {5,-0.04}, {6,0.05}, {7,0.14}, {8,0.19}, {9,0.32} }; // B3

		_IGSO_MEO_Corr[BAND_2]["MEO"]  = { {0,-0.47}, {1,-0.38}, {2,-0.32}, {3,-0.23}, {4,-0.11}, {5,0.06}, {6,0.34}, {7,0.69}, {8,0.97}, {9,1.05} }; //B1
		_IGSO_MEO_Corr[BAND_7]["MEO"]  = { {0,-0.40}, {1,-0.31}, {2,-0.26}, {3,-0.18}, {4,-0.06}, {5,0.09}, {6,0.28}, {7,0.48}, {8,0.64}, {9,0.69} }; //B2
		_IGSO_MEO_Corr[BAND_6]["MEO"]  = { {0,-0.22}, {1,-0.15}, {2,-0.13}, {3,-0.10}, {4,-0.04}, {5,0.05}, {6,0.14}, {7,0.27}, {8,0.36}, {9,0.47} }; //B3
	}

	void t_gbds_codebias_cor::apply_IGSO_MEO(const t_gtime& now,const string& rec, t_gtriple& rec_crd, t_gallnav* gnav, vector<t_gobsgnss>& obsdata)
	{
		if (!this->_correct_bds_code_bias || obsdata.size() == 0) return;

		if (rec_crd.zero())
		{
			if (!this->_recAprCoordinate(now ,rec, rec_crd, gnav, obsdata)) return;
		}

		// Calculate elevation and Correct Obs
		map<GOBSBAND, double> Band_cor;
		string sat_type;

		vector<t_gobsgnss>::iterator data;
		for (data = obsdata.begin(); data != obsdata.end();)
		{
			if (data->getrangestate("bds_code_bias"))    
			{ 
				data++; 
				continue;
			}
			if (data->site() != rec || data->gsys() != BDS)  
			{
				data++;
				continue;
			} 
			string sat = data->sat();
			if (sat <= "C05" || sat > "C16") // BDS2 - IGSO/MEO
			{
				data++;
				continue;
			}
			if (sat == "C11" || sat == "C12" || sat == "C14") sat_type = "MEO";
			else                                              sat_type = "IGSO";


			t_gtriple sat_crd = data->satcrd();
			t_gtriple rec_sat_vector = sat_crd - rec_crd;
			double distance = rec_sat_vector.norm();
			double elev = (rec_crd[0] * rec_sat_vector[0] + rec_crd[1] * rec_sat_vector[1] + rec_crd[2] * rec_sat_vector[2]) / rec_crd.norm() / distance;
			elev = 90.0 - acos(elev) * 180.0 / G_PI;

			// get correction
			double elev0 = elev / 10.0;
			int elev0_int = floor(elev0);
			if (elev0_int < 0)
			{
				Band_cor[BAND_2] = _IGSO_MEO_Corr[BAND_2][sat_type][0];
				Band_cor[BAND_7] = _IGSO_MEO_Corr[BAND_7][sat_type][0];
				Band_cor[BAND_6] = _IGSO_MEO_Corr[BAND_6][sat_type][0];
			}
			else if (elev0_int >= 9)
			{
				Band_cor[BAND_2] = _IGSO_MEO_Corr[BAND_2][sat_type][9];
				Band_cor[BAND_7] = _IGSO_MEO_Corr[BAND_7][sat_type][9];
				Band_cor[BAND_6] = _IGSO_MEO_Corr[BAND_6][sat_type][9];
			}
			else
			{
				Band_cor[BAND_2] = _IGSO_MEO_Corr[BAND_2][sat_type][elev0_int] * (1.0 - elev0 + elev0_int)
					+ _IGSO_MEO_Corr[BAND_2][sat_type][elev0_int + 1] * (elev0 - elev0_int);
				Band_cor[BAND_7] = _IGSO_MEO_Corr[BAND_7][sat_type][elev0_int] * (1.0 - elev0 + elev0_int)
					+ _IGSO_MEO_Corr[BAND_7][sat_type][elev0_int + 1] * (elev0 - elev0_int);
				Band_cor[BAND_6] = _IGSO_MEO_Corr[BAND_6][sat_type][elev0_int] * (1.0 - elev0 + elev0_int)
					+ _IGSO_MEO_Corr[BAND_6][sat_type][elev0_int + 1] * (elev0 - elev0_int);
			}

			vector<GOBS> obs_vec = data->obs();
			for (auto obs_type : obs_vec)
			{
				// skip not code obs
				if (!t_gobs(obs_type).is_code())
				{
					continue;
				}

				GOBSBAND b = t_gobs(obs_type).band();
				double obs_P = data->obs_C(obs_type);

				obs_P += Band_cor[b];
				data->resetobs(obs_type, obs_P);
				data->setrangestate("bds_code_bias", true);
			}

			data++;
		}
		return;
	}

	bool t_gbds_codebias_cor::_recAprCoordinate(const t_gtime& now,const string& rec, t_gtriple& rec_crd, t_gallnav* gnav, vector<t_gobsgnss>& obsdata)
	{
		Matrix BB;

		BB.ReSize(obsdata.size(), 4);
		BB = 0.0;
		int iobs = 0;

		vector<t_gobsgnss>::iterator iter;
		for (iter = obsdata.begin(); iter != obsdata.end();)
		{
			if (iter->site() != rec)
			{
				iter++;
				continue;
			}

			GSYS gs = iter->gsys();

			GOBSBAND b1 = _band_index[gs][FREQ_1];
			GOBSBAND b2 = _band_index[gs][FREQ_2];

			GOBS l1 = iter->select_phase(b1);
			GOBS l2 = iter->select_phase(b2);
			GOBS p1 = iter->select_range(b1);
			GOBS p2 = iter->select_range(b2);

			if (p1 == X || l1 == X || p2 == X || l2 == X)
			{
				iter++;
				continue;
			}

			double P3 = iter->P3(p1, p2);
			double L3 = iter->L3(l1, l2);

			if (double_eq(L3, 0.0) || double_eq(P3, 0.0))
			{
				iter++;
				continue;
			}

			
			t_gtriple sat_pos;
			double sat_clk;
			if (!this->_satPosClk(now, iter->sat(), gnav, P3, sat_pos, sat_clk))
			{
				iter = obsdata.erase(iter);
				continue;
			}
			iter->addcrd(sat_pos);

			iobs++;
			BB(iobs, 1) = iter->satcrd().crd(0);
			BB(iobs, 2) = iter->satcrd().crd(1);
			BB(iobs, 3) = iter->satcrd().crd(2);
			BB(iobs, 4) = P3 + sat_clk;

			iter++;
		}

		if (iobs < 4) return false;

		BB = BB.Rows(1, iobs);    // delete zero rows

		ColumnVector vBanc;

		gbancroft(BB, vBanc);

		rec_crd[0] = vBanc(1);
		rec_crd[1] = vBanc(2);
		rec_crd[2] = vBanc(3);

		return true;
	}

	bool t_gbds_codebias_cor::_satPosClk(const t_gtime& now, string sat, t_gallnav* gnav, double P3, t_gtriple& sat_pos, double& sat_clk)
	{
		sat_pos = t_gtriple(0.0, 0.0, 0.0);
		sat_clk = 0.0;
		if (!gnav) return false;

		t_gtime sat_time;

		double clk;
		double satclk = 0.0;
		double satclk2 = 1.0;
		int count = 0, irc = -1;
		while (fabs(satclk - satclk2) > 1.e-3 / CLIGHT)
		{
			count++;
			satclk2 = satclk;
			sat_time = now - P3 / CLIGHT - satclk;
			irc = gnav->clk(sat, sat_time, &clk);
			if (irc < 0 || count > 25) return false;
			satclk = clk;
		}

		double xyz[3] = { 0.0, 0.0, 0.0 };
		double vel[3] = { 0.0, 0.0, 0.0 };
		double var[3] = { 0.0, 0.0, 0.0 };
		irc = gnav->pos(sat, sat_time, xyz, var, vel);
		if (irc < 0) return false;

		t_gtriple txyz(xyz);

		// relativistic correction
		double rel = 2.0 * (txyz[0] * vel[0] + txyz[1] * vel[1] + txyz[2] * vel[2]) / CLIGHT / CLIGHT; //default
		satclk -= rel;

		sat_pos = txyz;
		sat_clk = satclk * CLIGHT;

		return true;
	}

} // namespace
