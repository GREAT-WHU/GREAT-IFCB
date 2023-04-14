/**
*
* @verbatim
History
-1.0 BoWong  2019-02-22  creat the file.
-1.1 BoWong  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		ifcb.cpp
* @brief	decode and enconde ifcb file
*
* @author   BoWong, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
*/

#include "gcoders/ifcb.h"

using namespace std;
namespace great {

	t_ifcb::t_ifcb(t_gsetbase* s, string version, int sz)
		:t_gcoder(s, version, sz)
	{
		gtrace("t_ifcb::constructor");
	}

	/** @brief destructor. */
	t_ifcb::~t_ifcb()
	{

	}

	int t_ifcb::decode_head(char* buff, int sz, vector<string>& errmsg)
	{
		gtrace("t_ifcb::decode_head");
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();

		if (t_gcoder::_add2buffer(buff, sz) == 0)
		{
			_mutex.unlock();
			return 0;
		};
#ifdef DEBUG
		cout << " BUFFER : \n" << _buffer << "\n size = " << sz << " END OF BUFFER \n\n"; cout.flush();
#endif

		_mutex.unlock();
		return -1;
	}

	int t_ifcb::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();

		if (t_gcoder::_add2buffer(buff, sz) == 0)
		{
			_mutex.unlock();
			return 0;
		};
#ifdef DEBUG
		cout << " BUFFER : \n" << _buffer << "\n size = " << sz << " END OF BUFFER \n\n"; cout.flush();
#endif
		string tmp;
		int tmpsize = 0;
		int consume = 0;
		string str;// no use

		int mjd;
		double sod;

		string prn;
		t_ifcbrec ifcb;
		try
		{
			while ((tmpsize = t_gcoder::_getline(tmp, 0)) >= 0)
			{
				istringstream istr(tmp);
				consume += tmpsize;
				if (tmp.substr(1, 10) == "EPOCH-TIME")
				{
					istr >> str >> mjd >> sod;
					_epoch.from_mjd(mjd, sod, sod - floor(sod));
				}
				else if (tmp.substr(0, 1) == "x" || tmp.substr(0, 1) == "X")
				{
					//cerr << "warning: data can not use :" << tmp << endl;
				}
				else if (tmp.substr(0, 1) == " ")
				{
					istr >> prn >> ifcb.value >> ifcb.sigma >> ifcb.npoint;
					//fill data loop
					map<string, t_gdata*>::iterator it = _data.begin();
					while (it != _data.end()){
						if (it->second->id_type() == t_gdata::IFCB)
							((t_gifcb*)it->second)->add_sat_ifcb(_epoch, prn, ifcb);
						it++;
					}
				}
				else if (tmp.substr(0, 3) == "EOF")
				{
					if (_log)
					{
						_log->comment(2, "t_ifcb::decode_data", "WARNING: End of file" + tmp);
					}
					else
					{
						cout << "WARNING: t_ifcb::decode_data , End of file" << tmp << endl;;
					}
					t_gcoder::_consume(tmpsize);
					_mutex.unlock();
					return -1;
				}
				else
				{
					if (_log)
					{
						_log->comment(2, "t_ifcb::decode_data", "WARNING: unknown ifcb-data message" + tmp);
					}
					else
					{
						cout << "WARNING: t_ifcb::decode_data , unknown ifcb-data message" << tmp << endl;;
					}
					t_gcoder::_consume(tmpsize);
					_mutex.unlock();
					return -1;
				}
				t_gcoder::_consume(tmpsize);
			}
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_ifcb::decode_data", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_ifcb::decode_data , unknown mistake" << endl;
			}
			return -1;
			throw(-1);
		}
		_mutex.unlock();
		return consume;
	}

	int t_ifcb::encode_head(char* buff, int sz, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();
		try
		{
			if (_ss_position == 0)
			{
				_ss << "% IFCB generated using GREAT-IFCB" << endl;
			}
			int size = _fill_buffer(buff, sz);
			_mutex.unlock();
			return size;
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_ifcb::encode_head", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_ifcb::encode_head , unknown mistake" << endl;
			}
			return -1;
			throw(-1);
		}
	}

	int t_ifcb::encode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();
		try
		{
			if (_ss_position == 0)
			{
				t_gtime valid_beg;
				//get data from _data
				map<t_gtime, one_epoch_ifcb> ifcb;
				auto it = _data.begin();
				for (it = _data.begin(); it != _data.end(); ++it)
				{
					if (it->second->id_type() == t_gdata::IFCB)
					{
						ifcb = dynamic_cast<t_gifcb*>(it->second)->get_ifcb();
						valid_beg = dynamic_cast<t_gifcb*>(it->second)->get_valid_beg();
					}
				}

				// encode
				// ifcb
				auto itifcb = ifcb.begin();
				for (itifcb; itifcb != ifcb.end(); ++itifcb)
				{

					if (itifcb->first < valid_beg || itifcb->second.size() == 0)
					{
						continue;
					}
					_ss << " EPOCH-TIME" << setw(8) << itifcb->first.mjd()
						<< setw(10) << fixed << setprecision(1) << itifcb->first.sod()
						+ itifcb->first.dsec() << endl;

					auto itepo = itifcb->second.begin();
					for (itepo; itepo != itifcb->second.end(); itepo++)
					{
						if (itepo->second->valid()) {
							_ss << " ";
						}
						else {
							_ss << "x";
						}
						_ss << setw(11) << left << itepo->first << fixed << right << setw(10) << setprecision(3)
							<< itepo->second->value << fixed << setw(10) << setprecision(3) << itepo->second->sigma
							<< setw(5) << itepo->second->npoint << endl;
					}
				}

				_ss << "EOF" << endl;
			}
			int size = _fill_buffer(buff, sz);
			_mutex.unlock();
			return size;
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_ifcb::encode_data", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_ifcb::encode_data , unknown mistake" << endl;
			}
			return -1;
			throw(-1);
		}
	}


}//namespace