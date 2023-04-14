

#include <iomanip>
#include <sstream>
#include <algorithm>

#include "gset/gsetinp.h"
#include "gutils/gfileconv.h"

using namespace std;
using namespace pugi;

namespace gnut {


	IFMT t_gsetinp::str2ifmt(const string& s)
	{
		string tmp = s;
		transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
		if (tmp == "RINEXO") return RINEXO_INP;
		if (tmp == "RINEXN") return RINEXN_INP;
		if (tmp == "BIABERN") return BIABERN_INP;
		if (tmp == "AMBFLAG") return AMBFLAG_INP;
		if (tmp == "AMBFLAG13") return AMBFLAG13_INP;
		if (tmp == "IFCB") return IFCB_INP;
		if (tmp == "SATPARS")return SATPARS_INP;

		return IFMT(-1);
	}



	string t_gsetinp::ifmt2str(const IFMT& f)
	{
		switch (f) {
		case RINEXO_INP:  return "RINEXO";
		case RINEXN_INP:  return "RINEXN";
		case BIABERN_INP:    return "BIABERN";
		case AMBFLAG_INP:    return "AMBFLAG";
		case IFCB_INP:       return "IFCB";
		case SATPARS_INP:	 return "SATPARS";
		default:             return "UNDEF";
		}
		return "UNDEF";
	}


	t_gsetinp::t_gsetinp()
		: t_gsetbase()
	{
		_set.insert(XMLKEY_INP);
		_chkNavig = true;
		_chkHealth = true;
		_corrStream = "";
	}



	t_gsetinp::~t_gsetinp()
	{}



	int t_gsetinp::input_size(const string& fmt)
	{
		_gmutex.lock();

		int tmp = _inputs(fmt).size();

		_gmutex.unlock(); return tmp;
	}



	multimap<IFMT, string> t_gsetinp::inputs_all()
	{
		_gmutex.lock();

		multimap<IFMT, string> map;

		set<string> ifmt = _iformats();
		set<string>::const_iterator itFMT = ifmt.begin();

		while (itFMT != ifmt.end())
		{
			string fmt = *itFMT;
			IFMT  ifmt = str2ifmt(fmt);
			vector<string> inputs = _inputs(fmt);//get file name in input node
			vector<string>::const_iterator itINP = inputs.begin();
			while (itINP != inputs.end()) {
				map.insert(map.end(), pair<IFMT, string>(ifmt, *itINP));
				itINP++;
			}
			itFMT++;
		}
		_gmutex.unlock(); return map;
	}



	vector<string> t_gsetinp::inputs(const string& fmt)
	{
		return _inputs(fmt);
	}



	set<string> t_gsetinp::iformats()
	{
		return _iformats();
	}

	vector<string> t_gsetinp::_inputs(const string& fmt)
	{
		vector<string> tmp;
		set<string> list;
		string str;

		for (xml_node node = _doc.child(XMLKEY_ROOT).child(XMLKEY_INP).first_child(); node; node = node.next_sibling()) {
			if (node.name() == fmt) {
				istringstream is(node.child_value());
				while (is >> str && !is.fail()) {
					if (str.find("://") == string::npos) str = GFILE_PREFIX + str;
					if (list.find(str) == list.end()) {
						tmp.push_back(str);
						list.insert(str);
					}
					else {
						if (_log) _log->comment(1, "gsetinp", "READ: " + str + " multiple request ignored");
					}
				}
			}
		}
		return tmp;
	}



	set<string> t_gsetinp::_iformats()
	{
		set<string> tmp;
		for (xml_node node = _doc.child(XMLKEY_ROOT).child(XMLKEY_INP).first_child(); node; node = node.next_sibling())
		{
			tmp.insert(node.name());
		}
		return tmp;
	}


	bool t_gsetinp::chkNavig()
	{
		bool tmp;

		tmp = _doc.child(XMLKEY_ROOT).child(XMLKEY_INP).attribute("chk_nav").as_bool();

		return tmp;
	}


	bool t_gsetinp::chkHealth()
	{
		bool tmp;

		tmp = _doc.child(XMLKEY_ROOT).child(XMLKEY_INP).attribute("chk_health").as_bool();

		return tmp;
	}


	void t_gsetinp::check()
	{
		_gmutex.lock();

		// check existence of nodes/attributes
		xml_node parent = _doc.child(XMLKEY_ROOT);
		xml_node node = _default_node(parent, XMLKEY_INP);

		// check supported input formats (see IFMT enum !)
		set<string> ifmt = _iformats();
		set<string>::const_iterator itFMT = ifmt.begin();
		while (itFMT != ifmt.end()) {
			string fmt = *itFMT;
			IFMT  ifmt = str2ifmt(fmt);
			if (ifmt < 0) {
				_doc.child(XMLKEY_ROOT).child(XMLKEY_INP).remove_child(node.child(fmt.c_str()));
				if (_log) _log->comment(0, "Warning: " + fmt + " inp format not implemented [gsetinp::check()]!");
				itFMT++;
				continue;
			}

			// check application-specific output format
			if (_IFMT_supported.find(ifmt) == _IFMT_supported.end()) {
				_doc.child(XMLKEY_ROOT).child(XMLKEY_INP).remove_child(node.child(fmt.c_str()));
				if (_log) _log->comment(0, "Warning: " + fmt + " inp format not supported by this application!");
				else                cerr << "Warning: " + fmt + " inp format not supported by this application!\n";
			}
			itFMT++;
		}

		_default_attr(node, "chk_nav", _chkNavig);
		_default_attr(node, "chk_health", _chkHealth);

		xml_node nodeBNCRTCM = _doc.child(XMLKEY_ROOT).child(XMLKEY_INP).child("bncrtcm");
		_default_attr(nodeBNCRTCM, "_corrStream", _corrStream);

		_gmutex.unlock(); return;
	}



	void t_gsetinp::help()
	{
		_gmutex.lock();


		cerr << " <inputs>\n"
			<< "   <rinexo> file://dir/name </rinexo> \t\t <!-- obs RINEX decoder -->\n"
			<< "   <rinexn> file://dir/name </rinexn> \t\t <!-- nav RINEX decoder -->\n"
			<< " </inputs>\n";

		cerr << "\t<!-- inputs description:\n"
			<< "\t <decoder> path1 path2 path3  </decoder>\n"
			<< "\t ... \n"
			<< "\t where path(i) contains [file,tcp,ntrip]:// depending on the application\n"
			<< "\t -->\n\n";

		_gmutex.unlock(); return;
	}

} // namespace
