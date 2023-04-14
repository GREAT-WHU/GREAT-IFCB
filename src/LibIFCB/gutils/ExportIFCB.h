/**
*
* @verbatim
	History
	-1.0 jdhuang  2019-01-24  creat the file.
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		ExportIFCB.h
* @brief	for the use of Lib IFCB(inter-frequency clock bias).
*
* @author   Jiande Huang , Wuhan University
* @version	1.0.0
* @date		2019-10-24
*
*/

#ifndef IFCB_EXPORT_H
#define IFCB_EXPORT_H

// ½ûÓÃVS¾¯¸æ
#if defined(_MSC_VER)
#pragma warning( disable : 4244 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 4512 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4702 )
#pragma warning( disable : 4511 )
#pragma warning( disable : 4996 )
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  if defined(GREAT_IFCB_LIBRARY )
#    define  IFCB_LIBRARY_EXPORT __declspec(dllexport)
#  else
#    define IFCB_LIBRARY_EXPORT  __declspec(dllimport)
#  endif
#else
#  define IFCB_LIBRARY_EXPORT
#endif


#ifdef _MSC_VER
# if (_MSC_VER >= 1300)
#  define __STL_MEMBER_TEMPLATES
# endif
#endif

// for check whether output the log file or not 
//#define DEBUG_AMBFIX

#include <map>
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <iostream>

using namespace std;


#endif 

