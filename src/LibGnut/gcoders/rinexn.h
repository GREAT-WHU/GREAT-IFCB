
#ifndef RINEXN_H
#define RINEXN_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: 
  Version: $Rev:$

  2011-04-20 /JD: created

-*/


#include <vector> 
#include <sstream>

#include "gcoders/gcoder.h"
#include "gdata/grxnhdr.h"
#include "gset/gsetinp.h"
#include "gdata/gnav.h"
using namespace std;

namespace gnut {

class GNUT_LIBRARY_EXPORT t_rinexn : public t_gcoder {

 public:
  /** @brief constructor */
  t_rinexn( t_gsetbase* s, string version = "", int sz = DEFAULT_BUFFER_SIZE );
  /** @brief destructor */
  virtual ~t_rinexn(){};
  /** @brief NAV-RINEX header */
  virtual  int decode_head(char* buff, int sz,           vector<string>& errmsg);
  /** @brief NAV-RINEX body */
  virtual  int decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg);
   
 protected:         
  
 private:
   char            _gnsssys;    ///< Navigation Types
   t_gtime         _check_dt;   ///< to validate the message
   t_rxnhdr        _rxnhdr;     ///< RINEX header
   vector<string>  _comment;    ///< RINEX comments
   
};

} // namespace

#endif
