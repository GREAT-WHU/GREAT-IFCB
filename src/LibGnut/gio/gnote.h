
#ifndef GNOTE_H
#define GNOTE_H

/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements motes (messages,warning,errors)
  Version: $ Rev: $

  2017-08-04 /JD: created

-*/

#include <map>
#include <set>
#include <string>
#include <memory>
#include <stdio.h>
#include <fstream>

#include "gio/glog.h"
#include "gutils/gmutex.h"

#define BUF_SIZE 1024

using namespace std;

namespace gnut {

enum t_note{ GERROR, GWARNING, GMESSAGE };

class GNUT_LIBRARY_EXPORT t_gnote {
   
 public:
   /**@brief constructor */
   t_gnote(t_note n, string f, string s);
   /**@brief destructor */
   virtual ~t_gnote();
   
   string str()const{ return _str()+_text; }
   string note()const{ return _str(); }
   string text()const{ return _text; }
   string func()const{ return _func; }
   t_note status()const{ return _stat; }

   /**@brief overloading </==/<< operator*/
   bool            operator<(const t_gnote& n) const;
   bool            operator==(const t_gnote& n) const;
   friend ostream& operator<<(ostream& os, const t_gnote& n);

 protected:
   /**@brief get string */
   virtual string _str()const;
   
   string  _func;          ///< note function
   string  _text;          ///< note text
   t_note  _stat;          ///< note status

 private:
     
};

   
/**@brief container for gallnotes, should not be derived from gdata as others */  
class t_gallnote {

 public:
  /**@brief constructor */
  t_gallnote();
  /**@brief destructor*/
  virtual ~t_gallnote();
  /**@brief set notes (messages/warning/errors) */
  void mesg(t_note note, string func, string text);   
  /**@brief get notes (messages/warning/errors) */
  vector<t_gnote> mesg();
   
  void clear();
   
 protected:

  mutable t_gmutex _gmutex;
  vector<t_gnote> _gnotes;                     ///< cummulate notes message/warning/error

};

} // namespace

#endif
