/**
* @verbatim
History
-1.0 hjzheng	 2022-07-02  creat the file.
@endverbatim
* Copyright (c) 2020, Wuhan University. All rights reserved.
*
* @file			goutlierproc.cpp
* @brief		detecting range outliers
* @author       hjzheng, Wuhan University
* @version		1.0.0
* @date			2022-07-02
*
*/

#include <iomanip>
#include <memory>

#include "gproc/goutlierproc.h"

namespace great {

	t_goutliers_process::t_goutliers_process(t_glog* glog):
		_log(glog)
	{
	}

	t_goutliers_process::~t_goutliers_process()
	{
	}

	void t_goutliers_process::flagRangeOutliers(shared_ptr<t_gobsgnss> ObsPre, shared_ptr<t_gobsgnss> Obs, double sampling)
	{
		ostringstream os;
		os.str("");
		t_gtime crt_time = Obs->epoch();
		string  crt_site = Obs->site();

		vector<GOBS> obs_vec = Obs->obs();

		// Gao Y et al. Modeling and estimation of C1-P1 bias in GPS receivers[J]. Journal of Geodesy, 2001.
		GSYS gsys = Obs->gsys();
		GOBSBAND b1 = GNSS_BAND_PRIORITY.at(gsys)[1];
		GOBS ref_obsP = Obs->select_range(b1);
		double ref_P = Obs->obs_C(ref_obsP);  // m

		for (auto obs_type : obs_vec)
		{
			// skip not code obs
			if (!t_gobs(obs_type).is_code()) continue;
			// skip ref obs(add case: ref_obsP == X)
			if (obs_type == ref_obsP || ref_obsP == X) continue;

			t_gobs gobs(obs_type);
			GOBSBAND bX = gobs.band();

			double obs_P = Obs->obs_C(obs_type);
			double diff_P = abs(obs_P - ref_P);

			double value = (b1 == bX) ? 10 : 30;  // meters

			os << fixed << setw(10) << " PP[m] " << Obs->sat() << "  " << setw(6) << ObsPre->epoch().sod() << setw(6) << crt_time.sod() <<
				"  " << gobs2str(obs_type) << "  " << gobs2str(ref_obsP) << setw(15) << setprecision(4) << diff_P << endl;

			if (diff_P > value)
			{
				Obs->addoutliers(obs_type, 1);
				Obs->addoutliers(ref_obsP, 1);

				if (_log)
				{
					ostringstream msg;
					msg << fixed << setw(10) << " Outliers in Range : " << crt_time.str_hms() << "   " << Obs->sat() << "   " << gobs2str(ref_obsP)
						<< "   " << gobs2str(obs_type) << "  value  " << setw(16) << setprecision(3) << diff_P << "  >  " << setw(16) << setprecision(3) << value;
					_log->comment(1, crt_site, msg.str());
				}
			}
		}
	}



} // namespace
