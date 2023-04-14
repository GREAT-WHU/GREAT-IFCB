
#ifndef GNAV_H
#define GNAV_H 
 
/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
  
  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements ephemerides and navigation classes
  Version: $ Rev: $

  2011-01-10 /JD: created

  TODO: overwritting mode
        return first/last
        limit for number of data inclusions per satellites
-*/

#include "gdata/geph.h"
#include "gutils/gsys.h"
#include "gutils/gtime.h"

#define MAX_RINEXN_REC     29      // maximum number of RINEXN records for any system !!

#define MAX_NAV_TIMEDIFF  3600*2    // NAV GNS valitity interval [s]
#define MAX_GPS_TIMEDIFF  3600*4.5  // NAV GPS validity interval [s]
#define MAX_GLO_TIMEDIFF  60*17		// NAV GLO validity interval [s]
#define MAX_GAL_TIMEDIFF  3600*3    // NAV GAL validity interval [s]
#define MAX_BDS_TIMEDIFF  3600      // NAV BDS validity interval [s]

using namespace std;

namespace gnut {

typedef double t_gnavdata[MAX_RINEXN_REC];

class  GNUT_LIBRARY_EXPORT t_gnav : public t_geph {

 public:
  /** @brief default constructor. */
  t_gnav();
  /** @brief default destructor. */
  virtual ~t_gnav();
  static  int nav_validity( GSYS gs );  // get GNSS NAV validity (half-interval) [s]
  /** @brief convert general gnavdata to gnav_glo element. */
  virtual int data2nav( string sat, const t_gtime& ep, const t_gnavdata& data ){ return -1; }
  /** @brief convert gnav_glo element to general gnavdata. */
  virtual int nav2data(                                      t_gnavdata& data ){ return -1; }
  /** @brief IOD of GLONASS clocks. */
  virtual int iod() const { return -1; }
  /** @brief number of RINEXN records */
  virtual int rec() const { return MAX_RINEXN_REC; }
  /** @brief healthy of Navigation. */
  virtual bool healthy() const;
  /** @brief relate to navigation messages. */
  virtual int chk(set<string>& msg){ return 1; }
  /** @brief get frequency id. */
  virtual int freq_num() const { return 255; }

 protected:
  /** @brief healthy of Navigation. */
  virtual bool _healthy() const { return true; }

 private:

};

} // namespace

#endif
