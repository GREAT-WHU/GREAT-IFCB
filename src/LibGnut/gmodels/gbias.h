
#ifndef GBIAS_H
#define GBIAS_H 
 
/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
 
  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements GNSS code biases
  Version: $ Rev: $

  2012-11-05 /JD: created

-*/

#include <vector>

#include "newmat/newmat.h"
#include "gdata/gdata.h"
#include "gutils/gconst.h"
#include "gutils/gtime.h"
#include "gutils/gnss.h"

using namespace std;

namespace gnut {   

// ----------
class t_gbias : public t_gdata {

   public:
   /**@brief constructor */
   t_gbias();
   /**@brief destructor */
   virtual ~t_gbias();
   /**@brief add single differential bias */
   void  set(t_gtime beg, t_gtime end, double d, GOBS obs1, GOBS obs2 = X);   
   /**@brief add single differential bias */
   void  set(double d, GOBS obs1, GOBS obs2 = X);                        
   /**@brief set reference signal */
   void  ref(GOBS ref);
   /**@brief get single code bias */
   double bias(bool meter = true);                                         
   /**@brief  get obs*/
   GOBS gobs()const{ return _gobs; }
   /**@brief  get val*/
   double val()const { return _val; }
   /**@brief  get reference obs */
   GOBS ref()  const{ return _ref;   }      
   /**@brief  set valid beg time */
   void    beg(t_gtime t){ _beg = t; }         
   /**@brief  get valid beg time */
   t_gtime beg()const{ return _beg;  }
   /**@brief  set valid end time */
   void    end(t_gtime t){ _end = t; }        
   /**@brief  get valid end time */
   t_gtime end()const{ return _end;  }
   /**@brief test validity */
   bool valid(const t_gtime& epo);
   
   private:

   t_gtime        _beg;        ///< valid from
   t_gtime        _end;        ///< valid until
   
   GOBS           _gobs;       ///< the second obs
   GOBS           _ref;        ///< ref obs
   double         _val;        ///< code biases are stored in meters
 

};

} // namespace

#endif
