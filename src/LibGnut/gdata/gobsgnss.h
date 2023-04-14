
#ifndef  GOBSGNSS_H
#define  GOBSGNSS_H

/* ----------------------------------------------------------------------
    (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
       Research Institute of Geodesy, Topography and Cartography
       Ondrejov 244, 251 65, Czech Republic
   
   Purpose: implementation of GNSS observation element
   Version: $Rev:$
       
   2011-09-04 /JD: created
   2012-09-24 /JD: selections of code, phase etc.
   
   Todo: multi-GNSS & various LCs !
         gobj implementation

-*/

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

#include "gdata/gdata.h"
#include "gutils/gnss.h"
#include "gutils/gobs.h"
#include "gutils/gtime.h"
#include "gutils/gsys.h"
#include "gall/gallbias.h"

#define DEF_CHANNEL 255

using namespace std;

namespace gnut {  
   
const static double NULL_GOBS = 0.0;


/** @brief priority tables for choice of available signals (code [m]) */
const static GOBS code_choise[9][19] = {
   {   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X }, //
   { C1A, C1B, C1C,   X, C1I, C1L, C1M,   X, C1P, C1Q, C1S, C1W, C1X, C1Y, C1Z,  P1,  C1,  CA,  CB }, //  C1
   {   X,   X, C2C, C2D, C2I, C2L, C2M,   X, C2P, C2Q, C2S, C2W, C2X, C2Y,   X,  P2,  C2,  CC,  CD }, //  C2
   {   X,   X,   X,   X, C3I,   X,   X,   X,   X, C3Q,   X,   X, C3X,   X,   X,   X,   X,   X,   X }, //
   {   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X }, //
   {   X,   X,   X,   X, C5I,   X,   X,   X,   X, C5Q,   X,   X, C5X,   X,   X,  P5,  C5,   X,   X }, //  C5
   { C6A, C6B, C6C,   X, C6I,   X,   X,   X,   X, C6Q,   X,   X, C6X,   X, C6Z,   X,  C6,   X,   X }, //  C6
   {   X,   X,   X,   X, C7I,   X,   X,   X,   X, C7Q,   X,   X, C7X,   X,   X,   X,  C7,   X,   X }, //  C7
   {   X,   X,   X,   X, C8I,   X,   X,   X,   X, C8Q,   X,   X, C8X,   X,   X,   X,  C8,   X,   X }  //  C8
};

/** @brief priority tables for choice of available signals (phase [full-cycles]) */
const static GOBS phase_choise[9][19] = {
   {   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X }, //
   { L1A, L1B, L1C,   X, L1I, L1L, L1M, L1N, L1P, L1Q, L1S, L1W, L1X, L1Y, L1Z,   X,  L1,  LA,  LB }, //  L1
   {   X,   X, L2C, L2D, L2I, L2L, L2M, L2N, L2P, L2Q, L2S, L2W, L2X, L2Y,   X,   X,  L2,  LC,  LD }, //  L2
   {   X,   X,   X,   X, L3I,   X,   X,   X,   X, L3Q,   X,   X, L3X,   X,   X,   X,   X,   X,   X }, //
   {   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X,   X }, //
   {   X,   X,   X,   X, L5I,   X,   X,   X,   X, L5Q,   X,   X, L5X,   X,   X,   X,  L5,   X,   X }, //  L5
   { L6A, L6B, L6C,   X, L6I,   X,   X,   X,   X, L6Q,   X,   X, L6X,   X, L6Z,   X,  L6,   X,   X }, //  L6
   {   X,   X,   X,   X, L7I,   X,   X,   X,   X, L7Q,   X,   X, L7X,   X,   X,   X,  L7,   X,   X }, //  L7
   {   X,   X,   X,   X, L8I,   X,   X,   X,   X, L8Q,   X,   X, L8X,   X,   X,   X,  L8,   X,   X }  //  L8
};

class t_obscmb;

class GNUT_LIBRARY_EXPORT t_gobsgnss : public t_gdata {
   
 public:
   /** @brief constructor  */
   t_gobsgnss();
   /** @brief  constructor */
   t_gobsgnss(const string& sat);
   /** @brief  constructor */
   t_gobsgnss(const string& site, const string& sat, const t_gtime& t);
   /** @brief destructor  */
   virtual ~t_gobsgnss();
   /** @brief add a single observation  */
   void addobs(const GOBS& obs,const double& d);  
   /** @brief add a lost-of-lock indicator  */
   void addlli(const GOBS& obs,const int& i);    
   /** @brief add a range outlier indicator  */
   void addoutliers(const GOBS& obs, const int& i); 

   void addcrd(const t_gtriple& xyz) { _sat_pos = xyz; }
   t_gtriple satcrd() { return _sat_pos; }

   /** @brief get system-specific frequency  for band i  */
   void setrangestate(string name, bool b);
   bool getrangestate(string name);
   double frequency(GOBSBAND b);                                     
   /** @brief get system-specific wavelength for band i  */
   double wavelength(GOBSBAND b);                 
   /** @brief get wavelength for wild lane  */
   double wavelength_WL(GOBSBAND b1 = BAND_1,
	   GOBSBAND b2 = BAND_2);                     
   /** @brief get wavelength for narrow lane  */
   double wavelength_NL(GOBSBAND b1 = BAND_1,
	   GOBSBAND b2 = BAND_2);                     
   /** @brief get GNSS system from satellite IDOC  */
   GSYS gsys() const;                                  

   /** @brief return obs of band b  */
   GOBS id_range(GOBSBAND b){ return _id_range(b); }; 
   /** @brief return obs of band b  */
   GOBS id_phase(GOBSBAND b){ return _id_phase(b); }; 
   /** @brief get code obs of selected band   */
   GOBS select_range(GOBSBAND b) const;				   
   /** @brief get pahse obs of selected band  */									  
   GOBS select_phase(GOBSBAND b) const;				    
   /** @brief get vector of available observations  */
   vector<GOBS> obs();                                
   /** @brief get a single observation (DIFFERENT UNITS!)  */
   double getobs(const GOBS&   obs);
   /** @brief get a lost-of-lock indicator  */
   int    getlli(const GOBS& obs);
   /** @brief get a range outlier indicator  */
   int    getoutliers(const GOBS& obs);
   /** @brief  set channel number for Glonass satellite */
   void   resetobs(const GOBS& obs, double v);    // add by glfeng
   void   channel(int canal);                     
   /** @brief  get channel number for Glonass satellites */
   int    channel() const;                           
   
   /** @brief get  code observation [m] only requested type! */
   double obs_C(const t_gobs& gobs);  
   /** @brief get phase observation [m] only requested type */
   double obs_L(const t_gobs& gobs);   
   /** @brief get  code observation [m] only requested type */
   double obs_C(const t_gband& gb);    
   /** @brief get phase observation [m] only requested type */
   double obs_L(const t_gband& gb);   
   
   /** @brief get Melbourne-Wuebenna combination for phase & code [cycle]!) */
   double P3(const t_gobs& g1, const t_gobs& g2);  // get ionosphere-free combination for code  [m]
   double L3(const t_gobs& g1, const t_gobs& g2);  // get ionosphere-free combination for phase [m]
   double L4(const t_gobs& g1, const t_gobs& g2);  // get geometry-free combination for phase   [m]
   /** @brief get Geometry-Free and Ionospheric-Free combination [meter]!) */
   double GFIF_meter(const t_gobs& gL1, const t_gobs& gL2, const t_gobs& gL3);
   double MW_cycle(const t_gobs& gL1, const t_gobs& gL2,
	     const t_gobs& gC1, const t_gobs& gC2); 
   /** @brief get extra-wide-lane Melbourne-Wuebenna combination for phase & code [cycle]!) */
   double EWL_cycle(const t_gobs& gL1, const t_gobs& gL2, const t_gobs& gL3,
	   const t_gobs& gC1, const t_gobs& gC2);  
   /** @brief add bias correction on observations according to gallbias ) */
   bool   apply_bias(t_gallbias& allbias);
   /** @brief add bias correction on observations) */
   bool   apply_code_phase_bias(t_gallbias& allbias);
   /** @brief add dcb bias correction on observations) */
   bool   apply_dcb(t_gallbias& allbias);
   /** @brief get inter-frequency DCB ) */
   double interFreqDcb(t_gallbias& allbias, GOBSBAND band1, GOBSBAND band2);

   /** @brief  get reference epoch */
   t_gtime epoch() const{ return _epoch; }                      
   /** @brief  get station id  */
   string  site()  const{ return _staid; }                     
   /** @brief  get satellite id  */
   string  sat()   const{ return _satid; }                     
   /** @brief  get satellite system id  */
   string  sys()   const{ return _satid.substr(0,1); }         
   /** @brief  set satellite id  */
   void   sat(string id) { _satid = id;
                           _gsys  = t_gsys::char2gsys(id[0]); }    
   /** @brief set site id  */
   void  site(string id) { _staid = id; }                       
   /** @brief set epoch  */
   void   epo(t_gtime t) { _epoch = t;  }                          
   // clean data
   void clear();
   bool valid();  
   bool obs_empty();
      
   /** @brief  get sat health */
   bool    health(){return _health;}             
   /** @brief  set sat health */
   void    health(bool health){_health = health;} 
   
 protected:
   /** @brief get  code observation [m] only requested type NEW + AUTO!  */
   double _obs_range(const t_gobs&  go);        
   /** @brief get  code observation [m] only requested type NEW + AUTO!  */
   double _obs_range(const t_gband& gb);        
   /** @brief get phase observation [m] only requested type NEW + AUTO   */
   double _obs_phase(const t_gband& gb);        
   double _lc_range(const t_gobs* g1,   const double& cf1,      // return value [m] of general pseudo-range
                    const t_gobs* g2,   const double& cf2,      // LC = c1*O1 + ... 
               const t_gobs* g3=0, const double& cf3=0);   // for 2 or 3 bands with given coefficients  NEW !   
   
   double _lc_phase(const t_gobs* g1,   const double& cf1,      // return value [m] of general carrier-phase
               const t_gobs* g2,   const double& cf2,   // LC = c1*O1 + ... 
               const t_gobs* g3=0, const double& cf3=0);   // for 2 or 3 bands with given coefficients   NEW !
   int _coef_ionofree(GOBSBAND b1, double& c1,   // return coefficients (c1,c2) of the ionosphere-free linear combination
                      GOBSBAND b2, double& c2);  // (2 bands)
   int _coef_geomfree(GOBSBAND b1, double& c1,   // return coefficients (c1,c2) of the geometry-free linear combination
       GOBSBAND b2, double& c2);  // (2 bands)
  
   /** @brief  get  code ID of selected band (according to table) */
   GOBS _id_range(GOBSBAND b);                   
   /** @brief  get phase ID of selected band (according to table) */
   GOBS _id_phase(GOBSBAND b);                   
                
   // clean internal function
   virtual void _clear();
   virtual bool _valid() const;
   virtual bool _valid_obs() const;

   map<GOBS, double>   _gobs;     ///< maps of observations
   map<GOBS, int>      _glli;     ///< maps of lost-of-lock identifications
   map<GOBS, int>      _goutlier; ///< maps of gross error of range observations
   								  
   string              _staid;    ///< station id
   string              _satid;    ///< satellite id ["G??", "R??", "E??" ...]
   GSYS                _gsys;     ///< system 
   t_gtime             _epoch;    ///< epoch of the observation
   int                 _channel;  ///< satellite channel number
   bool                _health;   ///< healthy or not
   t_gtriple           _sat_pos;
 private:
	 bool              _bds_code_bias_mark;    ///< true, correct; false, not correct
	 bool              _dcb_correct_mark;      ///< mark for correct dcb; true: dcb corrected; false: not corrected; 
   
};

class t_obscmb
{
 public:
   t_obscmb(){num = 0.0; lam = 0.0;};
   double num;
   double lam;
   t_gobs first;
   t_gobs second;
   bool operator<(const t_obscmb& t) const;
};

} // namespace

#endif
