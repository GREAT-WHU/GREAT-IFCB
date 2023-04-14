 
#ifndef GNAVGLO_H
#define GNAVGLO_H 
 
/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)

  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: 
  Version: $ Rev: $

  2011-02-14 /JD: created
  2018-08-13 /JD: updated

-*/

#include <vector>

#include "newmat/newmat.h"
#include "gdata/gnav.h"
#include "gutils/gtime.h"
#include "gutils/gconst.h"
#include "gutils/gcommon.h"
#include "gutils/gtriple.h"

#define MAX_RINEXN_REC_GLO 15

#define MIN_GLO_RADIUS 25300       // km
#define MAX_GLO_RADIUS 25700       // km
#define MAX_GLO_RADDIF 50          // km
#define MAX_GLO_CLKDIF 50          // ns
#define RAD_GLO_FACTOR 0.001       // m->km
#define CLK_GLO_FACTOR 1000000000  // sec->ns

using namespace std;

namespace gnut {

	class GNUT_LIBRARY_EXPORT  t_gnavglo : public t_gnav {

 public:
  /** @brief default constructor. */
  t_gnavglo();
  /** @brief default destructor. */
  virtual ~t_gnavglo();
  /** @brief get channnel. */
  int channel() const;
  /** @brief convert general gnavdata to gnav_glo element. */
  int data2nav( string  sat, const t_gtime& ep, const t_gnavdata& data );
  /** @brief convert gnav_glo element to general gnavdata. */
  int nav2data( t_gnavdata& data );
  /** @brief get frequency id. */
  int freq_num() const { return _freq_num; }

 protected:
   /** @brief IOD of GLONASS clocks. */
   int  _iod() const;

 private:
   /** @brief six orbital differential equations. */
   ColumnVector _deriv(const ColumnVector& xx, const t_gtriple& acc);

   double _maxEphAge;    ///< max age of ephemerises [s]
   int      _iodc;       ///< issue of clocks
   double   _x;          ///< position X [km]
   double   _x_d;        ///< velocity X [km/s]
   double   _x_dd;       ///< acceleration X [km/s^2]
   double   _y;          ///< position Y [km]
   double   _y_d;        ///< velocity Y [km/s]
   double   _y_dd;       ///< acceleration Y [km/s^2]
   double   _z;          ///< position Z [km]
   double   _z_d;        ///< velocity Z [km/s]
   double   _z_dd;       ///< acceleration Z [km/s^2]
   double   _E;          ///< Age of oper. information [days]
   int      _freq_num;   ///< frequency number (-7 ... 13)
   double   _health;     ///< health 0 = OK
   t_gtime  _toc;        ///< Epoch of clocks [s]
   double   _gamma;      ///< SV relative frequency bias []
   double   _tau;        ///< SV clock bias [s]
   double   _tki;        ///< message frame time [0 ... 86400 s]
   int      _min_step;   ///< mininal step length for Runge Kutta
   
};

} // namespace

#endif
