
#ifndef GIO_H
#define GIO_H

/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements gtime class (day and precise time)
  Version: $ Rev: $

  2011-01-10 /JD: created

-*/

#include <stdio.h>
#include <fstream>
#include <string>

#include "gio/glog.h"
#include "gutils/gmutex.h"
#include "gcoders/gcoder.h"

#define BUF_SIZE 1024

using namespace std;

namespace gnut
{

	class t_gcoder;
	class GNUT_LIBRARY_EXPORT t_gio
	{

	public:
		/**@brief constructor */
		t_gio();
		/**@brief destructor */
		virtual ~t_gio();
		/**@brief start reading (could be run in a separate thread) */
		virtual void run_read();
		/**@brief start reading (could be run in a separate thread) */
		virtual void run_write();
		/**@brief set coder */
		virtual void coder(t_gcoder* coder) { _coder = coder; }

		virtual int init_write() { return _opened = _init_common(); }
		virtual int init_read() { return _opened = _init_common(); }

		virtual int stop_write() { _stop_common(); return 0; }
		virtual int stop_read() { _stop_common(); return 0; }
		/**@brief set path */
		virtual int    path(string str);                     
		/**@brief get path */
		virtual string path()const { return _path; }
		/**@brief set local i/o file */
		void   file(const char* f) { _giof.mask(f); }      
		/**@brief get local i/o file */
		string file() { return _giof.mask(); }
		/**@brief set verbosity */
		void   verb(int i) { _verb = i; }                    
		/**@brief get verbosity */
		int    verb() { return _verb; }
		/**@brief  stop */
		void   stop() { _stop = 1; }                            
		/**@brief  get size */
		size_t size() { return _size; }                        

		int   running() { return _running; }
		int    opened() { return _opened; }
		int connected() { return _opened; }
		/**@brief set glog pointer */
		void glog(t_glog* l) { _log = l; }                     
		/**@brief get glog pointer */
		t_glog* glog() { return _log; }
		/**@brief overloading < operator */
		bool            operator<(const t_gio& n) const;
		/**@brief overloading == operator */
		bool            operator==(const t_gio& n) const;
		/**@brief overloading << operator */ 
		friend ostream& operator<<(ostream& os, const t_gio& n);

	protected:

		virtual int _gio_write(const char* buff, int size) = 0;
		virtual int _gio_read(char* buff, int size) = 0;

		/**@brief local log file archive */
		virtual int _locf_write(const char* buff, int size);
		/**@brief local log file source */
		virtual int _locf_read(char* buff, int size);
		/**@brief common function for initialization */
		virtual int _init_common();
		/**@brief common function for socket/file close */
		virtual int _stop_common();

		t_glog*         _log;              ///< log pointer
		int             _fd;               ///< file descriptor
		size_t          _size;             ///< buffer size
		string          _path;             ///< URL-like path (e.g. file:///home/honza/file)
		t_giof          _giof;             ///< local file
		int             _count;            ///< record counter
		int             _verb;             ///< verbosity
		int             _stop;             ///< require a stop at run() loop
		int             _opened;           ///< 1: opened/connected
		int             _running;          ///< running
		t_gcoder*       _coder;            ///< decoder/encoder
		t_gmutex        _gmutex;           ///< mutual exlusion

#ifdef BMUTEX
		boost::mutex    _mutex;            ///< mutual exlusion
#endif

	private:

	};

} // namespace

#endif
