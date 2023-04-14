/**
*
* @verbatim
History
-1.0 glfeng BoWong xjhan 2019-02-26  creat the file.
-1.1 BoWong              2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gambcommon.cpp
* @brief	declare some classes and some mathematical method.
*
* @author   glfeng BoWong xjhan, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/
#include "gambfix/gambcommon.h"
using namespace std;

namespace great
{


	void getMeanWgt(bool is_edit, list<pair<double, double>>& wx, double& mean, double& sigma, double& mean_sig)
	{
		try
		{
			if (wx.empty())
			{
				mean_sig = 0.0;
			    return;
			}
			double wgt_sum;

			bool loop_flag = true;
			while (loop_flag)
			{
				// mean
				mean = 0.0;
				wgt_sum = 0.0;
				for (auto itwx = wx.begin(); itwx != wx.end(); itwx++)
				{
					mean += itwx->first * itwx->second;
					wgt_sum += itwx->second;
				}
				mean /= wgt_sum;

				// sigma
				sigma = 0.0;
				wgt_sum = 0.0;
				for (auto itwx = wx.begin(); itwx != wx.end(); itwx++)
				{
					sigma += (itwx->first - mean)*(itwx->first - mean) * itwx->second;
					wgt_sum += itwx->second;
				}
				if (wx.size() == 1)
				{
					sigma = 999.0;
					mean_sig = 999.0;
				    return;
				}
				else
				{
					sigma = sqrt(sigma / (wx.size() - 1.0));
					mean_sig = sigma / sqrt(wgt_sum);
				}

				if (is_edit)
				{
					loop_flag = false;
					for (auto itwx = wx.begin(); itwx != wx.end();)
					{
						if (fabs(itwx->first - mean) > 3 * (sigma))
						{
							itwx = wx.erase(itwx);
							loop_flag = true;
						}
						else
						{
							itwx++;
						}

					}
					if (!loop_flag && sigma > 0.04 && wx.size() >= 5) {
						auto itwx = wx.begin();
						auto maxwx = itwx;
						for (; itwx != wx.end(); itwx++) {
							if (fabs(itwx->first - mean) > fabs(maxwx->first - mean)) {
								maxwx = itwx;
							}
						}
						wx.erase(maxwx);
						loop_flag = true;
					}
				}
				else
				{
				    return;
				}
			}
		    return;
		}
		catch (...)
		{
			cout << "ERROR: t_gmath::get_wgt_mean , unknown mistake";
			throw(-1);
		}
	}

}