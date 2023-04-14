
#ifndef GNSS_H
#define GNSS_H
 
/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
  
  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: definition of GNSS data
  Version: $ Rev: $

  2012-09-26 /JD: created

-*/

#include <map>
#include <set>
#include <vector>
#include <string>

#include "gutils/gtriple.h"
#include "gutils/gcommon.h"  
#include "gutils/ExportGnut.h"
using namespace std;

// ------------------------------------------------------------------------------------------------------
// ENUMS
// ------------------------------------------------------------------------------------------------------

namespace gnut
{

	///< GNSS systems and augmentations
	enum GSYS { // GXX = -1,
		GPS, GAL, GLO, BDS, QZS, SBS, IRN, GNS
	};

	///< GNSS freq Sequence ID
	enum FREQ_SEQ {
		FREQ_1 = 1, FREQ_2 = 2, FREQ_3 = 3, FREQ_4 = 4, FREQ_5 = 5, FREQ_6=6, FREQ_7=7,
		FREQ_X = 999
	};

	///< GNSS frequencies
	enum GFRQ { // FXX = -1,
		G01 = 10, G02 = 11, G05 = 12,     ///< GPS
		R01 = 20, R02 = 21,               ///< GLONASS FDMA
		R01_CDMA = 30, R02_CDMA = 31,		
		R03_CDMA = 32, R05_CDMA = 33,     ///< GLONASS CDMA
		E01 = 50, E05 = 51, E07 = 52,		
		E08 = 53, E06 = 54,               ///< Galileo
        C02 = 60, C07 = 61, C06 = 62,     ///< BeiDou
		J01 = 70, J02 = 71, J05 = 72,		
		J06 = 73,                         ///< QZSS
		S01 = 80, S05 = 81,               ///< SBAS
		I05 = 90,                         ///< IRNSS
		LAST_GFRQ = 999
	};

	///< GNSS receiver types
	enum RECTYPE {
		P1P2,    ///< receiver providing C1, P1, P2
		C1X2,    ///< cross-correlation
		C1P2     ///< modern receivers providing C1, P2
	};

	///< Broadcast messages types
	enum GNAVTYPE { FNAV, INAV, INAV_E01, INAV_E07, CNAV, NAV };



	///< GNSS type/band/attr definitions
	enum GOBSTYPE {
		TYPE_C = 1, TYPE_L = 2, TYPE_D = 3, TYPE_S = 4,
		TYPE_P = 101, // only for P-code!
		TYPE = 999    // ""  UNKNOWN
	};
	enum GOBSBAND {
		BAND_1 = 1, BAND_2 = 2, BAND_3 = 3, BAND_5 = 5,
		BAND_6 = 6, BAND_7 = 7, BAND_8 = 8, BAND_9 = 9,
		BAND_A = 101, BAND_B = 102, BAND_C = 103, BAND_D = 104,
		BAND = 999    // ""  UNKNOWN
	};
	enum GOBSATTR {
		ATTR_A, ATTR_B, ATTR_C, ATTR_D, ATTR_I, ATTR_L, ATTR_M, ATTR_N,
		ATTR_P, ATTR_Q, ATTR_S, ATTR_W, ATTR_X, ATTR_Y, ATTR_Z,
		ATTR_NULL,    // " " 2CHAR code
		ATTR = 999    // ""  UNKNOWN
	};

	///< GNSS observations
	enum GOBS {

		///< psedorange [in meters] (RINEX 3.x)
		C1A = 0, C1B, C1C,C1D, C1I, C1L, C1M, C1P, C1S, C1Q, C1W, C1X, C1Y, C1Z,
		C2C, C2D, C2I, C2L, C2M, C2P, C2S, C2Q, C2W, C2X, C2Y,
		C3I, C3Q, C3X,
		C5A, C5B, C5C, C5D, C5I, C5P, C5Q, C5X,
		C6A, C6B, C6C, C6I, C6L, C6S, C6Q, C6X, C6Z,
		C7I, C7Q, C7X,
		C8D, C8I, C8P, C8Q, C8X,
		C9D, C9P, C9Z,
		

		///< carrier phase [in whole cycles] (RINEX 3.x)
		L1A = 100, L1B, L1C, L1D, L1I, L1L, L1M, L1N, L1P, L1S, L1Q, L1W, L1X, L1Y, L1Z,
		L2C, L2D, L2I, L2L, L2M, L2N, L2P, L2S, L2Q, L2W, L2X, L2Y,
		L3I, L3Q, L3X,
		L5A, L5B, L5C, L5D, L5I, L5P, L5Q, L5X,
		L6A, L6B, L6C, L6I, L6L, L6S, L6Q, L6X, L6Z,
		L7I, L7Q, L7X,
		L8D, L8I, L8P, L8Q, L8X,
		L9D, L9P, L9Z,
		

		///< doppler [cycles/sec] (RINEX 3.x)
		D1A = 200, D1B, D1C, D1I, D1L, D1M, D1N, D1P, D1S, D1Q, D1W, D1X, D1Y, D1Z,
		D2C, D2D, D2I, D2L, D2M, D2N, D2P, D2S, D2Q, D2W, D2X, D2Y,
		D3I, D3Q, D3X,
		D5A, D5B, D5C, D5I, D5Q, D5X,
		D6A, D6B, D6C, D6I, D6L, D6S, D6Q, D6X, D6Z,
		D7I, D7Q, D7X,
		D8I, D8Q, D8X,
	


		///< signal strength [DBHZ] (RINEX 3.x)
		S1A = 300, S1B, S1C, S1I, S1L, S1M, S1N, S1P, S1S, S1Q, S1W, S1X, S1Y, S1Z,
		S2C, S2D, S2I, S2L, S2M, S2N, S2P, S2S, S2Q, S2W, S2X, S2Y,
		S3I, S3Q, S3X,
		S5A, S5B, S5C, S5I, S5Q, S5X,
		S6A, S6B, S6C, S6I, S6L, S6S, S6Q, S6X, S6Z,
		S7I, S7Q, S7X,
		S8I, S8Q, S8X,


		///< special cases: v2.x or unknown tracking modes
		P1 = 1000, P2, P5, C1, C2, C5, C6, C7, C8, CA, CB, CC, CD,
		L1 = 1100, L2, L5, L6, L7, L8, LA, LB, LC, LD,
		D1 = 1200, D2, D5, D6, D7, D8, DA, DB, DC, DD,
		S1 = 1300, S2, S5, S6, S7, S8, SA, SB, SC, SD,

		X ///< LAST_GOBS
	};

	enum GOBS_LC { LC_UNDEF = 0, LC_L1 = 1, LC_L2 = 2, LC_L3 = 3, LC_L4 = 4, LC_L5 = 5, LC_IF, LC_MW, LC_NL, LC_WL, LC_GF };


	// ------------------------------------------------------------------------------------------------------
	// TYPEDEF
	// ------------------------------------------------------------------------------------------------------
	typedef vector< GOBSATTR >          t_vec_attr;
	typedef vector< GOBSBAND >          t_vec_band;
	typedef vector< GFRQ     >          t_vec_freq;

	typedef map< GOBSTYPE, t_vec_attr > t_map_attr;
	typedef map< GOBSBAND, t_map_attr > t_map_type;

	typedef map< GSYS, set<string> >    t_map_sats;
	typedef map< GSYS, set<string> >    t_map_gnav;
	typedef map< GSYS, t_map_type  >    t_map_gnss;
	typedef map< GSYS, t_vec_band  >    t_map_band;
	typedef map< GSYS, t_vec_freq  >    t_map_freq;

	typedef map< GOBSBAND, t_gtriple >  t_map_pcos;  ///< triple: ATX  NORTH / EAST / UP
	typedef map< GSYS, t_map_pcos >  t_map_offs;

	// ------------------------------------------------------------------------------------------------------
	// GLOBAL FUNCTIONS
	// ------------------------------------------------------------------------------------------------------
	/**@brief get GOBSATTR enum from gobs string */
	GNUT_LIBRARY_EXPORT GOBSATTR str2gobsattr(string s);           
	/**@brief get GOBSBAND enum from gobs string */
	GNUT_LIBRARY_EXPORT GOBSBAND str2gobsband(string s);           
	/**@brief get GOBSTYPE enum from gobs string */
	GNUT_LIBRARY_EXPORT GOBSTYPE str2gobstype(string s);                     

	/**@brief get GOBSATTR enum from char */
	GNUT_LIBRARY_EXPORT GOBSATTR char2gobsattr(char c);            
	/**@brief get GOBSBAND enum from char */
	GNUT_LIBRARY_EXPORT GOBSBAND char2gobsband(char c);                        
	/**@brief get GOBSTYPE enum from char */
	GNUT_LIBRARY_EXPORT GOBSTYPE char2gobstype(char c);            

	/**@brief get string enum from GOBSATTR */
	GNUT_LIBRARY_EXPORT string gobsattr2str(GOBSATTR e);            
	/**@brief get string enum from GOBSBAND */
	GNUT_LIBRARY_EXPORT string gobsband2str(GOBSBAND e);           
	/**@brief get string enum from GOBSTYPE  */
	GNUT_LIBRARY_EXPORT string gobstype2str(GOBSTYPE e);                    
	/**@brief get string from GOBS enum */
	GNUT_LIBRARY_EXPORT string gobs2str(GOBS);                     
	/**@brief get GOBS enum from string */
	GNUT_LIBRARY_EXPORT GOBS str2gobs(string s);                                      
	/**@brief get GOBS enum (pha->snr) */
	GNUT_LIBRARY_EXPORT GOBS pha2snr(GOBS o);                      
	/**@brief get true for code obs */
	GNUT_LIBRARY_EXPORT bool gobs_code(GOBS o);                    
	/**@brief get true for phase obs */
	GNUT_LIBRARY_EXPORT bool gobs_phase(GOBS o);                   
	/**@brief static map of default GNSS satellites  */
	GNUT_LIBRARY_EXPORT t_map_sats GNSS_SATS();                                           
	/**@brief static map of default GNSS navigation types */
	GNUT_LIBRARY_EXPORT t_map_gnav GNSS_GNAV();                     
	/**@brief static map of default GNSS data types/bands/attrs priorities */
	GNUT_LIBRARY_EXPORT t_map_gnss GNSS_DATA_PRIORITY(); 	      
	/**@brief get FREQ_SEQ enum from string */
	GNUT_LIBRARY_EXPORT FREQ_SEQ str2gnssfreq(string s);  
	/**@brief get FREQ_SEQ enum from char */
	GNUT_LIBRARY_EXPORT FREQ_SEQ char2gnssfreq(char c);

     ///< static map of default GNSS freq priorities
	 const t_map_freq GNSS_FREQ_PRIORITY = {
		{GPS , { LAST_GFRQ, G01,      G02,      G05}},
		{GLO , { LAST_GFRQ, R01,      R02,      R03_CDMA, R05_CDMA}},
		{GAL , { LAST_GFRQ, E01,      E05,      E07,      E08,      E06     }},
		{BDS , { LAST_GFRQ, C02,      C07,      C06,                        }},
		{QZS , { LAST_GFRQ, J01,      J02,      J05,      J06               }},
		{SBS , { LAST_GFRQ, S01,      S05                                   }},
		{GNS , {                                                            }},
	};	 
     ///< static map of default GNSS band priorities
	 const t_map_band GNSS_BAND_PRIORITY= {
		{GPS,{ BAND, BAND_1, BAND_2, BAND_5                 }},
		{GLO,{ BAND, BAND_1, BAND_2, BAND_3, BAND_5			}},
		{GAL,{ BAND, BAND_1, BAND_5, BAND_7, BAND_8, BAND_6 }},
		{BDS,{ BAND, BAND_2, BAND_7, BAND_6,                }},
		{QZS,{ BAND, BAND_1, BAND_2, BAND_5, BAND_6         }},
		{SBS,{ BAND, BAND_1, BAND_5							}},
		{GNS,{												}},
	};	          
	


} // namespace

#endif // GOBS_H

