/**
*
* @verbatim
History
-1.0 hjzheng   2019-01-04 creat the file.
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   ginfolog.h
* @brief	   log funciton
* @author      hjzheng, Wuhan University
* @version	   1.0.0
* @date		   2020-10-08
*
*/

#ifndef GINFOLOG_H
#define GINFOLOG_H

#include "gio/glog.h"
#include "gutils/ExportIFCB.h"

namespace gnut
{
	/** @brief write log information */
	void   IFCB_LIBRARY_EXPORT write_log_info(t_glog* log, int l, string description);
	/** @brief write log information */
	void   IFCB_LIBRARY_EXPORT write_log_info(t_glog* log, int l, string ID, string description);

}
#endif // !GINFOLOG_H
