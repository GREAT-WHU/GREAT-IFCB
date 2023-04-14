
#ifndef GFILE_H
#define GFILE_H

/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements file io based on gio class
  Version: $ Rev: $

  2011-01-10 /JD: created

-*/

#include <stdio.h>
#include <fstream>
#include <string>

#include "gio/gio.h"
#include "gio/giof.h"

///< special buffer size for file reading
///< must be bellow gcoder maximum limit !
#define FILEBUF_SIZE 20480
#define FILEHDR_SIZE    48

using namespace std;

namespace gnut {

class GNUT_LIBRARY_EXPORT t_gfile : public t_gio {

 public:
   /** @brief constructor  */
   t_gfile();
   /** @brief destructor  */
   virtual ~t_gfile();
   /** @brief  init write function (write header) */
   virtual int init_write();
   /** @brief  init read function (read header) */
   virtual int init_read();
   /** @brief  get irc status */
   virtual int irc()const;                  
   /** @brief  eof */
   virtual bool eof();                    
   /** @brief  mask */
   virtual string mask();                

   /** @brief  set path */
   virtual int path( string str );          
   /** @brief  get path */
   virtual string path();
   /** @brief  get name */
   virtual string name();
   /** @brief  reset path */
   virtual void reset();                   

 protected:
   /** @brief  write data  */
   virtual int _gio_write( const char* buff, int size );
   /** @brief  read data */
   virtual int _gio_read(  char* buff, int size );
   /** @brief  common function for file close */
   virtual int _stop_common();              
   /** @brief  integrate ascii */
   virtual int _read( char* b, int s);      
   /** @brief  integrate gascii */
   virtual int _write( const char* b, int s);

   int         _irc;
   t_giof*     _file;                        ///< ascii file

 private:
};

} // namespace
 
#endif
