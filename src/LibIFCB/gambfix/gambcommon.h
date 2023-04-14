/**
*
* @verbatim
History
-1.0 glfeng BoWong xjhan 2019-02-26  creat the file.
-1.1 BoWong              2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gambcommon.h
* @brief	declare some classes and some mathematical method.
*
* @author   glfeng BoWong xjhan, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/
#ifndef GAMBCOMMON_H
#define GAMBCOMMON_H 

#include "gutils/ExportIFCB.h"
#include <string>
#include <map>
#include <memory>
#include <tuple>
#include <list>
#include "gutils/gtime.h"
#include "gutils/gtypeconv.h"

using namespace std;
using namespace gnut;

namespace great
{
	/**
	*@ brief zero-difference ambiguity structure  for arc.
	*/
	class IFCB_LIBRARY_EXPORT t_oneway_ambiguity
	{
	public:

		/** @brief default constructor. */
		t_oneway_ambiguity() {};

		/** @brief default destructor. */
		virtual ~t_oneway_ambiguity() {};

		string ambtype;  ///< C/1/2/W
		string sat;
		int    ipt     = 0;         ///< index of par
		int    beg_epo = 0;     ///< start time    
		int    end_epo = 0;     ///< end time
		double rwl   = 0.0;     ///< real value widelane(cyc) jdhuang : remove warnning
		double srwl  = 0.0;     ///< its sigma
		double rewl  = 0.0;     ///< real value extrawidelane(cyc)
		double srewl = 0.0;     ///< its sigma
		double rlc  = 0.0;      ///< real value lc from slution(m)
		double srlc = 0.0;     ///< its sigma
		double ele  = 0.0;      ///< elevation
		double r    = 0.0;        ///< ambiguity in UCUD mode
		double sr   = 0.0;       ///< sigma of ambiguity
	};


	/** @brief map for storaging oneway_ambiguity  */
	typedef vector<shared_ptr<t_oneway_ambiguity>> IFCB_LIBRARY_EXPORT t_OW_amb;  //satellites OW;

	/** @brief map for storaging oneway_ambiguity , one station/all satellite/all epoch. */
	typedef map<string, t_OW_amb> IFCB_LIBRARY_EXPORT t_all_sats_OW;  //satellites OW;

	//** @brief map for storaging ambiguity value, one station/all epo/all satellite. */
	typedef map<int, map<string, double> > IFCB_LIBRARY_EXPORT t_epo_sat_amb;


	/**
	* @brief Get mean, sigma and sigma of the mean of a set sampled data.
	* @param[in]  is_edit   whether to eliminate errors.
	* @param[in]  wx     	list pairs contain data and it's weight.
	* @param[in]  mean	    data mean.
	* @param[in]  sigma	    data sigma.
	* @param[in]  sigx	    sigma's sigma.
	* @param[out] mean
	* @param[out] sigma
	* @param[out] sigx
	* @return	  void
	*/
	IFCB_LIBRARY_EXPORT void getMeanWgt(bool is_edit, list<pair<double, double>>& wx, double& mean, double& sigma, double& mean_sig);


}

#endif
