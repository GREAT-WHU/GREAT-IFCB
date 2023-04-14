/**
*
* @verbatim
History
-1.0 hjzheng   2020-10-06 creat the file.
-1.1 hjzheng   2020-10-18 Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gcfg_ifcb.h
* @brief	   control set from XML
* @author      hjzheng, Wuhan University
* @version	   1.0.0
* @date		   2019-04-10
*
*/

#ifndef GCFG_IFCB_H
#define GCFG_IFCB_H

#include "gutils/ExportIFCB.h"
#include "gset/gsetgen.h"
#include "gset/gsetgnss.h"
#include "gset/gsetinp.h"
#include "gset/gsetout.h"
#include "gset/gsetpreedit.h"

using namespace gnut;

namespace great
{
	/**
	*@brief	   Class for IFCB contrl setting
	*/
	class IFCB_LIBRARY_EXPORT t_gcfg_ifcb :
		public t_gsetgen,
		public t_gsetinp,
		public t_gsetout,
		public t_gsetgnss,
		public t_gsetpreedit
	{
	public:

		/** @brief default constructor. */
		t_gcfg_ifcb();

		/** @brief default destructor. */
		virtual ~t_gcfg_ifcb();

		/** 
		* @brief settings check. 
		* @return void
		*/
		void check();                                 // settings check

		/** 
		* @brief settings help. 
		* @return void
		*/
		void help();                                  // settings help

	protected:

	private:
	};

}

#endif 