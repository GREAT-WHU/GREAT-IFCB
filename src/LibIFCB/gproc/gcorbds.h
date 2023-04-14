/**
*
* @verbatim
History
-1.0 glfeng    2020-10-06 creat the file.
-1.1 hjzheng   2020-10-18 add gsys in band_index for bancroft
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gcorbds.h
* @brief	   correct code bias on bds system
* @author      glfeng, Wuhan University
* @version	   1.1.0
* @date		   2020-10-18
*
*/

#ifndef QUALITYCONTROL_H
#define QUALITYCONTROL_H

#include "gall/gallnav.h"
#include "gall/gallobs.h"
#include "gdata/gobsgnss.h"

#include "gset/gsetgen.h"
#include "gset/gsetgnss.h"
#include "gutils/ExportIFCB.h"

namespace great
{
	using namespace gnut;

	/**
	*@brief	   Class for BDS satellite-induced code pseudorange variations correct
	*/
	class IFCB_LIBRARY_EXPORT t_gbds_codebias_cor
	{
	public:

		/** @brief default constructor. */
		t_gbds_codebias_cor(t_gsetbase* settings);
		/** @brief default destructor. */
		virtual ~t_gbds_codebias_cor() {};
		/** @brief apply correction on bds observations about IGSO and MEO. */
		void apply_IGSO_MEO(const t_gtime& now,const string& rec, t_gtriple& rec_crd, t_gallnav* gnav, vector<t_gobsgnss>& obsdata);

	private:

		t_gsetbase*                          _set;						///< settings of xml
		map< GSYS, map<FREQ_SEQ, GOBSBAND> > _band_index;				///< band of observations used on xml
		bool                                 _correct_bds_code_bias;	///< correct bds code bias or not
		map<GOBSBAND, map<string, map<int, double> > > _IGSO_MEO_Corr;  ///< // Wanninger & Beer : BeiDou satellite-induced code pseudorange variations: diagnosis and therapy [unit:m]
		
		/** @brief get recciver postion by SPP */
		bool _recAprCoordinate(const t_gtime& now,const string& rec, t_gtriple& rec_crd, t_gallnav* gnav, vector<t_gobsgnss>& obsdata);

		/** @brief get satllite postions and clock  */
		bool _satPosClk(const t_gtime& now, string sat, t_gallnav* gnav, double P3, t_gtriple& sat_pos, double& sat_clk);
	};

} 

#endif
