
#ifndef BIABERNESE_H
#define BIABERNESE_H
 
/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
  
  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: bias SINEX
  Version: $Rev:$

  2018-03-12 /PV: created
  2019-04-08 glfeng modified: add decode_data for BSX file from CAS & change the function combination

-*/ 

#include "gcoders/gcoder.h"
#include "gutils/gtime.h"
#include "gall/gallbias.h"
#include "gutils/ExportGnut.h"

using namespace std;

namespace gnut {

class GNUT_LIBRARY_EXPORT t_biabernese : public t_gcoder {

 public:
   /** @brief default constructor. */
   t_biabernese( t_gsetbase* s, string version, int sz = DEFAULT_BUFFER_SIZE, string id = "biabernese" );
   /** @brief default destructor. */
   virtual ~t_biabernese(){};
   /** @brief decode_head. */
   virtual  int decode_head(char* buff, int bufLen,           vector<string>& errmsg);
   /** @brief decode_data. */
   virtual  int decode_data(char* buff, int bufLen, int& cnt, vector<string>& errmsg);
   
 protected:
   /** @brief decode CODE DCB files. */
   int _decode_data_CODE(char* buff, int sz, int& cnt, vector<string>& errmsg); 
   /** @brief decode sinex BIAS files. */
   int _decode_data_sinex(char* buff, int sz, int& cnt, vector<string>& errmsg);
   /** @brief decode version 0.01 sinex BIAS files. */
   int _decode_data_sinex_0(char* buff, int sz, int& cnt, vector<string>& errmsg);

   t_gallbias* _allbias;		///< store bias
   t_gtime _beg;				///< valid begin time   
   t_gtime _end;				///< valid end time
   string  _ac;					///< analysis center

   double _version;				///< version of sinex bias file
   bool _is_bias = false;		///< read from sinex bias file or not
   bool _is_absolute = false;	///< absolute bias or not
};

} // namespace

#endif
