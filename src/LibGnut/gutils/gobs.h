
#ifndef GOBS_H
#define GOBS_H

/* ----------------------------------------------------------------------
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)

  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
	  Research Institute of Geodesy, Topography and Cartography
	  Ondrejov 244, 251 65, Czech Republic

  Purpose: definition of GNSS observation types
  Version: $ Rev: $

  2012-09-26 /JD: created

-*/

#include <map>
#include <list>
#include <vector>
#include <string>
#ifdef BMUTEX
#include <boost/thread/mutex.hpp>
#endif

#include "gutils/gnss.h"
#include "gutils/ExportGnut.h"

using namespace std;

namespace gnut {

	class GNUT_LIBRARY_EXPORT t_gattr {

	public:
		/**@brief constructor */
		t_gattr() { _gattr = ATTR; };
		/**@brief constructor */
		t_gattr(GOBSATTR a) { _gattr = a; };
		/**@brief destructor */
		~t_gattr() {};
		/**@brief set attr */
		virtual void attr(const GOBSATTR& a);            
		/**@brief get attr */
		virtual GOBSATTR attr()const;                    
		/**@brief operator */
		virtual bool operator==(const t_gattr& g)const;
		/**@brief valid */
		virtual bool valid()const;

	protected:
		GOBSATTR _gattr;
	};



	class GNUT_LIBRARY_EXPORT t_gband : public t_gattr {

	public:
		/**@brief constructor */
		t_gband() :t_gattr() { _gband = BAND; };
		/**@brief constructor */
		t_gband(GOBSBAND b, GOBSATTR a) :t_gattr(a) { _gband = b; };
		/**@brief destructor */
		virtual ~t_gband() {};
		/**@brief set band */
		virtual void band(const GOBSBAND& g);             
		/**@brief get band */
		virtual GOBSBAND band()const;                     
		/**@brief set t_gattr */
		virtual void gattr(const t_gattr& g);             
		/**@brief get t_gattr */
		virtual t_gattr gattr()const;                     
		/**@brief operators */
		virtual bool operator==(const t_gband& g)const;
		/**@brief valid */
		virtual bool valid()const;

	protected:
		GOBSBAND _gband;
	};


	class GNUT_LIBRARY_EXPORT t_gobs : public t_gband {

	public:
		/**@brief constructor */
		t_gobs() :t_gband() { _gtype = TYPE; };
		/**@brief constructor */
		t_gobs(GOBSTYPE t, GOBSBAND b, GOBSATTR a) :t_gband(b, a) { _gtype = t; };
		/**@brief constructor */
		t_gobs(GOBS g) { gobs(g); };
		/**@brief destructor */
		virtual ~t_gobs() {};
		/**@brief set type */
		virtual void type(const GOBSTYPE& t);              
		/**@brief get type */
		virtual GOBSTYPE type()const;                       
		/**@brief set gband */
		virtual void gband(const t_gband& g);              
		/**@brief get gband */
		virtual t_gband gband()const;                       
		/**@brief set from GOBS */
		int gobs(const GOBS& g);                           
		/**@brief  set from string */
		int gobs(const string& s);
		/**@brief get GOBS enum */
		GOBS gobs()const;                                 
		/**@brief  change obs 2.xx to 3.xx */
		void gobs2to3(GSYS gs);							
		/**@brief operator */
		bool operator==(const t_gobs& g)const;
		/**@brief valid or not */
		bool valid()const;
		/**@brief get true if code observation */
		bool is_code()const;
		/**@brief get true if phase observation */
		bool is_phase()const;

	protected:

		GOBSTYPE          _gtype;   ///< obs type
	};



} // namespace

#endif // GOBS_H

