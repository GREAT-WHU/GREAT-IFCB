
#ifndef GDATA_H
#define GDATA_H 
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: 
  Version: $Rev:$

  2011-03-25 /JD: created

-*/

#include <sstream>
#include <iostream>

#include "gdata/gmonit.h"
#include "gutils/gmutex.h"
#include "gio/glog.h"
#include "gio/gnote.h"
#include "gutils/gcommon.h"

using namespace std;

namespace gnut {

class GNUT_LIBRARY_EXPORT t_gdata : public t_gmonit {

 public:
  /** @brief constructor */
  t_gdata();
  /** @brief copy constructor */
  t_gdata( const t_gdata& data );
  /** @brief destructor */
  virtual ~t_gdata();
  /** @brief assignment operator */
  t_gdata& operator=( const t_gdata& data );
     
  enum ID_TYPE {
    NONE,    ///< none
       
    OBS,     ///< obseravation base
    OBSGNSS, ///< gnss observations
    
    EPH,     ///< navigation base
    EPHGPS,  ///< navigation
    EPHGLO,  ///< navigation
    EPHGAL,  ///< navigation
    EPHQZS,  ///< navigation
    EPHBDS,  ///< navigation       
    EPHSBS,  ///< navigation 
    EPHIRN,  ///< navigation        

    ALLNAV,      ///< all navigation all
    ALLOBS,      ///< all observations
    ALLBIAS,     ///< all PROD    

    BIAS,        ///< code & phase biases

	AMBFLAG,	 ///< ambiguity flag
	AMBFLAG13,   ///< ambigutiy flag 13
	IFCB,		 ///< ifcb
	SATPARS,     ///< sat_parameters_new file  
	LAST
  };

  enum ID_GROUP {
    GRP_NONE,    ///< none
    GRP_OBSERV,  ///< observations
    GRP_EPHEM,   ///< ephemerides
    GRP_MODEL,   ///< models
    GRP_LAST
  };

  /** @brief set glog pointer */
  void glog(t_glog* l){ _log = l; }                 
  /** @brief get glog pointer */
  t_glog* glog()const{ return _log; }
  /** @brief set gallnote pointer */
  void gnote(t_gallnote* n){ _gnote = n; }         
  /** @brief get gallnote pointer */
  t_gallnote* gnote()const{ return _gnote; }
  /** @brief get data type */
  ID_TYPE  id_type()const{  return _type; }
  /** @brief get group type */
  ID_GROUP id_group()const{ return _group; }
  /** @brief get data type */
  string  str_type()const;
  /** @brief get group type */
  string  str_group()const;
  /** @brief ID_TYPE to string */
  static string type2str(ID_TYPE type);


 protected:
  /** @brief set data type */
  int id_type(  ID_TYPE  t );
  /** @brief set group type */
  int id_group( ID_GROUP g );

  mutable t_gmutex  _gmutex;
  t_glog*           _log;
  t_gallnote*       _gnote;
  ID_TYPE           _type;
  ID_GROUP          _group;
   
 private:

};

} // namespace

#endif
