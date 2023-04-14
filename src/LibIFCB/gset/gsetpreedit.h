/**
*
* @verbatim
History
-1.0 hzjheng	2022-07-02 created
@endverbatim
* Copyright (c) 2020, Wuhan University. All rights reserved.
*
* @file			gsetturboedit.h
* @brief		preedit set 
* @author       hjzheng, Wuhan University
* @version		1.0.0
* @date			2022-07-02
*
*/

#ifndef GSETPREEDIT_H
#define GSETPREEDIT_H

#define XMLKEY_PREEDIT             "preedit"

#include <map>
#include <string>
#include <iostream>

#include "gset/gsetbase.h"
#include "gutils/ExportIFCB.h"

using namespace std;
using namespace gnut;

namespace great
{

	/**
	*@brief class for setting of process modelue in XML file
	*/
	class IFCB_LIBRARY_EXPORT t_gsetpreedit : public virtual t_gsetbase
	{
	public:
		/** @brief default constructor. */
		t_gsetpreedit();

		/** @brief default destructor. */
		virtual ~t_gsetpreedit() {};

		/**
		* @brief get the minimum elevation angle of check 
		* @return double: minimum elevation angle [unit:degrees]
		*/
		double minimum_elev();

		/**
		* @brief the setting of MW combination in detecting cycle slip
		* @param[out] mw_limit  MW combination limitation [unit:cycles]
		* @return bool : true -- using MW combination; false -- not using MW combination
		*/
		bool checkMW(double& mw_limit);

		/**
		* @brief the setting of GF combination in detecting cycle slip
		* @param[out] gf_limit  GF combination limitation [unit:meters]
		* @return bool : true -- using GF combination; false -- not using GF combination
		*/
		bool checkGF(double& gf_limit);

		/**
		* @brief the setting of checking gapped epoch arc  in detecting cycle slip
		* @param[out] gap_limit  gapped epoch arc limitation [unit:epochs]
		* @return bool : true -- checking gapped epoch arc; false -- not checking gapped epoch arc
		*/
		bool checkGap(int& gap_limit);

		/**
		* @brief the setting of checking  short epoch arc in detecting cycle slip
		* @param[out] short_limit  short epoch arc limitation [unit:epochs]
		* @return bool : true -- checking short epoch arc; false -- not checking short epoch arc
		*/
		bool checkShort(int& short_limit);

		/**
		* @brief settings help.
		* @return void
		*/
		void help();

	protected:
		
		double _defaultMinimumEle;		///< default minimum elevation angle
		double _defaultMWLimit;			///< default MW combination limitation
		double _defaultGFLimit;			///< default GF combination limitation
		int    _defaultGapArcLimit;		///< default gapped epoch arc limitation
		int    _defaultShortArcLimit;	///< default short epoch arc limitation

	};
}
#endif // !SETPREEDIT_H
