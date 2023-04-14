
#ifndef RINEXO_H
#define RINEXO_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: Clock RINEX encoder/decoder
  Version: $Rev:$

  2011-11-04 /JD: created
  2012-09-04 /JD: RINEX 3.01 implemented
  2012-09-24 /JD: RINEX 2.11 implemented
  2012-09-27 /JD: support of multi-GNSS (phase not converted to [m])
  2013-03-08 /JD: filtering via general settings
  2013-11-19 /JD: shared pointers
 
  Todo: header information completed 
        gobj getting information from rinexo header
        encoders implementation
        to finish obs-filtering via settings

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
#include "gcoders/rinexo3.h"
#include "gdata/grnxhdr.h"

using namespace std;

namespace gnut {

class GNUT_LIBRARY_EXPORT t_rinexo : public t_rinexo3 {

 public:
  /** @brief default constructor. */
  t_rinexo( t_gsetbase* s, string version = "", int sz = DEFAULT_BUFFER_SIZE );
  /** @brief default detructor. */
  virtual ~t_rinexo(){};

  /** @brief decode header. 
  *  read individual lines (tmpsize once used and consumed)
  *  read block of lines at once (tmpsize cummulated and then consumed)
  */
  virtual  int decode_head(char* buff, int sz,           vector<string>& errmsg);
  /** @brief decode data. 
  *  read block of lines for each epoch (tmpsize cummulated and then consumed)
  */
  virtual  int decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg);
  
 protected:
  /** @brief decode header. */
  virtual  int _decode_head();
  /** @brief decode data. */
  virtual  int _decode_data();

 private:

};

} // namespace

#endif
