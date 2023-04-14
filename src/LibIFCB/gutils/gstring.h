/**
*
* @verbatim
History
-1.0 hjzheng   2020-10-06 creat the file.
-1.1 hjzheng   2020-10-18 Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gstring.h
* @brief	   tool function for string
* @author      hjzheng, Wuhan University
* @version	   1.1.0
* @date		   2020-10-18
*
*/

#include <string>
#include "gutils/ExportIFCB.h"
using namespace std;

namespace great
{
	/** @brief Capitalize all characters in the string */
	IFCB_LIBRARY_EXPORT std::string str2upper(const std::string& str);

	/** @brief Change all characters in the string to lowercase */
	IFCB_LIBRARY_EXPORT std::string str2lower(const std::string& str);
}
