/**
*
* @verbatim
History
-1.0 hjzheng   2019-01-04 creat the file.
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   ginfolog.cpp
* @brief	   log funciton
* @author      hjzheng, Wuhan University
* @version	   1.0.0
* @date		   2020-10-08
*
*/

#include "ginfolog.h"
#include <iostream>
#include <iomanip>



void gnut::write_log_info(t_glog * log, int l, string description)
{
	if (log)
	{
		log->comment(l, description);
	}
	else
	{
		std::cout << setw(88) << description << endl;
	}
}

void gnut::write_log_info(t_glog * log, int l, string ID, string description)
{
	if (log)
	{
		log->comment(l, ID, description);
	}
	else
	{
		std::cout << std::setw(8) << ID + " : " << std::setw(88) << description << endl;
	}
}
