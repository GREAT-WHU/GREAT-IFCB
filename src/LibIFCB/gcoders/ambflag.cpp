/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		ambflag.cpp
* @brief	decode and enconde ambflag file(XXXXddd0.yyo.log/log13)
*				XXXX  ---- SITE name
*				 ddd  ---- Doy of the file
*				  yy  ---- year
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
*/

#include"gcoders/ambflag.h"

using namespace std;
namespace great {

	t_ambflag::t_ambflag(t_gsetbase* s,t_gdata::ID_TYPE ambflag_version, string version, int sz)
		:t_gcoder(s, version, sz)
	{
		if (ambflag_version != t_gdata::AMBFLAG && ambflag_version != t_gdata::AMBFLAG13) {
			throw "t_ambflag only support AMBFLAG or AMBFLAG13";
		}
		_ambflag_version = ambflag_version;
		gtrace("t_ambflag::constructor");
	}

	t_ambflag::~t_ambflag()
	{
		gtrace("t_ambflag::destructor");
	}

	int t_ambflag::decode_head(char* buff, int sz, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();

		if (t_gcoder::_add2buffer(buff, sz) == 0)
		{
			_mutex.unlock();
			return 0;
		}

		string tmp;
		int tmpsize = 0;
		int consume = 0;
		string str;
		try
		{
			while ((tmpsize = t_gcoder::_getline(tmp, 0)) >= 0)
			{
				consume += tmpsize;
				if (tmp.find("END OF HEADER", 60) == string::npos)
				{
					if (tmp.find("SOFTWARE / DATE", 60) != string::npos)
					{
						_ambflag_head.software = trim(tmp.substr(0, 20));
						str = trim(tmp.substr(40, 16));
						_ambflag_head.run_time.from_str("%d-%b-%Y", str);
					}
					else if (tmp.find("STATION", 60) != string::npos)
					{
						_ambflag_head.site_name = trim(tmp.substr(0, 4));
					}
					else if (tmp.find("SYS / FREQ1 / FREQ2", 60) != string::npos)
					{
						GSYS gsys = t_gsys::str2gsys( trim(tmp.substr(0, 3)) );
						_ambflag_head.proc_band[gsys].insert( trim(tmp.substr(6, 3)) );
						_ambflag_head.proc_band[gsys].insert( trim(tmp.substr(12, 3)) );
					}
					else if (tmp.find("BEGIN TIME", 60) != string::npos)
					{
						istringstream istr(tmp);
						int year, mon, day, hr, min;
						double sec;
						bool conv = false;
						istr >> year >> mon >> day >> hr >> min >> sec >> str;
						if (str == "GPST") conv = true;
						_ambflag_head.beg_time.from_ymdhms(year, mon, day, hr, min, sec, conv);
					}
					else if (tmp.find("END TIME", 60) != string::npos)
					{
						istringstream istr(tmp);
						int year, mon, day, hr, min;
						double sec;
						bool conv = false;
						istr >> year >> mon >> day >> hr >> min >> sec >> str;
						if (str == "GPST") conv = true;
						_ambflag_head.end_time.from_ymdhms(year, mon, day, hr, min, sec, conv);
					}
					else if (tmp.find("INTERVAL", 60) != string::npos)
					{
						_ambflag_head.interval = str2dbl( trim(tmp.substr(0, 10)) );
					}
					else
					{
						if (_log)
						{
							_log->comment(2, "t_ambflag::decode_head", "WARNING: unknown ambflag-head message :" + tmp);
						}
						else
						{
							cout << "WARNING: t_ambflag::decode_head ,unknown ambflag-head message :" << tmp << endl;
						}
						t_gcoder::_consume(tmpsize);
						_mutex.unlock();
						return -1;
					}
					t_gcoder::_consume(tmpsize);
				}
				else
				{
					//add head data
					map<string, t_gdata*>::iterator it = _data.begin();
					while (it != _data.end()){
						if (it->second->id_type() == _ambflag_version)
						{
							string site_name;
							site_name = this->_fname.substr(_fname.rfind(".ambflag") - 12).substr(0, 4);
							((t_gallambflag*)it->second)->addAmbFlagHead(site_name, _ambflag_head);
						}
						it++;
					}
					t_gcoder::_consume(tmpsize);
					_mutex.unlock();
					return -1;
				}
			}
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_ambflag::decode_head", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_ambflag::decode_head , unknown mistake" << endl;
			}
			return -1;
			throw("Decode ambflag head is Wrong!");
		}
		_mutex.unlock();
		return consume;
	}

	int t_ambflag::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();

		if (t_gcoder::_add2buffer(buff, sz) == 0)
		{
			_mutex.unlock();
			return 0;
		}

		string tmp;
		int tmpsize = 0;
		ambflag_data data_tmp;
		string prn;
		try
		{
			while ((tmpsize = t_gcoder::_getline(tmp, 0)) >= 0)
			{
				istringstream istr(tmp);
				if (tmp.substr(0, 3) == "AMB" || tmp.substr(0, 3) == "DEL" || tmp.substr(0, 3) == "BAD")
				{
					istr >> data_tmp.identify >> prn >> data_tmp.beg_epo >> data_tmp.end_epo
						>> data_tmp.reason;

					if (tmp.substr(0, 3) == "AMB")          data_tmp.iflag = "1";
					else if (tmp.substr(0, 3) == "BAD")     data_tmp.iflag = "2";
					else if (tmp.substr(0, 3) == "DEL")     data_tmp.iflag = "3";

					//fill data loop
					map<string, t_gdata*>::iterator it = _data.begin();
					while (it != _data.end())
					{
						if (it->second->id_type() == _ambflag_version)
						{
							string site_name;
							site_name = this->_fname.substr(_fname.rfind(".ambflag") - 12).substr(0, 4);
							((t_gallambflag*)it->second)->addAmbFlagData(site_name, prn, data_tmp);
						}
						it++;
					}
				}
				else
				{
					if (_log)
					{
						_log->comment(2, "t_ambflag::decode_data", "WARNING: unknown ambflag-data message :" + tmp);
					}
					else
					{
						cout << "WARNING: unknown ambflag-data message :" << tmp << endl;
					}
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
				_log->comment(2, "t_ambflag::decode_data", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_ambflag::decode_data , unknown mistake" << endl;
			}
			return -1;
			throw("Decode ambflag data is Wrong!");
		}
		_mutex.unlock();
		return tmpsize;
	}

}//namespace
