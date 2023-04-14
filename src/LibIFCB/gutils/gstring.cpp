/**
*
* @verbatim
History
-1.0 hjzheng   2020-10-06 creat the file.
-1.1 hjzheng   2020-10-18 Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gstring.cpp
* @brief	   tool function for string
* @author      hjzheng, Wuhan University
* @version	   1.1.0
* @date		   2020-10-18
*
*/

#include "gutils/gstring.h"
#include "gutils/ExportIFCB.h"
namespace great
{
	string str2upper(const string & str)
	{
		string tmp = str;
		transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
		return tmp;
	}

	string str2lower(const string & str)
	{
		string tmp = str;
		transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		return tmp;
	}

}