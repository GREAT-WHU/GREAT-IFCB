
#include <iomanip>
#include <sstream>
#include <algorithm>

#if  defined _WIN32 || defined _WIN64
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "gset/gsetbase.h"

using namespace std;
using namespace pugi;

namespace gnut
{


	t_gsetbase::t_gsetbase()
		: _name(""),
		_pgm(""),
		_ver(""),
		_rev(""),
		_own(""),
		_dat(""),
		_tim("")
	{
		_log = 0;
		_delimiter = "  ";  // EMPTY! for nodes/elements only
		_set.insert("base");
	}



	t_gsetbase::~t_gsetbase()
	{
	}


	int t_gsetbase::read(const string& file)
	{
		_gmutex.lock();

		_name = file;

		if (!(_irc = _doc.load_file(_name.c_str()))) {
			cerr << "xconfig: not file read " + _name + " " + string(_irc.description()) << endl;
			_gmutex.unlock(); return -1;
		}


		_gmutex.unlock();

		this->check();

		return 0;
	}


	// Read from istream
	// ----------
	int t_gsetbase::read_istream(istream& is)
	{
#if defined _WIN32 || defined _WIN64
		if (_isatty(_fileno(stdin))) return -1;  // stdin is terminal
#else // pipe
		if (isatty(fileno(stdin))) return -1;  // stdin is terminal
	  // else return 1;
#endif
		return 1; // NOT FUNCTIONAL

		// ELSE: stdin is pipe (or file)

		_gmutex.lock();

		if (!(_irc = _doc.load(is))) {
			cerr << "xconfig read: not istream! [" + string(_irc.description()) + "]" << endl;
			_gmutex.unlock(); return 1;
		}


		_gmutex.unlock();

		this->check();

		return 0;
	}



	int t_gsetbase::write(const string& file)
	{
		_gmutex.lock();

		ofstream of;
		string name(file);  
		try {
			of.open(name.c_str());
		}
		catch (fstream::failure e) {
			cerr << "Warning: Exception opening file " << name << ": " << e.what() << endl;
			_gmutex.unlock(); return -1;
		}

		_doc.save_file(name.c_str(), _delimiter.c_str(), pugi::format_indent);

		of.close();

		if (_log) _log->comment(0, "gsetbase", "XML-config saved: " + name);
		else               cout << "gsetbase:  XML-config saved: " + name << endl;

		_gmutex.unlock(); return 0;
	}


	int t_gsetbase::write_ostream(ostream& os)
	{
		_gmutex.lock();

		_doc.save(os, _delimiter.c_str(), pugi::format_indent);

		_gmutex.unlock(); return 0;
	}


	string t_gsetbase::strval(const string& elem, const string& subelem)
	{
		_gmutex.lock();

		string tmp = _strval(elem, subelem);

		_gmutex.unlock(); return tmp;
	}



	void t_gsetbase::usage()
	{
		cout << endl << app() << endl;
		cout << endl << "Usage: "
			<< endl
			<< endl << "    -h|--help              .. this help                          "
			<< endl << "    -V int                 .. version                            "
			<< endl << "    -v int                 .. verbosity level                    "
			<< endl << "    -x file                .. configuration input file           "
			<< endl << "    --                     .. configuration from stdinp          "
			<< endl << "    -l file                .. log output file                    "
			<< endl << "    -X                     .. output default configuration in XML"
			<< endl << endl;

		exit(0); return;
	}


	void t_gsetbase::arg(int argc, char *argv[], bool add, bool thin)
	{
		string conf("");
		string save("");
		bool stdi = false; // ENFORCE STDINP


		for (int i = 1; i < argc; ++i)
		{
			if (!strcmp(argv[i], "--help") && i < argc) { usage(); exit(0); }
			else if (!strcmp(argv[i], "--") && i < argc) { stdi = true; }
			else if (!strcmp(argv[i], "-h") && i < argc) { usage(); exit(0); }
			else if (!strcmp(argv[i], "-V") && i < argc) { cout << app() << endl; exit(0); }
			else if (!strcmp(argv[i], "-X") && i < argc) { help(); exit(0); }
			else if (!strcmp(argv[i], "-x") && i + 1 < argc) { conf = argv[++i]; }
			else if (!strcmp(argv[i], "-Z") && i + 1 < argc) { save = argv[++i]; }

				// warning if no other commandline option is supported
			else if (!add) fprintf(stderr, "Unknown option: %s\n", argv[i]);
		}

		if (!conf.empty()) {
			if (read(conf)) { exit(1); } // file XML
		}
		else {
			if (stdi) {
				if (read_istream(cin) > 0) { this->help();           exit(1); } // pipe XML
				else if (read_istream(cin) < 0) { cerr << "use --help\n"; exit(1); } // term
			}
			else {
				usage(); exit(1);
			}
		}

		// add/replace command-line option in XML
		for (int i = 1; i < argc; ++i) {
			xml_node NODE = _default_node(_doc, "config", "", true);

			string s = argv[i]; size_t pos1 = 0, pos2 = 0;
			if (s[0] == '-') continue;

			while (((pos1 = s.find(':', pos2)) != string::npos ||       // i+1 < argc ... used later
				(pos1 = s.find('=', pos2)) != string::npos))     // ORDER IMPORTANT
			{
				if ((pos2 = s.find(':', pos1 + 1)) != string::npos) {
					NODE = _default_node(NODE, s.substr(pos1 + 1, pos2 - pos1 - 1).c_str(), "", true);

				}
				else if ((pos2 = s.find('=', pos1 + 1)) != string::npos) {
					_default_attr(NODE, s.substr(pos1 + 1, pos2 - pos1 - 1).c_str(), s.substr(pos2 + 1, string::npos), true);
					pos2 = string::npos;

				}
				else {
					if (i + 1 < argc) _default_node(NODE, s.substr(pos1 + 1, string::npos).c_str(), argv[++i], true); // only if exists
					else           cerr << "Incomplete command-line argument\n";
					pos2 = string::npos;
				}
			}
		}

		if (thin) {
			xml_node node = _doc.child(XMLKEY_ROOT);
			_default_attr(node, "thin", "true", true);
		}
		//？？
		this->check();

		if (!conf.empty()) { if (_log) _log->comment(0, "gsetbase", "xconfig: read from file " + _name); }
		else { if (_log) _log->comment(0, "gsetbase", "xconfig: read from istream"); }

		if (!save.empty()) {
			if (_log) _log->comment(0, "gsetbase", "xconfig: save to file " + _name);
			write(save);
		}

		// COMMENTS
		if (_log && !thin) {
			_log->comment(0, "gsetbase", "program: " + app());
			_log->comment(0, "gsetbase", "==========");
			_log->comment(1, "gsetbase", "setting: " + conf);
			_log->comment(1, "gsetbase", "logfile: " + _log->mask());
			_log->comment(1, "gsetbase", "verbose: " + int2str(_log->verb()));
			_log->comment(1, "gsetbase", "==========");
		}

		return;
	}


	void t_gsetbase::app(const string& pgm, const string& ver,
		const string& rev, const string& own,
		const string& dat, const string& tim)
	{
		_gmutex.lock();

		_pgm = pgm;
		_ver = ver;
		_rev = rev;
		_own = own;
		_dat = dat;
		_tim = tim;

		_gmutex.unlock(); return;
	}



	string t_gsetbase::app()
	{
		_gmutex.lock();

		string tmp = _pgm + " [" + _ver + "] compiled: " + _dat + " " + _tim + " (" + _rev + ")";

		_gmutex.unlock(); return tmp;

	};


	string t_gsetbase::pgm()
	{
		_gmutex.lock();

		string tmp = _pgm;

		_gmutex.unlock(); return tmp;

	};




	set<string> t_gsetbase::setval(const string& elem, const string& subelem)
	{
		_gmutex.lock();

		set<string> tmp = _setval(elem, subelem);

		_gmutex.unlock(); return tmp;
	}



	vector<string> t_gsetbase::vecval(const string& elem, const string& subelem)
	{
		_gmutex.lock();

		vector<string> tmp = _vecval(elem, subelem);

		_gmutex.unlock(); return tmp;
	}


	bool t_gsetbase::thin()
	{
		_gmutex.lock();

		bool tmp = _doc.child(XMLKEY_ROOT).attribute("thin").as_bool();

		_gmutex.unlock(); return tmp;
	}



	double t_gsetbase::_dblatt(const string& elem, const string& attrib)
	{
		double num = 0.0;

		num = _doc.child(XMLKEY_ROOT).child(elem.c_str()).attribute(attrib.c_str()).as_double();

		return num;
	}


	double t_gsetbase::_dblval(const string& elem, const string& subelem)
	{
		double num = 0.0;

		istringstream is(_doc.child(XMLKEY_ROOT).child(elem.c_str()).child_value(subelem.c_str()));

		if (is >> num)
		{
			return num;
		}

		return num;
	}


	string t_gsetbase::_strval(const string& elem, const string& subelem)
	{
		string word;

		istringstream is(_doc.child(XMLKEY_ROOT).child(elem.c_str()).child_value(subelem.c_str()));

		if (is >> word) {
			transform(word.begin(), word.end(), word.begin(), ::toupper);
			return word;
		}

		return "";
	}


	set<string> t_gsetbase::_setval(const string& elem, const string& subelem)
	{
		set<string> vals;
		string word;
		//string ELEM = elem; transform(ELEM.begin(), ELEM.end(), ELEM.begin(), ::toupper);
		istringstream is(_doc.child(XMLKEY_ROOT).child(elem.c_str()).child_value(subelem.c_str()));
		while (is >> word) 
		{
			transform(word.begin(), word.end(), word.begin(), ::toupper);
			vals.insert(word);
		}

		return vals;
	}




	vector<string> t_gsetbase::_vecval(const string& elem, const string& subelem)
	{
		vector<string> vec;
		string word;

		istringstream is(_doc.child(XMLKEY_ROOT).child(elem.c_str()).child_value(subelem.c_str()));
		while (is >> word)
		{
			transform(word.begin(), word.end(), word.begin(), ::toupper);
			vec.push_back(word);
		}

		return vec;
	}



	xml_node t_gsetbase::_default_node(xml_node& node, const char* n, const char* val, bool reset)
	{



		string s(n);
		transform(s.begin(), s.end(), s.begin(), ::tolower);

		// remove if to be reset
		if (strcmp(val, "") && reset) node.remove_child(s.c_str());



		xml_node elem = node.child(s.c_str());



		if (!elem)
		{
			reset = true;
			elem = node.append_child(s.c_str());
		}

		if (!elem)
		{
			if (_log) _log->comment(0, "gsetbase", "warning - cannot create element " + s);
			else               cerr << "gsetbase:  warning - cannot create element " + s + "\n";
		}

		if (elem && strcmp(val, "") && reset)
		{
			elem.append_child(pugi::node_pcdata).set_value(val);
		}

		return elem;
	}



	void t_gsetbase::_default_attr(xml_node& node, const char* n, const string& v, bool reset)
	{
		if (node.attribute(n).empty()) node.append_attribute(n);
		if (strlen(node.attribute(n).value()) == 0 || reset) node.attribute(n).set_value(v.c_str());

		string s = node.attribute(n).value();
		transform(s.begin(), s.end(), s.begin(), ::tolower);
		node.attribute(n).set_value(s.c_str());
	}


	void t_gsetbase::_default_attr(xml_node& node, const char* n, const bool& v, bool reset)
	{
		if (node.attribute(n).empty()) node.append_attribute(n);
		if (strlen(node.attribute(n).value()) == 0 || reset) node.attribute(n).set_value(v);
	}

	void t_gsetbase::_default_attr(xml_node& node, const char* n, const int& v, bool reset)
	{
		if (node.attribute(n).empty()) node.append_attribute(n);
		if (strlen(node.attribute(n).value()) == 0 || reset) node.attribute(n).set_value(v);
	}



	void t_gsetbase::_default_attr(xml_node& node, const char* n, const double& v, bool reset)
	{
		if (node.attribute(n).empty()) node.append_attribute(n);
		if (strlen(node.attribute(n).value()) == 0 || reset) node.attribute(n).set_value(v);
	}

  
	void t_gsetbase::_add_log(string element, string msg)
	{
		map<string, set<string> >::iterator itELEM = _chache_log.find(element);
		if (itELEM != _chache_log.end())
		{
			itELEM->second.insert(msg);
		}
		else
		{
			set<string> msgs;
			msgs.insert(msg);
			_chache_log[element] = msgs;
		}

	}


	void t_gsetbase::help_header()
	{
		cerr << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?> \n"
			<< "<!DOCTYPE config>\n\n"
			<< "<config>\n";
	}


	void t_gsetbase::help_footer()
	{
		cerr << "</config>\n";
	}

} // namespace
