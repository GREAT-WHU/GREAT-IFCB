
#ifndef MUTEX_H
#define MUTEX_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: mutual exclusion
  Version: $ Rev: $

  2013-08-14 /JD: created

-*/

#if defined _WIN32 || defined _WIN64
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "gutils/ExportGnut.h"

namespace gnut
{

	class GNUT_LIBRARY_EXPORT t_gmutex
	{
	public:
		/**@brief constructor */
		t_gmutex();
		/**@brief destructor */
		~t_gmutex();
		/**@brief lock */
		void lock();
		/**@brief unlock */
		void unlock();
#ifdef WIN32
		CRITICAL_SECTION* get_mutex();
#else
		pthread_mutex_t* get_mutex();
#endif

	protected:
#ifdef WIN32
		CRITICAL_SECTION _mutex;
#else
		pthread_mutex_t _mutex;
#endif
	};

} // namespace

#endif // MUTEX_H
