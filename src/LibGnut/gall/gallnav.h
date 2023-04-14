
#ifndef GALLNAV_H
#define GALLNAV_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: 
  Version: $ Rev: $

  2011-02-14 /JD: created

-*/

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>

#include "gdata/gnav.h"
#include "gdata/geph.h"
#include "gdata/gdata.h"
#include "gdata/gnavglo.h"
#include "gdata/gnavbds.h"
#include "gutils/gconst.h"
#include "gutils/gtime.h"

#define MAX_GLO_PRN 24
#define NAV_BUF   1024


using namespace std;

namespace gnut
{

	class GNUT_LIBRARY_EXPORT t_gallnav : public t_gdata
	{

	public:
		/** @brief constructor. */
		t_gallnav();
		/** @brief destructor. */
		virtual ~t_gallnav();

		typedef multimap<t_gtime, shared_ptr<t_geph> > t_map_ref;  ///< all data for a single satellite
		typedef map<string, t_map_ref>                 t_map_sat;  ///< all data for all satellites

        virtual int pos(string sat, const t_gtime& t, double  xyz[3], double  var[3] = NULL, double  vel[3] = NULL, bool chk_mask = true); // [m]
		virtual int clk(string sat, const t_gtime& t, double* clk, double* var = NULL, double* dclk = NULL, bool chk_mask = true); // [s]  
		/** @brief add single navigation message. */
		virtual int add(shared_ptr<t_gnav> nav);

		/** @brief return glonass frequency ID. */
		virtual shared_ptr<t_geph> find(string sat, const t_gtime& t, bool chk_mask = true);   // find appropriate t_geph element (interface only)
		map<string, int> glo_freq_num() { return _glo_freq_num; }

	protected:
		virtual shared_ptr<t_geph> _find(string sat, const t_gtime& t, bool chk_mask = true);  // find appropriate t_geph element
		bool               _com;         ///< position/clock reference point (com = true; apc = false);
		int                _offset;      ///< offset for RTCM corrections
		int                _nepoch;      ///< maximum number of epochs (0 = keep all)
		t_map_sat          _mapsat;      ///< map over all satellites (positions,..?)
		bool               _multimap;    ///< use multimap for redundant records
		bool               _overwrite;   ///< overwrite mode (for derived classes with MAP)
		bool               _chk_health;  ///< check satellite health (navigation)
		bool               _chk_navig;   ///< check navigation messages (internal)
		shared_ptr<t_geph> _null;        ///< null pointer
		bool _chk_tot;
		map<string, int> _glo_freq_num;

	};

} // namespace

#endif
