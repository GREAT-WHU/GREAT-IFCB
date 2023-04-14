

#include <iomanip>
#include <sstream>
#include <algorithm>

#include "gutils/gnss.h"
#include "gutils/gsys.h"
#include "gutils/gtypeconv.h"
#include "gset/gsetgen.h"

using namespace std;
using namespace pugi;

namespace gnut
{


	t_gsetgen::t_gsetgen(bool gnss)
		: t_gsetbase(),
		_gnss(gnss),
		_dec(0)
	{
		_set.insert(XMLKEY_GEN);

		// initiate GNSS string
		if (_gnss) {
			t_map_sats gnss_sats = GNSS_SATS();
			for (auto itGNS = gnss_sats.begin(); itGNS != gnss_sats.end(); ++itGNS) {
				_sys += " " + t_gsys::gsys2str(itGNS->first);
			}
		}
	}


	t_gsetgen::~t_gsetgen()
	{}


	
	t_gtime t_gsetgen::beg(bool conv)
	{
		_gmutex.lock();

		string str = _doc.child(XMLKEY_ROOT).child(XMLKEY_GEN).child_value("beg");
		substitute(str, "\n", "");
		substitute(str, "\"", "");

		t_gtime gt(t_gtime::GPS);

		if (str.empty()) {
			_gmutex.unlock();
			t_gtime tmp(FIRST_TIME);
			return tmp;
		}

		gt.from_str("%Y-%m-%d %H:%M:%S", trim(str), conv);
		_gmutex.unlock(); return gt;
	}



	t_gtime t_gsetgen::end(bool conv)
	{
		_gmutex.lock();

		string str = _doc.child(XMLKEY_ROOT).child(XMLKEY_GEN).child_value("end");
		substitute(str, "\n", "");
		substitute(str, "\"", "");

		t_gtime gt(t_gtime::GPS);

		if (str.empty()) {
			_gmutex.unlock();
			t_gtime tmp(LAST_TIME);
			return tmp;
		}

		gt.from_str("%Y-%m-%d %H:%M:%S", trim(str), conv);
		_gmutex.unlock(); return gt;
	}



	double t_gsetgen::sampling()
	{
		_gmutex.lock();

		string str = _doc.child(XMLKEY_ROOT).child(XMLKEY_GEN).child_value("int");

		// delete spaces
		str.erase(remove(str.begin(), str.end(), ' '), str.end());

		double tmp = str2dbl(str);

		if (str.find(".") != string::npos) {
			_dec = str.substr(str.find(".") + 1).length(); // decimal digits resolution
		}

		_gmutex.unlock(); return tmp;
	}



	set<string> t_gsetgen::sys()
	{
		_gmutex.lock();

		set<string> xcl, tmp = t_gsetbase::_setval(XMLKEY_GEN, "sys");

		// exclude starting with '-'
		set<string>::iterator itSYS, itTMP;
		for (itTMP = tmp.begin(); itTMP != tmp.end(); ) {
			if ((*itTMP)[0] == '-') { xcl.insert(*itTMP); itSYS = itTMP; ++itTMP; tmp.erase(itSYS); }//"-"mean without this system?
			else ++itTMP;
		}

		// if empty, complete, i.e. if only exclusions listed (and gnss requested!)
		if (tmp.size() == 0 && _gnss) {
			t_map_sats gnss_sats = GNSS_SATS();
			t_map_sats::const_iterator itGNS;
			// loop over all systems
			for (itGNS = gnss_sats.begin(); itGNS != gnss_sats.end(); ++itGNS) {
				string gs = t_gsys::gsys2str(itGNS->first);
				if (xcl.find("-" + gs) == xcl.end()) tmp.insert(gs);
			}
		}
		_gmutex.unlock(); return tmp;
	}



	set<string> t_gsetgen::recs()
	{
		_gmutex.lock();

		set<string> tmp = t_gsetbase::_setval(XMLKEY_GEN, "rec");

		_gmutex.unlock(); return tmp;
	}

	set<string> t_gsetgen::sat_rm()
	{
		_gmutex.lock();

		set<string> tmp = t_gsetbase::_setval(XMLKEY_GEN, "sat_rm");
		_gmutex.unlock(); return tmp;
	}



	void t_gsetgen::check()
	{
		_gmutex.lock();

		// check existence of nodes/attributes
		xml_node parent = _doc.child(XMLKEY_ROOT);
		xml_node node = _default_node(parent, XMLKEY_GEN);

		_default_node(node, "BEG", "");                         // all!
		_default_node(node, "END", "");	                      // all!
		_default_node(node, "SYS", "");	                      // all!
		_default_node(node, "REC", "");	                      // none 
		_default_node(node, "INT", int2str(DEF_SAMPLING).c_str());  // default


		// TO CHECK USER-SETUP CONSISTENCY
		_gmutex.unlock();
		if (floor(sampling()) < 1 || _dec > 0) {

			if (sampling() < 0.0) {
				_default_node(node, "INT", "0.0", true); // reset!
				if (_log) _log->comment(0, "gsetgen", "Warning: sampling rate settings negative: reset to 0");
				cerr << "gsetgen - Warning: sampling rate settings negative: reset to 0\n";
			}
			else {
				if (_log) _log->comment(1, "gsetgen: sampling rate settings above 1Hz recognized");
			}
		}

		return;
	}



	void t_gsetgen::help()
	{
		_gmutex.lock();
		t_gtime beg(t_gtime::GPS); beg = beg - beg.sod();
		t_gtime end(t_gtime::GPS); end = beg + 86399;

		cerr << "\n <gen>\n"
			<< "   <beg> \"" << beg.str_ymdhms() << "\" </beg>\n"  // FIRST_TIME.str("\"%Y-%m-%d %H:%M:%S\"")
			<< "   <end> \"" << end.str_ymdhms() << "\" </end>\n"; // LAST_TIME.str("\"%Y-%m-%d %H:%M:%S\"")

		if (_gnss)
			cerr << "   <sys> " << _sys << " </sys>\n"; // GNSS systems

		cerr << "   <rec> GOPE WTZR POTS                </rec>\n"  // list of site identificators
			<< "   <int>" + int2str(DEF_SAMPLING) + "</int>\n"
			<< " </gen>\n";

		cerr << "\t<!-- general description:\n"
			<< "\t beg    .. beg time          (default: all)\n"
			<< "\t end    .. end time          (default: all)\n"
			<< "\t int    .. data sampling     (default: 30s)\n";

		if (_gnss)
			cerr << "\t sys    .. GNSS system(s)    (default: all)\n";

		cerr << "\t rec    .. GNSS receiver(s)  (rec active list, e.g.: GOPE ONSA WTZR ... )\n"
			<< "\t -->\n\n";

		_gmutex.unlock(); return;
	}

} // namespace
