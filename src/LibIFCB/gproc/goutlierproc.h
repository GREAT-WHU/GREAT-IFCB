/**
* @verbatim
History
-1.0 hjzheng	 2022-07-02  creat the file.
@endverbatim
* Copyright (c) 2020, Wuhan University. All rights reserved.
*
* @file			goutlierproc.h
* @brief		detecting range outliers
* @author       hjzheng, Wuhan University
* @version		1.0.0
* @date			2022-07-02
*
*/

#ifndef GOUTLIERPROC_H
#define GOUTLIERPROC_H

#include "gall/gallnav.h"
#include "gall/gallobs.h"

#include "gset/gsetgen.h"

#include "gset/gsetgnss.h"
#include "gutils/ExportIFCB.h"

using namespace gnut;

namespace great
{

	/**
	* @brief class for detecting range outliers
	*/
	class IFCB_LIBRARY_EXPORT t_goutliers_process
	{
	public:
		/**
		* @brief default constructor, init log.
		* @param[in] log    log for detecting range outliers
		*/
		t_goutliers_process(t_glog* glog);

		/** @brief default destructor. */
		virtual ~t_goutliers_process();

		/**
		* @brief main process of detecting range outliers
		* @param[in] ObsPre		previous epoch observations
		* @param[in] Obs		current epoch observations
		* @param[in] sampling	sampling interval.
		*/
		void flagRangeOutliers(shared_ptr<t_gobsgnss> ObsPre, shared_ptr<t_gobsgnss> Obs, double sampling);

	private:

		t_glog*            _log;	///< general log output
	};

} // namespace

#endif
