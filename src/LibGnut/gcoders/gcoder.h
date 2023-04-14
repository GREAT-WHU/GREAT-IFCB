
#ifndef GCODER_H
#define GCODER_H 
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: 
  Version: $Rev:$

  2011-04-20 /JD: created
  2013-03-07 /JD: general settings supported

-*/

#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <memory>
#ifdef BMUTEX
#include <boost/thread/mutex.hpp>
#endif

#include "gio/gio.h"
#include "gio/gnote.h"
#include "gdata/gdata.h"
#include "gutils/gsys.h"
#include "gutils/gmutex.h"
#include "gutils/gcommon.h"
#include "gset/gsetbase.h"
#include "gset/gsetgnss.h"
#include "gset/gsetgen.h"
#include "gcoders/gcoder_buffer.h"

#define BUFFER_INCREASE_FAC  1.5
#define DEFAULT_BUFFER_SIZE  4096
#define MAXIMUM_BUFFER_SIZE  240000 // 9000000
using	 namespace std;
namespace gnut {



using namespace std;

class t_gio;
class GNUT_LIBRARY_EXPORT t_gcoder {

 public:
   /** @brief default constructor. */
   t_gcoder();
   /** @brief constructor. */
   t_gcoder(t_gsetbase* s, string version = "", int sz = DEFAULT_BUFFER_SIZE, string id = "gcoder" );
   /** @brief default destructor. */
   virtual ~t_gcoder();

   /** @brief clear menory. */
   virtual void clear();
   /** @brief decode header. */
   virtual int decode_head( char* buff, int sz,           vector<string>& errmsg ){ return 0; } 
   /** @brief decode data. */
   virtual int decode_data( char* buff, int sz, int& cnt, vector<string>& errmsg ){ return 0; } 
   /** @brief encode header. */
   virtual int encode_head( char* buff, int sz,           vector<string>& errmsg ){ return 0; } 
   /** @brief encode data. */
   virtual int encode_data( char* buff, int sz, int& cnt, vector<string>& errmsg ){ return 0; } 

   /** @brief set glog pointer. */
   void glog(t_glog* l){ _log = l; }
	/** @brief get glog pointer. */
   t_glog* glog(){ return _log; }

   /** @brief set path. */
   void path(string s);
   /** @brief get path. */
   string path(){ return _fname; }
   /** @brief add specific data structure to the coder. */
   int add_data( string data_id, t_gdata* data );

   /** @brief set notes (messages/warning/errors). */
   void            mesg(t_note m, string s);
   /** @brief get notes (messages/warning/errors). */
   vector<t_gnote> mesg();

   
 protected:
   /** @brief get end position of buffer. */
   int   endpos(){ return _endpos; }
   /** @brief number of char elements in buffer (excl \0). */
   int     size(){ return _endpos; } 
   /** @brief get buffer point. */
   char* buffer(){ return _buffer; }

   /** @brief get settings (NO MORE PUBLIC, only via CONSTRUCT). */
   virtual int  _gset(t_gsetbase* s);
   /** @brief initialize. */
   virtual void _init();
   /** @brief add specific data structure to the coder. */
   virtual void _add_data(string id,t_gdata* data){};
   /** @brief sampling filter for epochs (return true if the epoch fits sampling). */
   virtual bool _filter_epoch(const t_gtime& epo);
   /** @brief GNSS/sat filter (return true if the epoch fits gnss & sat). */
   virtual bool _filter_gnss(const string& prn);
   /** @brief get single line from the buffer. */
   int _getline(string& str, int from_pos = 0);
   /** @brief cummulate buffer. */
   int _add2buffer( char* buff, int sz );
   /** @brief remove from buffer. */
   int _consume( int bytes_to_eat );

   weak_ptr<t_gio>         _gio_ptr;
   vector<t_gnote>         _notes;               ///< cummative notes message/warning/error
   string                  _fname;               ///< decoded file
   string                  _class;               ///< string for reporting
   bool                    _initialized;         ///< if initialized
   bool                    _gnss;                ///< if gnss definition is requested
   int                     _out_len;             ///< [min] encoder data batch length
   float                   _out_smp;             ///< [sec] encoder data batch sample
   t_gtime                 _out_epo;             ///< encoder refrence epoch
   string                  _version;             ///< format version
   string                  _initver;             ///< format initial version
   map<string,t_gdata*>    _data;                ///< data pointer
   t_glog*                 _log;                 ///< log pointer
   t_gsetbase*             _set;                 ///< set pointer
   char*                   _buffer;              ///< class buffer
   int                     _buffsz;              ///< size of buffer
   int                     _endpos;              ///< after last position
   int					   _begpos;				 ///< begin position of useful data
   int                     _irc;                 ///< IRC code   
   bool                    _close_with_warning;  ///< close with warnings (desctructor)
   

   gcoder_char_buffer	       _decode_buffer;

   // settings
   t_gtime     _beg;                             ///< default beg time
   t_gtime     _end;                             ///< default end time
   double      _int;                             ///< default interval
   int         _scl;                             ///< default scaling for decimation (if greater or equal to 1Hz)
   set<string> _sys;                             ///< default systems
   set<string> _rec;                             ///< default sites/receivers
   map<GSYS,set<string> > _sat;                  ///< default satellites
   map<GSYS,set<string> > _obs;                  ///< default observations (all mixed for single GNSS !!)
   map<GSYS,set<string> > _nav;                  ///< default navigation messages

   // ENCODING
   /** @brief remove specific data structure from the coder. */
   int _fill_buffer( char* buff, int sz );
   stringstream     _ss;
   long             _ss_position;
   string           _pgm;
   bool             _hdr;
   
#ifdef BMUTEX
   boost::mutex            _mutex;               // buffer mutex
#else
  t_gmutex                 _mutex;               // buffer mutex
#endif
 private:

};

} // namespace

#endif
