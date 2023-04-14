
#ifndef GSYS_H
#define GSYS_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: definition of GNSS constellations and observation frequencies
  Version: $ Rev: $

  2012-09-26 /JD: created

-*/


#include <set>
#include <map>
#include <vector>
#include <string>
#ifdef BMUTEX
#include <boost/thread/mutex.hpp>
#endif

#include "gutils/gnss.h"
#include "gutils/gobs.h"
#include "gutils/gconst.h"
#include "gutils/gmutex.h"
#include "gutils/ExportGnut.h"

using namespace std;

namespace gnut {

#define  SBS_OFFSET        (100)       ///< offset for SBAS satellites (RINEX3)
#define  QZS_OFFSET        (192)       ///< offset for QZSS satellites (RINEX3)

///< GNSS base frequencies [Hz]
#define  GPS_FRQ      10230000.0       ///< GPS     base frequency
#define  GLO_FRQ     178000000.0       ///< GLONASS base frequency (FDMA)
#define  GLO_FRQ_CDMA 10230000.0       ///< GLONASS base frequency (CDMA)
#define  GAL_FRQ      10230000.0       ///< GALILEO base frequency
#define  BDS_FRQ      10230000.0       ///< BEIDOU  base frequency
#define  QZS_FRQ      10230000.0       ///< QZSS    base frequency
#define  SBS_FRQ      10230000.0       ///< SBAS    base frequency
#define  IRN_FRQ      10230000.0       ///< IRNSS   base frequency

///< GNSS frequency multiplier factors (dimensionless)

#define  G01_MLT          154.0
#define  G02_MLT          120.0
#define  G05_MLT          115.0
#define  R01_MLT            9.0        ///< FDMA                                    1602.0MHz
#define  R02_MLT            7.0        ///< FDMA                                    1246.0MHz
#define  R01_MLT_CDMA       9.0        ///< CDMA                  156.5 * GPS_FRQ = 1600.995 MHz
#define  R02_MLT_CDMA       7.0        ///< CDMA                  122.0 * GPS_FRQ = 1248.06  MHz
#define  R03_MLT_CDMA     117.5        ///< CDMA                  117.5 * GPS_FRQ = 1202.25  MHz L3OC
#define  R05_MLT_CDMA     115.5        ///< CDMA                  115.0 * GPS_FRQ = 1176.45  MHz L5OCM
#define  R01_STEP      562500.0
#define  R02_STEP      437500.0
#define  E01_MLT          154.0
#define  E05_MLT          115.0
#define  E06_MLT          125.0
#define  E07_MLT          118.0
#define  E08_MLT          116.5
#define  C02_MLT          152.6       ///< B1I BeiDou-2 
#define  C06_MLT          124.0       ///< B3I BeiDou-2/3
#define  C07_MLT          118.0       ///< B2I BeiDou-2
#define  C01_MLT          154.0       ///< BeiDou-3
#define  C08_MLT          116.5       ///< BeiDou-3
#define  C05_MLT          115.0       ///< BeiDou-3
#define  C09_MLT          118.0       ///< BeiDou-3
#define  J01_MLT          154.0
#define  J02_MLT          120.0
#define  J05_MLT          115.0
#define  J06_MLT          125.0
#define  S01_MLT          154.0
#define  S05_MLT          115.0
#define  I05_MLT          115.0

/// GNSS derived frequencies [Hz]
#define  G01_F      G01_MLT * GPS_FRQ                ///<  L1    - GPS NAVSTAR
#define  G02_F      G02_MLT * GPS_FRQ                ///<  L2    - GPS NAVSTAR
#define  G05_F      G05_MLT * GPS_FRQ                ///<  L5    - GPS NAVSTAR

#define  R01_F(a)   R01_MLT * GLO_FRQ + R01_STEP*(a) ///<  G1    - GLONASS (FDMA) base frequency  L1 = 1602 MHz + (n x 0.5625) MHz
#define  R02_F(a)   R02_MLT * GLO_FRQ + R02_STEP*(a) ///<  G2    - GLONASS (FDMA) base frequency  L2 = 1246 MHz + (n x 0.4375) MHz
#define  R01_F_CDMA R03_MLT_CDMA * GLO_FRQ_CDMA      ///<  G1    - GLONASS (CDMA)
#define  R02_F_CDMA R05_MLT_CDMA * GLO_FRQ_CDMA      ///<  G2    - GLONASS (CDMA)
#define  R03_F_CDMA R03_MLT_CDMA * GLO_FRQ_CDMA      ///<  G3    - GLONASS (CDMA)
#define  R05_F_CDMA R05_MLT_CDMA * GLO_FRQ_CDMA      ///<  G5    - GLONASS (CDMA)

#define  E01_F      E01_MLT * GAL_FRQ                ///<  E1    - Galileo
#define  E05_F      E05_MLT * GAL_FRQ                ///<  E5a   - Galileo
#define  E06_F      E06_MLT * GAL_FRQ                ///<  E6    - Galileo
#define  E07_F      E07_MLT * GAL_FRQ                ///<  E5b   - Galileo
#define  E08_F      E08_MLT * GAL_FRQ                ///<  E5a+b - Galileo

#define  C02_F      C02_MLT * BDS_FRQ                ///< B1I BeiDou-2 
#define  C06_F      C06_MLT * BDS_FRQ                ///< B3I BeiDou-2/3
#define  C07_F      C07_MLT * BDS_FRQ                ///< B2I BeiDou-2
#define  C01_F      C01_MLT * BDS_FRQ                ///< B1C BeiDou-3
#define  C08_F      C08_MLT * BDS_FRQ                ///< B2  BeiDou-3
#define  C05_F      C05_MLT * BDS_FRQ                ///< B2a BeiDou-3
#define  C09_F      C09_MLT * BDS_FRQ                ///< B2b BeiDou-3

#define  J01_F      J01_MLT * QZS_FRQ                ///<  L1    - QZSS
#define  J02_F      J02_MLT * QZS_FRQ                ///<  L2    - QZSS
#define  J05_F      J05_MLT * QZS_FRQ                ///<  L5    - QZSS
#define  J06_F      J06_MLT * QZS_FRQ                ///<  LEX   - QZSS
													   
#define  S01_F      S01_MLT * SBS_FRQ                ///<  L1    - SBAS
#define  S05_F      S05_MLT * SBS_FRQ                ///<  L5    - SBAS

#define  I05_F      I05_MLT * IRN_FRQ                ///<  I5    - IRNSS

///< OBSOLETE INFORMATIONS -- NOT YET !?
#define GPSLAMB1    CLIGHT/(G01_F)     ///< GPS L1 wave length [m]
#define GPSLAMB2    CLIGHT/(G02_F)     ///< GPS L2 wave length [m]
#define GPSLAMB3   ( ((G01_F*G01_F) / (G01_F*G01_F - G02_F*G02_F)) * GPSLAMB1 - ((G02_F*G02_F) / (G01_F*G01_F - G02_F*G02_F)) * GPSLAMB2 )  ///< GPS ionospher-free wave lenght [m]
#define LAMBWL      CLIGHT/(G01_F-G02_F)   ///< GPS wide-lane wave length [m]



	class GNUT_LIBRARY_EXPORT t_gsys {

	public:
		/**@brief constructor */
		t_gsys(GSYS sys);
		/**@brief constructor */
		t_gsys(string sys);
		/**@brief constructor */
		t_gsys(char c);
		/**@brief constructor */
		t_gsys() {};
		/**@brief destructor */
		~t_gsys() {};
		/**@brief get gobs priority  */
		static GOBS     gobs_priority(GSYS gs, GOBS         gobs);
		/**@brief get band priority  */
		static GOBS     gobs_priority(GSYS gs, GOBSBAND     band);
		/**@brief get default attr  */
		static GOBS     gobs_defaults(GSYS gs, GOBS         gobs1, GOBSBAND     band);
		/**@brief band priority (input sequence) */
		static GFRQ     freq_priority(GSYS gs, FREQ_SEQ     iseq); 
		/**@brief band priority (input sequence) */
		static GOBSBAND band_priority(GSYS gs, FREQ_SEQ     iseq); 
		/**@brief attr priority (input sequence) */
		static GOBSATTR attr_priority(GSYS gs, GOBSBAND gb, GOBSTYPE gt, unsigned int iseq); 
		/**@brief convert band (RINEX 3) to freq enum */
		static GFRQ     band2gfrq(GSYS gs, GOBSBAND band); 
		/**@brief convert band (RINEX 3) to freq sequence */
		static FREQ_SEQ band2freq(GSYS gs, GOBSBAND band); 
		/**@brief convert freq enum to band (RINEX 3) */
		static GOBSBAND gfrq2band(GSYS gs, GFRQ     freq); 
		/**@brief  convert freq enum to frequency sequence */
		static FREQ_SEQ gfrq2freq(GSYS gs, GFRQ     freq); 
		/**@brief convert freq enum to gsys */
		static GSYS     gfrq2gsys(GFRQ     freq); 

		/**@brief convert GSYS enum to GSYS string */
		static string gsys2str(GSYS sys);              
		/**@brief convert GSYS enum to GSYS char */
		static char   gsys2char(GSYS sys);             
		/**@brief convert GSYS string to GSYS enum */
		static GSYS    str2gsys(string s);             
		/**@brief convert sat string to GSYS enum */
		static GSYS    sat2gsys(const string& s);      
		/**@brief convert GSYS string to GSYS char */
		static char    str2char(string s);             
		/**@brief convert GSYS char to GSYS string */
		static string  char2str(char c);               
		/**@brief convert GSYS char to GSYS enum */
		static GSYS   char2gsys(char c);                  
		

		/**@brief convert full sat string */
		static string eval_sat(string sat, GSYS sys = GPS); 
		/**@brief convert full sat string */
		static string eval_sat(int svn, GSYS sys = GPS);  
		/**@brief get BDS GEO satellite  */
		static bool bds_geo(const string& sat);                 
		/**@brief set GSYS from string */
		void   from_string(string sys);             
		/**@brief set GSYS from enum */
		void   from_gsys(GSYS     sys);              
		/**@brief get GSYS enum */
		GSYS   gsys()const { return _gsys; }            
		/**@brief get GSYS string */
		string  str()const { return gsys2str(_gsys); }  

		/**@brief overloaded equivalence operator */
		bool operator==(const string& sys) const;   
		bool operator==(const GSYS& sys) const;     

	protected:

	private:
		GSYS            _gsys;     ///< system
		t_gmutex        _gmutex;
#ifdef BMUTEX   
		boost::mutex    _mutex;
#endif
	};


	class t_gfreq {

	public:
		/**@brief constructor */
		t_gfreq() { _gfreq = GFRQ::LAST_GFRQ; };
		/**@brief destructor */
		~t_gfreq() {};
		/**@brief get GFRQ enum from string */
		static GFRQ   str2gfreq(string frq);      
		/**@brief get string from GFRQ enum */
		static string gfreq2str(GFRQ frq);      


	private:
		GFRQ    _gfreq;    ///< frequency

	};

} // namespace

#endif // GFREQ_H