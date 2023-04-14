
#ifndef GTRIPLE_H
#define GTRIPLE_H 
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements vector of 3D (e.g. coordinates)
  Version: $ Rev: $

  2012-05-11 /JD: created

-*/
#include "gutils/ExportGnut.h"

#include <iostream>
#include <string.h>
#include "gutils/gpair.h"

#include "newmat/newmat.h"
#include "newmat/newmatio.h"

using namespace std;

namespace gnut
{

	class GNUT_LIBRARY_EXPORT  t_gtriple
	{

	public:
		/**@brief null constructor */
		t_gtriple();
		/**@brief constructor */
		t_gtriple(double x, double y, double z);
		/**@brief constructor */
		t_gtriple(double crd[3]);
		/**@brief constructor */
		t_gtriple(const ColumnVector& crd);
		/**@brief destructor */
		virtual ~t_gtriple();
		/**@brief assignment operator */
		t_gtriple& operator=(const t_gtriple& other);    
		/**@brief operator += */
		t_gtriple& operator+=(const t_gtriple& other);
		/**@brief operator -= */
		t_gtriple& operator-=(const t_gtriple& other);
		/**@brief operator *= */
		t_gtriple& operator*=(const double& x);
		/**@brief operator /= */
		t_gtriple& operator/=(const double& x);
		/**@brief operator + */
		t_gtriple  operator+(const t_gtriple& other) const;
		/**@brief operator - */
		t_gtriple  operator-(const t_gtriple& other) const;
		/**@brief operator * */
		t_gtriple  operator*(const double& x) const;
		/**@brief operator / */
		t_gtriple  operator/(const double& x) const;
		/**@brief equal operator */
		bool       operator==(const t_gtriple& tr) const; 
		/**@brief not equal operator */
		bool       operator!=(const t_gtriple& tr) const; 
		/**@brief operator for sorting */
		bool       operator<(const t_gtriple& tr)  const; 
		/**@brief get a reference of element */
		double& operator[](const size_t idx);           
		/**@brief get value of element */
		double     operator[](const size_t idx) const;    
		/**@brief overloading << operator */
		friend ostream& operator<<(ostream& os, const t_gtriple& x);
		/**@brief get single element */
		double        crd(int idx) const;                
		/**@brief Eukl. norm */
		double        norm() const;                    
		/**@brief Sum of items */
		double        sum() const;                       
		/**@brief set single element */
		void          set(int idx, double newValue);     
		/**@brief set array by ColumnVector */
		void          set(const ColumnVector&);          
		/**@brief set array by array */
		void          set(double crd[3]);           
		/**@brief get array */
		double* crd_array();                        
		/**@brief get ColumnVector */
		ColumnVector  crd_cvect();                  
		/**@brief get triple */
		t_gtriple& crd_tripl();                     
		/**@brief get unit ColumnVector */
		ColumnVector  unitary();                    
		/**@brief cut to gpair class */
		t_gpair       gpair() const;
		/**@brief true: zero elements, false: not zero elements */
		bool          zero();                            


	protected:

	private:
		double         _crd[3];     ///< Third-dimensional coordinates

	};

} // namespace

#endif
