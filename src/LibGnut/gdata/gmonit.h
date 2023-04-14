
#ifndef GMONIT_H
#define GMONIT_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: 
  Version: $Rev:$

  2011-03-25 /JD: created

-*/

#include <string>
#include <sstream>
#include "gutils/ExportGnut.h"

using namespace std;

namespace gnut {

class  GNUT_LIBRARY_EXPORT  t_gmonit {

 public:
  /** @brief constructor */
  t_gmonit( string id );
  /** @brief destructor */
  virtual ~t_gmonit();
  /** @brief basic class for monitoring purpose */
  virtual void  show( ostringstream& os, int verb );

 protected:
  string   _moni_id;

 private:

};

} // namespace

#endif
