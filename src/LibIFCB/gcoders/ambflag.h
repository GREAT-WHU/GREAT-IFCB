/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		ambflag.h
* @brief	decode and enconde ambflag file(XXXXddd0.yyo.log/log13)
*				XXXX  ---- SITE name
*				 ddd  ---- Doy of the file
*				  yy  ---- year
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
* Following is an example for the head of ambflag file.
*
* @verbatim
      ================================================================================
     |     GREAT-TURBOEDIT                         03-May-2020    SOFTWARE / DATE     |
     |GOP7                                                        STATION             |
     |GPS    L1    L2                                             SYS / FREQ1 / FREQ2 |
     |GAL    E1   E5a                                             SYS / FREQ1 / FREQ2 |
     |GLO    G1    G2                                             SYS / FREQ1 / FREQ2 |
     |BDS    B1    B2                                             SYS / FREQ1 / FREQ2 |
     |  2017  07  29  00  00  00.00           GPST                BEGIN TIME          |
     |  2017  07  29  23  59  59.00           GPST                END TIME            |
     |     30.00                                                  INTERVAL            |
     |      													  END OF HEADER       |
	  ================================================================================
@endverbatim
*
* Following is an example for the body of ambflag file.
*
* @verbatim
		================================================================================
	   ! identify prn      begin_epoch    end_epoch     cause of ambiguity
		================================================================================
		AMB   C05       1    2880    RN_biggap
		BAD   C07      59      59    RN_lessthan4obs
@endverbatim
*
*/
#ifndef AMBFLAG_H
#define AMBFLAG_H

#include "gutils/ExportIFCB.h"

#include "gcoders/gcoder.h"
#include "gall/gallambflag.h"

using namespace std;
using namespace gnut;

namespace great
{

	/**
	*@brief	   Class for decode/encode ambflag file
	*/
	class IFCB_LIBRARY_EXPORT t_ambflag : public t_gcoder
	{

	public:

		/**
		* @brief constructor.
		* @param[in]  s				  setbase control
		* @param[in]  ambflag_version version of ambflag
		* @param[in]  version		  version of the gcoder
		* @param[in]  sz			  size of the buffer
		*/
		t_ambflag(t_gsetbase* s,t_gdata::ID_TYPE ambflag_version,string version = "", int sz = DEFAULT_BUFFER_SIZE);

		/** @brief default destructor. */
		virtual ~t_ambflag();

		/**
		* @brief decode header of ambflag file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[out]  errmsg      error message of the data decoding
		* @return 0/ -1
		*/
		virtual  int decode_head(char* buff, int sz, vector<string>& errmsg)override;

		/**
		* @brief decode data body of ambflag file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[out]  errmsg      error message of the data decoding
		* @return consume size for data body decoding
		*/
		virtual  int decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)override;


	protected:
		t_gdata::ID_TYPE	_ambflag_version;   ///< record the version of ambflag(AMBFLAG or AMBFLAG13)
		ambflag_hd			_ambflag_head;		///< map for storaging ambflag data header
		ambflag_data		_ambflag_data;		///< map for storaging ambflag data body
		int					_max_epo;			///< max epoch begin
	private:

	};

} // namespace

#endif
