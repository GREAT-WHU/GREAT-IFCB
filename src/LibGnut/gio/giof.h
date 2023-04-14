
#ifndef GIOF_H
#define GIOF_H

/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: class derived from fstream
           - file name/mask added
           - support file hunting according to date/time
           - automated open/close handling
  Version: $ Rev: $

  2011-10-15 /JD: created

-*/

#include <fstream>
#include <string>

#ifdef BMUTEX
#include <boost/thread/mutex.hpp>
#endif

#include "gutils/gtime.h"
#include "gutils/gmutex.h"
#include "gutils/ExportGnut.h"

using namespace std;

namespace gnut
{

	class GNUT_LIBRARY_EXPORT t_giof : public fstream
	{

	public:
		/**@brief constructor */
		t_giof(string mask = "");
		/**@brief destructor */
		virtual ~t_giof();
		/**@brief set file mask */
		int    mask(string mask);                       
		/**@brief get file mask */
		string mask()const { return _mask; }             
		/**@brief get last filename */
		string name()const { return _name; }            

		/**@brief get irc status */
		int irc()const { return _irc; };                
		/**@brief set loop read */
		void loop(bool l) { _loop = l; }             
		/**@brief get loop read */
		bool loop()const { return _loop; }              
		/**@brief set time offset [min] for the file name */
		void toff(int i) { _toff = i; }                
		/**@brief get time offset [min] */
		int  toff()const { return _toff; }               
		/**@brief local log file archive */
		virtual int write(const char* buff, int size);        
		/**@brief local log file archive */
		int write(const string& s);
		/**@brief local log file source */
		int read(char* buff, int size);              
		/**@brief  append mode [false/true]*/
		void append(const bool& b = true);           
		/**@brief set time system for replacement */
		void tsys(t_gtime::t_tsys);                   
		/**@brief get time system for replacement */
		t_gtime::t_tsys tsys();



	protected:
		/**@brief replace mask to name */
		string _replace();                          

		int                _irc;                        ///< irc status OK=0, Warning bigger than 0, Error<0
		string             _mask;                       ///< original name mask
		string             _name;                       ///< actual (evaluated) name
		ios::openmode      _omode;                      ///< output open mode
		bool               _repl;                       ///< replace if time-specific
		int                _toff;                       ///< if replace, time offset [min] for the file name
		bool               _loop;                       ///< loop read
		t_gtime::t_tsys    _tsys;                       ///< time system for replacement
		t_gmutex           _gmutex;                     ///< mutual exlusion

#ifdef BMUTEX
		boost::mutex       _mutex;                      ///< mutual exlusion
#endif
	private:

	};

} // namespace

#endif
