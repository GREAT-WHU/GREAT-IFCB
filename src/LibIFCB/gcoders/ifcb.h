/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
-1.1 hjzheng  2020-06-09  Adding ifcb encode  function
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		ifcb.h
* @brief	decode and enconde ifcb file
* @author   bwang,hjzheng Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
* Following is an example for the body of ifcb file.
*
* @verbatim
        ====================================================================
       ! mark        mjd       second
        ====================================================================
        EPOCH-TIME   58689     240.0
        ====================================================================
       ! prn            value sigma        point number
        ====================================================================
		 G01            -0.001     0.006   53
	    xG02             0.000 10000.000    0
@endverbatim
*
*/
#ifndef IFCB_H
#define IFCB_H

#include <string> 
#include <vector> 

#include "gcoders/gcoder.h"
#include "gdata/gifcb.h"

using namespace std;
using namespace gnut;

namespace great {

	/**
	*@brief	   Class for decode/encode ifcb file
	*/
	class IFCB_LIBRARY_EXPORT t_ifcb : public t_gcoder {

	public:

		/**
		* @brief constructor.
		* @param[in]  s        setbase control
		* @param[in]  version  version of the gcoder
		* @param[in]  sz       size of the buffer
		*/
		t_ifcb(t_gsetbase* s, string version = "", int sz = DEFAULT_BUFFER_SIZE);

		/** @brief default destructor. */
		virtual ~t_ifcb();

		/**
		* @brief decode header of ifcb file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[in]  errmsg      error message of the data decoding
		* @return consume size of header decoding
		*/
		virtual  int decode_head(char* buff, int sz, vector<string>& errmsg)override;

		/**
		* @brief decode data of ifcb file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[in]  errmsg      error message of the data decoding
		* @return consume size of header decoding
		*/
		virtual  int decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)override;

		/**
		* @brief encode header of ifcb file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[in]  errmsg      error message of the data decoding
		* @return  size of header encoding
		*/
		virtual  int encode_head(char* buff, int sz, vector<string>& errmsg)override;

		/**
		* @brief encode data of ifcb file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[in]  errmsg      error message of the data decoding
		* @return  size of data body encoding
		*/
		virtual  int encode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)override;

	protected:
		string _ifcbmode;  ///< ifcb mode EWL,WL and NL.
		t_gtime _epoch;   ///< current epoch
	private:

	};

} // namespace

#endif
