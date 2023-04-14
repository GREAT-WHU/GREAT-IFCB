
#ifndef GPAIR_H
#define GPAIR_H
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: implements 2D coordinates representation (e.g. horizontal coordinates)
  Version: $ Rev: $

  2012-05-11 /JD: created

-*/

#include "gutils/ExportGnut.h"

#include <iostream>
#include <string.h>

#include "newmat/newmat.h"
#include "newmat/newmatio.h"

using namespace std;

namespace gnut
{

	class GNUT_LIBRARY_EXPORT t_gpair
	{

	public:
		/**@brief null constructor */
		t_gpair();
		/**@brief constructor */
		t_gpair(double x, double y);
		/**@brief constructor */
		t_gpair(double crd[2]);
		/**@brief constructor */
		t_gpair(const ColumnVector& crd);
		/**@brief destructor */
		virtual ~t_gpair();
		/**@brief assignment operator */
		t_gpair& operator=(const t_gpair& other);      
		/**@brief operator + */
		t_gpair    operator+(const t_gpair& other) const; 
		/**@brief equal operator */
		bool       operator==(const t_gpair& tr) const;  
		/**@brief operator for sorting */
		bool       operator<(const t_gpair& tr) const;
		/**@brief get a reference of element */
		double& operator[](const size_t idx);
		/**@brief get value of element */
		double     operator[](const size_t idx) const;

		/**@brief overloading << operator */
		friend ostream& operator<<(ostream& os, const t_gpair& x);
		/**@brief get single element */
		double        crd(int idx) const;                
		/**@brief set single element */
		void          set(int idx, double newValue);      
		/**@brief set array by ColumnVector */
		void          set(const ColumnVector&);          
		/**@brief set array by array */
		void          set(double crd[2]);               
		/**@brief get array */
		double* crd_array();                       
		/**@brief get ColumnVector */
		ColumnVector  crd_cvect();                       
		/**@brief get pair */
		t_gpair& crd_pair();                         
		/**@brief get unit ColumnVector */
		ColumnVector  unitary();                         
		/**@brief true: zero elements, false: not zero elements */
		bool          zero();                            

	protected:

	private:
		double         _crd[2];   ///< Two-dimensional coordinates

	};

} // namespace

#endif
