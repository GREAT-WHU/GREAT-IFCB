
/* ----------------------------------------------------------------------
 * G-Nut - GNSS software development library
 * 
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
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

#include <vector>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "gcoders/gcoder.h" 
#include "gutils/gtypeconv.h"
#include "gutils/gfileconv.h"
#include "gutils/gtimesync.h"

using namespace std;

namespace gnut
{

	t_gcoder::t_gcoder()
	{}

	t_gcoder::t_gcoder(t_gsetbase* s, string version, int sz, string id)
		: _class(id),
		_initialized(false),
		_gnss(true),
		_initver(version),
		_close_with_warning(true)
	{
		_init();

		// after init!
		_buffsz = sz;
		_log = 0;
		_set = 0;
		_irc = 0;
		_hdr = false;

		_ss_position = 0;

		_out_smp = 0;
		_out_len = 0;
		_out_epo = LAST_TIME;

		// use malloc instead of new due to realocate function!
		// ====================================================
		_buffer = (char*)malloc((_buffsz + 1) * sizeof(char));  // due to realocate function!

		if (s) _gset(s); // READ SETTINGS IN CONSTRUCT ONLY!

		_pgm = "Anubis";
	}

	t_gcoder::~t_gcoder()
	{
		// use free instead of delete due to realocate function!
		if (_buffer)
		{
			free(_buffer);
			_buffer = NULL; 
		}

		if (_close_with_warning)
		{
			std::sort(_notes.begin(), _notes.end());
			for (auto it = _notes.begin(); it != _notes.end(); ++it) {
				if (_log) { _log->comment(0, "gcoder", it->str() + " .. " + base_name(_fname)); }
				else { cerr << it->status() << " " << it->str() << endl; }
			}
		}
	}

	int t_gcoder::_gset(t_gsetbase* s)
	{
		gtrace("t_gcoder::gset");

		if (!s) return -1;

		_set = s;
		_beg = dynamic_cast<t_gsetgen*>(_set)->beg();
		_end = dynamic_cast<t_gsetgen*>(_set)->end();

		_hdr = dynamic_cast<t_gsetgen*>(_set)->thin();

		if (_gnss)
			_sys = dynamic_cast<t_gsetgen*>(_set)->sys();


		if(_rec.empty()) _rec = dynamic_cast<t_gsetgen*>(_set)->recs();
		_int = dynamic_cast<t_gsetgen*>(_set)->sampling();

		for (auto itGNSS = _sys.begin(); itGNSS != _sys.end(); ++itGNSS)
		{
			string gs = *itGNSS;
			GSYS gsys = t_gsys::str2gsys(gs);

			_sat[gsys] = dynamic_cast<t_gsetgnss*>(_set)->sat(gsys, false); // empty if not set (to speed up!)
			_obs[gsys] = dynamic_cast<t_gsetgnss*>(_set)->obs(gsys, false); // empty if not set (to speed up!)
			_nav[gsys] = dynamic_cast<t_gsetgnss*>(_set)->nav(gsys, false); // empty if not set (to speed up!)

			// extend gobs list with completely defined signals
			set<string> sgobs = dynamic_cast<t_gsetgnss*>(_set)->gobs(gsys);
			for (auto it = sgobs.begin(); it != sgobs.end(); it++) _obs[gsys].insert(*it);

#ifdef DEBUG
			set<string>::const_iterator it;
			ostringstream os;
			os << gs << " NAV:";
			for (it = _nav[gsys].begin(); it != _nav[gsys].end(); ++it) os << " " << *it;
			os << endl;

			os << gs << " OBS:";
			for (it = _obs[gsys].begin(); it != _obs[gsys].end(); ++it) os << " " << *it;
			os << endl;

			os << gs << " SAT:";
			for (it = _sat[gsys].begin(); it != _sat[gsys].end(); ++it) os << " " << *it;
			os << endl;

			cout << "GNSS filtering:\n" + os.str(); cout.flush();
#endif
		}

#ifdef DEBUG   
		cout << "GCODER settings:"
			<< " _beg " << _beg.str("%Y-%m-%d %H:%M:%S")
			<< " _end " << _end.str("%Y-%m-%d %H:%M:%S")
			<< " _int " << _int
			<< " _sys " << _sys.size()
			<< " _rec " << _rec.size()
			<< " _sat " << _sat.size()
			<< " _nav " << _nav.size()
			<< " _obs " << _obs.size() << endl; cout.flush();
#endif
		return 0;
	}

	void t_gcoder::path(string s)
	{
		substitute(s, GFILE_PREFIX, "");

		_fname = s;
	}

	void t_gcoder::mesg(t_note n, string s)
	{
		t_gnote m(n, "gcoder", s);

		// eliminate repeating messages
		bool exist = false;
		vector<t_gnote>::const_iterator it;
		for (it = _notes.begin(); it != _notes.end(); ++it) {
			if (*it == m) { exist = true; }
		}

		if (!exist) _notes.push_back(m);
	}

	vector<t_gnote> t_gcoder::mesg()
	{
		return _notes;
	}

	void t_gcoder::_init()
	{
		gtrace("t_gcoder::_init");

		_version = _initver;
		_endpos = 0;
	}

	void t_gcoder::clear()
	{
		gtrace("t_gcoder::clear");

		// use free instead of delete due to realocate function!
		if (_buffer)
		{
			free(_buffer);
			_buffer = NULL;
		}

		_init();

		// use malloc instead of new due to realocate function!
		// ====================================================
		_buffer = (char*)malloc((_buffsz + 1) * sizeof(char));  // due to realocate function!
	}

	int t_gcoder::_getline(string& str, int from_pos)
	{
		gtrace("t_gcoder::_getline");

		return _decode_buffer.getline(str, from_pos);
	}

	int t_gcoder::_add2buffer(char* buff, int sz)
	{
		gtrace("t_gcoder::_add2buffer");

		return _decode_buffer.add(buff, sz);
	}

	int t_gcoder::_consume(int bytes_to_eat)
	{
		gtrace("t_gcoder::_consume");

		return _decode_buffer.consume(bytes_to_eat);
	}

	int t_gcoder::add_data(string data_id, t_gdata* data)
	{
		gtrace("t_gcoder::add_data");

		if (data_id.empty() || data == 0) return -1;

		map<string, t_gdata*>::iterator it = _data.find(data_id);
		if (it != _data.end()) {
			if (_log) _log->comment(1, "gcoder", "warning: structure " + data_id + " already exists !");
			return -1;
		}

#ifdef DEBUG
		cout << " gcoder: adding data structure: " << data_id << endl; cout.flush();
#endif
		_data[data_id] = data;

		// can be used to directly setup data containers in individual gcoders
		_add_data(data_id, data);

		return 1;
	}

	int t_gcoder::_fill_buffer(char* buff, int sz)
	{
		gtrace("t_gcoder::_filter_buffer");

		_ss.seekg(0, _ss.end);
		long len = (long)_ss.tellg();
		len = len - _ss_position;
		_ss.seekg(_ss_position, ios_base::beg);

		int size = (len < sz) ? len : sz;

		_ss.clear();
		_ss.read(buff, size);

		string tmp = buff;
		tmp = tmp.substr(0, size);
		size_t ifirst = 0;
		if ((ifirst = tmp.find_last_of(crlf)) != string::npos) {
			tmp = tmp.substr(0, ifirst + 1);
			size = tmp.size();
			_ss.seekg(_ss_position + size, ios_base::beg);
		}

		if (_ss.fail()) {
			cout << "HEAD: any problem ?\n";

		}
		else if (_ss.gcount() == 0) {
			_ss_position = 0;
			_ss.str("");
			_ss.clear();
		}
		else {		
			_ss_position += size; 
		}

		return size;
	}

	bool t_gcoder::_filter_epoch(const t_gtime& epo)
	{
		gtrace("t_gcoder::_filter_epoch");

		if (time_sync(epo, _int, _scl, _log)) {
			return true;
		}
		return false;
	}

	bool t_gcoder::_filter_gnss(const string& prn)
	{
		gtrace("t_gcoder::_filter_gnss");

		string gs = t_gsys::char2str(prn[0]); // DONT USE SYS FROM ARG WHICH MAY BE EMPTY
		GSYS gsys = t_gsys::str2gsys(gs);     // DONT USE SYS FROM ARG WHICH MAY BE EMPTY
		if ((_sys.size() == 0 || _sys.find(gs) != _sys.end()) &&
			(_sat[gsys].size() == 0 || _sat[gsys].find(prn) != _sat[gsys].end()))
		{
			return true;
		}
		return false;
	}


} // namespace
