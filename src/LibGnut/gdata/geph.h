
#ifndef GEPH_H
#define GEPH_H 
 
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

#include <memory>

#include "gio/gio.h"
#include "gdata/gdata.h"
#include "gutils/gsys.h"
#include "gutils/gtime.h"

using namespace std;
    
namespace gnut {
   
class GNUT_LIBRARY_EXPORT t_geph : public t_gdata {

 public:
  /** @brief default constructor. */
  t_geph();
  /** @brief default destructor. */
  virtual ~t_geph();
  /** @brief health check. */
  virtual int clk( const t_gtime& t, double*    clk, double*   var = NULL, double*  dclk = NULL, bool chk_health = true ){ return -1; }  // [s]
  virtual int pos( const t_gtime& t, double  xyz[3], double var[3] = NULL, double vel[3] = NULL, bool chk_health = true ){ return -1; }  // [m]
  virtual bool healthy() const { return true; }
  /** @brief return nav type. */
  virtual GNAVTYPE gnavtype(bool full = true) const { return NAV; }

  virtual string linefmt() const { return ""; }

  /** @brief clean internal function. */
  void clear();
  /** @brief check valid. */
  bool valid();
  /** @brief GNSS system. */
  GSYS    gsys() const;                                       
  /** @brief satellite number. */
  string  gsat() const;                                   

  // POZDEJI JEN GSAT a vse pres MUTEX !!!
  /** @brief satellite number. */
  string  sat()      const{ return _sat; }                          
  /** @brief get validity interval. */
  double  interval() const{ return _interval; }                  
  /** @brief reference epoch. */
  t_gtime epoch()    const{ return _epoch; }                      
  /** @brief beg of validity. */
  t_gtime begin()    const{ return _epoch - _interval/2; }       
  /** @brief end of validity. */
  t_gtime end()      const{ return _epoch + _interval/2; }       
  virtual bool chktot(const t_gtime& t) { return true; }
   
 protected:
  /** @brief clean internal function. */
  virtual void _clear();
  /** @brief check valid. */
  virtual bool _valid() const;

  string            _sat;          ///< satellite number
  t_gtime           _epoch;        ///< reference epoch
  double            _interval;     ///< validity interval
  bool              _validity;     ///< validity
   
  shared_ptr<t_gio> _gio_ptr;

 private:

};

} // namespace

#endif
