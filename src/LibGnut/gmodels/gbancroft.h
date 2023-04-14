
#ifndef GBANCROFT_H
#define GBANCROFT_H
 
/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
  
  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: statistical function (1D)
  Version: $ Rev: $

  2014-04-18 /PV: created
  2018-09-28 /JD: revised

-*/

#include "newmat/newmatap.h"

#include "gall/gallobs.h"
#include "gall/gallnav.h"
#include "gutils/gtriple.h"
#include "gutils/gmutex.h"
#include "gset/gsetbase.h"

namespace gnut {   

	GNUT_LIBRARY_EXPORT int gbancroft(const Matrix& BBpass, ColumnVector& pos);

	GNUT_LIBRARY_EXPORT inline double lorentz(const ColumnVector& aa, const ColumnVector& bb);

} // namespace

#endif

