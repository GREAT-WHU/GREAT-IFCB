
#ifndef GLOG_H
#define GLOG_H

/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements glog class derived from giof
  Version: $ Rev: $

  2011-10-10 /JD: created

-*/

#include <string>
#include <vector>

#include "gio/giof.h"

#define CACHE_LINES 300

using namespace std;

namespace gnut
{

	class GNUT_LIBRARY_EXPORT t_glog : public t_giof
	{
	public:
	    /**@brief constructor */
		t_glog(string mask = "");
		/**@brief destructor */
		virtual ~t_glog();
		/**@brief comment */
		void comment(int l, const string& str);
		/**@brief comment & identificator */
		void comment(int l, const string& ide, const string& str);

		
		/**@brief time stamp */
		void time_stamp(bool b);
		/**@brief time stamp */
		bool time_stamp() const;
		/**@brief cache size */
		void cache_size(int i);
		/**@brief cache size */
		int  cache_size() const;
		/**@brief set verbosity */
		void verb(int i);
		/**@brief get verbosity */
		int  verb() const;
		/**@brief clear cache */
		void clear();

	protected:

		bool            _time;             ///< time stamp
		int             _verb;             ///< verbosity
		int             _size;             ///< cache size
		vector<string>  _cache;            ///< cache for messages
		t_gmutex        _log_gmutex;       ///< special mutex for comments

#ifdef BMUTEX
		boost::mutex    _log_mutex;
#endif

	};

} // namespace

#endif
