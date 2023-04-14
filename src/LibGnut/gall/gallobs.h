
#ifndef GALLOBS_H
#define GALLOBS_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: observation container
  Version: $ Rev: $

  2012-05-02 /JD: created

-*/

#include <iostream>
#include <string.h>
#include <map>
#include <set>
#include <memory>

#include "gdata/gdata.h"
#include "gdata/gobsgnss.h"
#include "gutils/gtypeconv.h"
#include "gutils/gconst.h"
#include "gutils/gtime.h"
#include "gutils/gnss.h"
#include "gutils/gsys.h"
#include "gutils/gobs.h"
#include "gset/gsetgen.h"

#define DIFF_SEC_NOMINAL 0.905  ///< [sec] returns observations within +- DIFF_SEC for 1Hz


#define DIFF_SEC(a) (((a)>(0.0)&&(a)<(1.0))?(DIFF_SEC_NOMINAL*a):(DIFF_SEC_NOMINAL))  ///< normalize for 1Hz !

using namespace std;

namespace gnut
{

	typedef shared_ptr<t_gobsgnss> t_spt_gobs;

	class GNUT_LIBRARY_EXPORT t_gallobs : public t_gdata
	{

	public:
		/** @brief constructor. */
		t_gallobs();

		/** @brief destructor. */
		virtual ~t_gallobs();

		enum XDATA { XDATA_BEG, XDATA_END, XDATA_SMP, XDATA_SYS };    ///< QC

		typedef map<XDATA, int>                      t_map_xdat;      ///< map of site filtered data (QC)
		struct  t_xfilter { t_map_xdat xdat; t_gtime beg, end; };     ///< filtered data (QC)
		typedef map<string, t_xfilter>               t_map_xfil;      ///< map of file filtered data (QC)
		typedef map<string, t_map_xfil>              t_map_xflt;      ///< map of  all filtered data (QC)


		typedef map<string, t_spt_gobs>              t_map_osat;      ///< all data-types/single epoch
		typedef map<t_gtime, t_map_osat>              t_map_oref;     ///> all data-types/all epochs/single object
		typedef map<string, t_map_oref>              t_map_oobj;      ///< all data-types/all epochs/all objects
		
	    /** @brief settings. */
		virtual void gset(t_gsetbase*);

		/** @brief return list of available satellites. */
		virtual vector<t_gobsgnss> obs(const string& site, const t_gtime& t);

		/** @get all t_gobsgnss pointers for epoch t. */
		virtual vector<t_spt_gobs> obs_pt(const string& site, const t_gtime& t);   

		/** @get all t_gobsgnss pointers for prn in interval. */
		virtual vector<t_spt_gobs> obs_prn_pt(const string& site, const string& prn,
			const t_gtime& beg, const t_gtime& end); 

		/** @brief add site-specific filtered data/epochs. */
		void      xdata(string site, string file, t_xfilter xflt);             
		
		/** @brief add single station observation (P and L in meters !). */
		int addobs(t_spt_gobs obs);  

		/** @brief return list of available satellites. */
		virtual t_map_osat find(string site, const t_gtime& t);

		/** @brief return obs of specific satellite. */
		virtual double     find(string site, const t_gtime& t, const string& prn, const GOBS& gobs);

		void addsitecrd(string site, t_gtriple crd) { _mapcrds[site] = crd; }
		t_gtriple getsitecrd(string site) { return _mapcrds[site]; }
		const set<string>& getSiteList() { return _map_sites; }

	protected:
		/** @brief return list of available satellites. */
		virtual vector<t_gobsgnss> _gobs(const string& site, const t_gtime& t);

		/** @brief find epoch from the map w.r.t. DIFF_SEC. */
		int _find_epo(const string& site, const t_gtime& epo, t_gtime& tt); 

		t_gsetbase*       _set;         ///< settings
		unsigned int      _nepoch;      ///< maximum number of epochs (0 = keep all)
		t_map_oobj        _mapobj;      ///< map over all objects (receivers)
		t_map_xflt        _filter;      ///< structure of stations/files filtered data (QC)
		bool              _overwrite;   ///< rewrite/add only mode
		set<string>       _sys;         ///< systems settings
		double            _smp;         ///< sampling settings
		map<string, t_gtriple>   _mapcrds;  // all sites apr coordinates
		set<string>       _map_sites;

	private:
	};

} // namespace

#endif
