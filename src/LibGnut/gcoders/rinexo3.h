
#ifndef RINEXO3_H
#define RINEXO3_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: Clock RINEX encoder/decoder
  Version: $Rev:$

  2011-11-04 /JD: created
  2012-09-04 /JD: RINEX 3.02 implemented
  2012-09-27 /JD: support of multi-GNSS (phase not converted to [m])
  2013-03-08 /JD: filtering via general settings
  2014-05-03 /PV: gobj getting information from rinexo header
  2014-11-19 /JD: shared pointers
  2014-12-21 /JD: advanced request filtering
 
  Todo: header information completed 
        encoders implementation

-*/

#include <string> 
#include <vector> 

#include "gall/gallobs.h"
#include "gutils/gtime.h"
#include "gutils/gtriple.h"
#include "gutils/gsys.h"
#include "gutils/gobs.h"
#include "gutils/gtypeconv.h"
#include "gcoders/gcoder.h"
#include "gcoders/rinexo2.h"
#include "gdata/grnxhdr.h"

using namespace std;

namespace gnut {

class GNUT_LIBRARY_EXPORT t_rinexo3 : public t_rinexo2 {

 public:
  /** @brief default constructor. */
  t_rinexo3( t_gsetbase* s, string version = "", int sz = DEFAULT_BUFFER_SIZE );
  /** @brief default constructor. */
  virtual ~t_rinexo3(){};
  /** @brief decode header. */
  virtual  int decode_head(char* buff, int sz,           vector<string>& errmsg) = 0;
  /** @brief decode data. */
  virtual  int decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg) = 0;
  

 protected:
  /** @brief decode header. */
  virtual int _decode_head();
  /** @brief decode data. */
  virtual int _decode_data();
  /** @brief fill header information. */
  virtual int _check_head();                                           
  /** @brief read epoch & number of satellites, return flag. */
  virtual int _read_epoch();                                           
  /** @brief read single satellite observation types. */
  virtual int _read_obstypes(const string& sat, const string& sys);    
  /** @brief fix band (BDS). */
  virtual int _fix_band(string sys, string& go);                        

  t_rnxhdr::t_obstypes       _mapcyc;     ///< map of GOBS phase quater-cycle shifts
  t_rnxhdr::t_obstypes       _glofrq;     ///< map of GLONASS slot/frequency
  t_rnxhdr::t_vobstypes      _globia;     ///< vec of GLONASS obs code-phase biases
   

 private:

};

} // namespace

#endif
