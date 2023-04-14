
/* ----------------------------------------------------------------------
 * G-Nut - GNSS software development library
 * 
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
  
  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  This file is part of the G-Nut C++ library.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 3 of
  the License, or (at your option) any later version.
 
  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses>.

-*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <memory> 

#include "gcoders/biabernese.h"
#include "gutils/gtypeconv.h"
#include "gmodels/gbias.h"

using namespace std;

namespace gnut {

// constructor   
t_biabernese::t_biabernese( t_gsetbase* s, string version, int sz, string id )
 : t_gcoder( s, version, sz, id )
{   
  gtrace(_class+"::construct");
  _allbias = 0;
  _ac = "";
  _beg = FIRST_TIME;
  _end = LAST_TIME;
}

int t_biabernese::decode_head(char* buff, int sz, vector<string>& errmsg)
{
#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif

	gtrace("t_biabernese::decode_head");

	_mutex.lock();

	if (t_gcoder::_add2buffer(buff, sz) == 0) { _mutex.unlock(); return 0; };

	string line;
	int consume = 0;
	int tmpsize = 0;
	size_t idx = 0;
	string year = "";
	string mon = "";

	while ((tmpsize = t_gcoder::_getline(line)) >= 0) {
		consume += tmpsize;
		if (line.find("%=BIA") == 0) { // first line
			_is_bias = true;
			_version = str2dbl(line.substr(6, 4));
			_ac = line.substr(11, 3);
			if (_ac == "IGG") _ac = "CAS";
			_is_absolute = line.substr(64, 1) == "A";
			if (_is_absolute) _ac += "_A";
			else _ac += "_R";
		} 
		else if (line.find("+BIAS/SOLUTION") != string::npos) {
			t_gcoder::_consume(tmpsize);
			_mutex.unlock(); return -1;
		}
		else if (line.find("CODE'S MONTHLY") != string::npos) {
			_ac = "COD_R";
			// change for another header of DCB files glfeng
			if ((idx = line.find("YEAR ")) != string::npos) {
				year = line.substr(idx + 5, 4);
				mon = line.substr(idx + 17, 2);
			}
			// add for another header of DCB files glfeng
			if ((idx = line.find("YEAR-MONTH")) != string::npos) {
				if (line.substr(idx + 11, 1) > "8") year = "19" + line.substr(idx + 11, 2);
				else year = "20" + line.substr(idx + 11, 2);
				mon = line.substr(idx + 14, 2);
			}
			int yyyy = str2int(year);
			int mm = str2int(mon);
			//_beg.from_ymdhms(yyyy, mm, 1, 0, 0, 0);
			//_end.from_ymdhms(yyyy, mm + 1, 1, 0, 0, 0); _end = _end - 1;  // ²»¿¼ÂÇÊ±¼ä
			t_gcoder::_consume(tmpsize);
			_mutex.unlock(); return -1;
		}
		else if (line.find("CODE'S 30-DAY") != string::npos){
			_ac = "COD_R";
			string end_doy = "";
			if ((idx = line.find("ENDING DAY")) != string::npos) {
				year = line.substr(idx + 16, 4);
				end_doy = line.substr(idx + 11, 3);
			}
			string time_str = year + ":" + end_doy;
			//_end.from_str("%Y:%j", time_str);
			//_beg = _end - 86400 * 30;
			t_gcoder::_consume(tmpsize);
			_mutex.unlock(); return -1;
		}
		t_gcoder::_consume(tmpsize);
	}
	_mutex.unlock(); return consume;
}
  
int t_biabernese::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
{
	if (_is_bias) {
		if (_version < 1.0) return _decode_data_sinex_0(buff, sz, cnt, errmsg);
		else                return _decode_data_sinex(buff, sz, cnt, errmsg);
	}
	else if(_ac == "COD_R") {
		return _decode_data_CODE(buff, sz, cnt, errmsg);
	}
	else {
		return -1;
	}
}

int t_biabernese::_decode_data_CODE(char* buff, int sz, int& cnt, vector<string>& errmsg)
{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
	_mutex.lock();

	if (t_gcoder::_add2buffer(buff, sz) == 0) { _mutex.unlock(); return 0; };


	string line;
	int consume = 0;
	int tmpsize = 0;
	bool complete = false;

	GOBS gobs1, gobs2;
	gobs1 = gobs2 = X;

	while ((tmpsize = t_gcoder::_getline(line)) >= 0) {
		complete = false;

		size_t idx = 0;
		string signals = "";
		if ((idx = line.find("DIFFERENTIAL (")) != string::npos) {
			signals = line.substr(idx + 14, 5);
			gobs1 = str2gobs(signals.substr(0, 2));
			gobs2 = str2gobs(signals.substr(3, 2));
			consume += t_gcoder::_consume(tmpsize);
		}

		string prn = "";
		double dcb, std;
		dcb = std = 0.0;

		istringstream istr(line);
		istr >> prn >> dcb >> std;
		if (!istr.fail()) {
			complete = true;
		}
		else {
			consume += t_gcoder::_consume(tmpsize);
			continue;
		}

		if (complete) {

			map<string, t_gdata*>::iterator it = _data.begin();
			while (it != _data.end()) {
				if (it->second->id_type() == t_gdata::ALLBIAS) {
					shared_ptr<t_gbias> pt_bias = make_shared<t_gbias>();
					pt_bias->set(_beg, _end, dcb* 1e-9 * CLIGHT, gobs1, gobs2);
					((t_gallbias*)it->second)->add(_ac, _beg, prn, pt_bias);
				}
				it++;
			}
			consume += t_gcoder::_consume(tmpsize);
		}
	}

	_mutex.unlock(); return consume;
}

int t_biabernese::_decode_data_sinex(char* buff, int sz, int& cnt, vector<string>& errmsg)
{
#ifdef BMUTEX
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_mutex.lock();

	if (t_gcoder::_add2buffer(buff, sz) == 0) { _mutex.unlock(); return 0; };

#ifdef DEBUG
	cout << " BUFFER : \n" << _buffer << "\n size = " << sz << " END OF BUFFER \n\n"; cout.flush();
#endif

	string line;
	int consume = 0;
	int tmpsize = 0;
	bool complete = false;
	size_t idx = 0;

	GOBS gobs1, gobs2;
	gobs1 = gobs2 = X;

	while ((tmpsize = t_gcoder::_getline(line)) >= 0) {
		complete = false;
		if (line.find("-BIAS/SOLUTION") != line.npos || line.find("%=ENDBIA") == 0) {
			_mutex.unlock(); return 0;
		}
		else if (line.substr(1, 4) == "DSB " || line.substr(1, 4) == "ISB " || line.substr(1, 4) == "OSB ") {
			string svn, prn, station, obs1, obs2, units;
			double value, precision, factor;
			if (line.size() >= 104) {
				svn = line.substr(6, 4);
				prn = line.substr(11, 3);
				station = trim(line.substr(15, 9));
				if (station.size() != 0) { // currently station-specific bias is not used
					consume += t_gcoder::_consume(tmpsize);
					continue;
				}
				obs1 = trim(line.substr(25, 4));
				obs2 = trim(line.substr(30, 4));
				gobs1 = str2gobs(obs1);
				gobs2 = str2gobs(obs2);
				_beg.from_str("%Y:%j:%s", line.substr(35, 14));
				_end.from_str("%Y:%j:%s", line.substr(50, 14));
				units = trim(line.substr(65, 4));		
				if (units == "ns") {
					factor = 1e-9 * CLIGHT;
				}
				else if (units == "cyc") {
					// may for phase bias, but wavelength is needed, so not recommended
					consume += t_gcoder::_consume(tmpsize);
					continue;
				}
				else {
					// other units type currently not considered
					consume += t_gcoder::_consume(tmpsize);
					continue;
				}
				value = str2dbl(line.substr(70, 21));
				precision = str2dbl(line.substr(92, 11));
				complete = true;
			}
			else {
				consume += t_gcoder::_consume(tmpsize);
				continue;
			}
			if (complete) {
				map<string, t_gdata*>::iterator it = _data.begin();
				while (it != _data.end()) {
					if (it->second->id_type() == t_gdata::ALLBIAS) {
						shared_ptr<t_gbias> pt_bias = make_shared<t_gbias>();
						pt_bias->set(_beg, _end, value * factor, gobs1, gobs2);
						((t_gallbias*)it->second)->add(_ac, _beg, prn, pt_bias);
					}
					it++;
				}
				consume += t_gcoder::_consume(tmpsize);
			}
		}
#ifdef OLD
		if ((line.substr(1, 3) == "DSB" || line.substr(1, 3) == "DCB") && trim(line.substr(12, 2)) != "") {

			double dcb, std;
			dcb = std = 0.0;

			istringstream istr(line);
			string str1, str2, str3, str4,
				strobs1, strobs2, beg_doy, end_doy, prn;
			istr >> str1 >> str2 >> prn >> strobs1 >> strobs2
				>> beg_doy >> end_doy >> str4 >> dcb >> std;

			if (!istr.fail()) {
				complete = true;
				if (str1 == "DSB")
				{
					_beg.from_str("%Y:%j", beg_doy.substr(0, 8));
					_end.from_str("%Y:%j", end_doy.substr(0, 8));
				}
				else if (str1 == "DCB")
				{
					_beg.from_str("%y:%j", beg_doy.substr(0, 6));
					_end.from_str("%y:%j", end_doy.substr(0, 6));
				}
				gobs1 = str2gobs(strobs1);
				gobs2 = str2gobs(strobs2);
			}
			else {
				consume += t_gcoder::_consume(tmpsize);
				continue;
			}
			if (complete) {

#ifdef DEBUG
				cout << "DCB decoding: " << prn << " " << beg.str_ymdhms() << " " << end.str_ymdhms() << " " << gobs2str(gobs1) << " " << gobs2str(gobs2) << " " << dcb << endl;
				int ooo; cin >> ooo;
#endif

				map<string, t_gdata*>::iterator it = _data.begin();
				while (it != _data.end()) {
					if (it->second->id_type() == t_gdata::ALLBIAS) {
						shared_ptr<t_gbias> pt_bias = make_shared<t_gbias>();
						pt_bias->set(_beg, _end, dcb* 1e-9 * CLIGHT, gobs1, gobs2);
						((t_gallbias*)it->second)->add(_ac, _beg, prn, pt_bias);
					}
					it++;
				}
				consume += t_gcoder::_consume(tmpsize);
			}
		}
		else if (line.substr(1, 3) == "OSB"  && trim(line.substr(12, 2)) != "") {

			double dcb, std;
			dcb = std = 0.0;

			istringstream istr(line);
			string str1, str2, str3, strobs1, beg_doy, end_doy, prn;
			istr >> str1 >> str3 >> prn >> strobs1 
				>> beg_doy >> end_doy >> str2 >> dcb >> std;

			if (!istr.fail()) {
				complete = true;
				//_beg.from_str("%Y:%j", beg_doy.substr(0, 8));
				//_end.from_str("%Y:%j", end_doy.substr(0, 8));
				_beg.from_str("%Y:%j:%s", beg_doy.substr(0, 14));
				_end.from_str("%Y:%j:%s", end_doy.substr(0, 14));
				gobs1 = str2gobs(strobs1);
			}
			else {
				consume += t_gcoder::_consume(tmpsize);
				continue;
			}
			if (complete) {

#ifdef DEBUG
				cout << "DCB decoding: " << prn << " " << beg.str_ymdhms() << " " << end.str_ymdhms() << " " << gobs2str(gobs1) << " " << gobs2str(gobs2) << " " << dcb << endl;
				int ooo; cin >> ooo;
#endif

				map<string, t_gdata*>::iterator it = _data.begin();
				while (it != _data.end()) {
					if (it->second->id_type() == t_gdata::ALLBIAS) {
						shared_ptr<t_gbias> pt_bias = make_shared<t_gbias>();
						pt_bias->set(_beg, _end, dcb* 1e-9 * CLIGHT, gobs1, gobs1);
						if (t_gobs(gobs1).is_phase()) 
						{
							((t_gallbias*)it->second)->add(_ac + "_PHASE", _beg, prn, pt_bias);
						}
						else
						{
							((t_gallbias*)it->second)->add(_ac, _beg, prn, pt_bias);
						}
					}
					it++;
				}
				consume += t_gcoder::_consume(tmpsize);
			}
		}
#endif
		else
		{
			consume += t_gcoder::_consume(tmpsize);
		}
	}

	_mutex.unlock(); return consume;
}

int t_biabernese::_decode_data_sinex_0(char* buff, int sz, int& cnt, vector<string>& errmsg)
{
#ifdef BMUTEX
	boost::mutex::scoped_lock lock(_mutex);
#endif
	_mutex.lock();

	if (t_gcoder::_add2buffer(buff, sz) == 0) { _mutex.unlock(); return 0; };

#ifdef DEBUG
	cout << " BUFFER : \n" << _buffer << "\n size = " << sz << " END OF BUFFER \n\n"; cout.flush();
#endif

	string line;
	int consume = 0;
	int tmpsize = 0;
	bool complete = false;
	size_t idx = 0;

	GOBS gobs1, gobs2;
	gobs1 = gobs2 = X;

	while ((tmpsize = t_gcoder::_getline(line)) >= 0) {
		complete = false;
		if (line.find("-BIAS/SOLUTION") != line.npos || line.find("%=ENDBIA") == 0) {
			_mutex.unlock(); return 0;
		}
		else if (line.substr(1, 4) == "DSB " || line.substr(1, 4) == "ISB " || line.substr(1, 4) == "OSB ") {
			string svn, prn, station, obs1, obs2, units;
			double value, precision, factor;
			if (line.size() >= 105) {
				svn = line.substr(6, 4);
				prn = line.substr(11, 3);
				station = trim(line.substr(15, 9));
				if (station.size() != 0) { // currently station-specific bias is not used
					consume += t_gcoder::_consume(tmpsize);
					continue;
				}
				obs1 = trim(line.substr(30, 4));
				obs2 = trim(line.substr(35, 4));
				gobs1 = str2gobs(obs1);
				gobs2 = str2gobs(obs2);
				_beg.from_str("%y:%j:%s", line.substr(40, 12));
				_end.from_str("%y:%j:%s", line.substr(53, 12));
				units = trim(line.substr(66, 4));
				if (units == "ns") {
					factor = 1e-9 * CLIGHT;
				}
				else if (units == "cyc") {
					// may for phase bias, but wavelength is needed, so not recommended
					consume += t_gcoder::_consume(tmpsize);
					continue;
				}
				else {
					// other units type currently not considered
					consume += t_gcoder::_consume(tmpsize);
					continue;
				}
				value = str2dbl(line.substr(71, 21));
				precision = str2dbl(line.substr(93, 11));
				complete = true;
			}
			else {
				consume += t_gcoder::_consume(tmpsize);
				continue;
			}
			if (complete) {
				map<string, t_gdata*>::iterator it = _data.begin();
				while (it != _data.end()) {
					if (it->second->id_type() == t_gdata::ALLBIAS) {
						shared_ptr<t_gbias> pt_bias = make_shared<t_gbias>();
						cout << value << factor << value * factor << endl;
						pt_bias->set(_beg, _end, value * factor, gobs1, gobs2);
						((t_gallbias*)it->second)->add(_ac, _beg, prn, pt_bias);
					}
					it++;
				}
				consume += t_gcoder::_consume(tmpsize);
			}
		}
		else {
			consume += t_gcoder::_consume(tmpsize);
		}
	}

	_mutex.unlock(); return consume;
}

} // namespace
