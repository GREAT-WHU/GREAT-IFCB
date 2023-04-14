
#ifndef T_RXNHDR
#define T_RXNHDR

/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)

  Purpose: rinexn header
  Version: $Rev:$

  2018-08-03 /JD: created

-*/

#include <string>
#include <vector>
#include <map>
#include <set>

#include "gutils/gtime.h"
#include "gutils/gtriple.h"

namespace gnut {

enum TSYS_CORR { TS_NONE, TS_GAUT, TS_GPUT, TS_SBUT, TS_GLUT, TS_GPGA, TS_GLGP, TS_QZGP, TS_QZUT, TS_BDUT, TS_IRUT, TS_IRGP };
enum IONO_CORR { IO_NONE, IO_GAL,  IO_GPSA, IO_GPSB, IO_QZSA, IO_QZSB, IO_BDSA, IO_BDSB, IO_IRNA, IO_IRNB };

struct t_tsys_corr { double a0 = 0.0, a1 = 0.0; int T = 0, W = 0; };
struct t_iono_corr { double x0 = 0.0, x1 = 0.0, x2 = 0.0, x3 = 0.0; int T = 0, sat = 0; };
   
typedef map<TSYS_CORR, t_tsys_corr> t_map_tsys;
typedef map<IONO_CORR, t_iono_corr> t_map_iono;

/** @brief convert */
string tsys_corr2str(TSYS_CORR c);
/** @brief convert */
string iono_corr2str(IONO_CORR c);

/** @brief convert */
TSYS_CORR str2tsys_corr(string s);
/** @brief convert */
IONO_CORR str2iono_corr(string s);

class t_rxnhdr
{
 public:
   t_rxnhdr();
  ~t_rxnhdr();

   string                     path() const { return _path; }
   void                       path(string s){ _path = s; }
   
   char                       rxnsys(){ return _rxnsys; }
   void                       rxnsys(char s){ _rxnsys = s; }
   
   string                     rxnver() const { return _rxnver; }
   void                       rxnver(string s){ _rxnver = s; }
   
   string                     program() const { return _program; }
   void                       program(string pgm){ _program = pgm; }
   
   string                     runby() const { return _runby; }
   void                       runby(string rnb){ _runby = rnb; }
   
   t_gtime                    gtime() const { return _gtime; }
   void                       gtime(t_gtime& t){ _gtime = t; }
   
   vector<string>             comment(){ return _comment; }
   void                       comment(vector<string>& cmt) {_comment = cmt;}
   
   int                        leapsec() const {return _leapsec;}
   void                       leapsec(int ls) {_leapsec = ls;}

   set<TSYS_CORR>             tsys_corr() const;
   t_tsys_corr                tsys_corr(const TSYS_CORR c) const;
   void                       tsys_corr(const TSYS_CORR c, t_tsys_corr tc);

   set<IONO_CORR>             iono_corr() const;
   t_iono_corr                iono_corr(const IONO_CORR c) const;
   void                       iono_corr(const IONO_CORR c, t_iono_corr io);

   void clear();

   friend ostream& operator<<(ostream& os, const t_rxnhdr& x);
   
 private:   
   char                       _rxnsys;     ///< G=GPS, R=GLO, E=GAL, S=SBAS, M=Mix
   string                     _path;       ///< rinex file path
   string                     _rxnver;     ///< rinex version
   string                     _program;    ///< name of program creating RINEX file
   string                     _runby;      ///< name of agency  creating RINEX file
   t_gtime                    _gtime;      ///< name of date and file of RINEX creation
   vector<string>             _comment;    ///< vector of comments
   int                        _leapsec;    ///< leapseconds since 6-Jan-1980
   t_map_tsys                 _ts_corr;    ///< RINEX header tsys corrections
   t_map_iono                 _io_corr;    ///< RINEX header iono corrections
   
};

} // namespace

#endif
