#ifndef GNUT_EXPORT_H
#define GNUT_EXPORT_H


///< Disable VS warnings
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
#  if defined(GREATUE_GNUT_LIBRARY )
#    define GNUT_LIBRARY_EXPORT  __declspec(dllexport)
#  else
#    define GNUT_LIBRARY_EXPORT  __declspec(dllimport)
#  endif
#else
#  define GNUT_LIBRARY_EXPORT
#endif


#ifdef _MSC_VER
# if (_MSC_VER >= 1300)
#  define __STL_MEMBER_TEMPLATES
# endif
#endif



#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#endif //GNUT_EXPORT_H

