

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include "gset/gsetgnss.h"
#include "gutils/gnss.h"
#include "gutils/gobs.h"
#include "gutils/gsys.h"

using namespace std;
using namespace pugi;

namespace gnut
{

	t_gsetgnss::t_gsetgnss()
		: t_gsetbase()
	{
		_set.insert(XMLKEY_GNSS);

		
		stringstream os;
		
		t_map_gnss gnss_data = GNSS_DATA_PRIORITY();
		for (auto itGNSS = gnss_data.begin(); itGNSS != gnss_data.end(); ++itGNSS) // using C++11 initializer
		{
			GSYS gsys = itGNSS->first;
			string gs = t_gsys::gsys2str(gsys);

			for (auto itBAND = gnss_data[gsys].begin();
				itBAND != gnss_data[gsys].end();
				++itBAND)
			{
				GOBSBAND gobsband = itBAND->first;
				string band = gobsband2str(gobsband);
				_band_str[gsys].push_back(band);

				for (auto itTYPE = gnss_data[gsys][gobsband].begin();
					itTYPE != gnss_data[gsys][gobsband].end();
					++itTYPE)
				{
					GOBSTYPE gobstype = itTYPE->first;
					string type = gobstype2str(gobstype);
					os << gs << "  band: " << band << "  type: " << type << "  attr:";

					// ONLY THOSE NOT YET INCLUDED
					set<string> type_search(_type_str[gsys].begin(), _type_str[gsys].end());
					if (type_search.find(type) == type_search.end())
					{

						_type_str[gsys].push_back(type);
						
					}

					for (auto itATTR = gnss_data[gsys][gobsband][gobstype].begin();
						itATTR != gnss_data[gsys][gobsband][gobstype].end();
						++itATTR)
					{
						GOBSATTR gobsattr = *itATTR;
						string attr = gobsattr2str(gobsattr);
						os << " " << attr;

						// ONLY THOSE NOT YET INCLUDED
						set<string> attr_search(_attr_str[gsys].begin(), _attr_str[gsys].end());
						if (attr_search.find(attr) == attr_search.end())
						{
							_attr_str[gsys].push_back(attr);
							
						}
					}
					os << endl;
				}
			}
			os << endl;
		}


	}



	t_gsetgnss::~t_gsetgnss()
	{}



	set<string> t_gsetgnss::sat()
	{
		_gmutex.lock();

		set<string> tmp;
		set<string>::const_iterator it;

		t_map_sats gnss_sats = GNSS_SATS();
		t_map_sats::const_iterator itGNS;
		for (itGNS = gnss_sats.begin(); itGNS != gnss_sats.end(); ++itGNS)
		{
			GSYS gsys = itGNS->first;
			string gs = t_gsys::gsys2str(gsys);


			transform(gs.begin(), gs.end(), gs.begin(), ::towlower);
			set<string> sats = t_gsetbase::_setval(gs, "sat");


			for (it = sats.begin(); it != sats.end(); ++it) tmp.insert(*it);

		}

		_gmutex.unlock(); return tmp;
	}



	set<string> t_gsetgnss::sat(GSYS gsys, bool def) // default=true enforce to fill 
	{
		_gmutex.lock();

		set<string> tmp = t_gsetbase::_setval(_gsys(gsys), "sat");

		// AUTO SET
		if (def && tmp.size() == 0) tmp = GNSS_SATS()[gsys];

		_gmutex.unlock(); return tmp;
	}



	set<string> t_gsetgnss::nav(GSYS gsys, bool def) // default=true enforce to fill 
	{
		_gmutex.lock();
		set<string> tmp = t_gsetbase::_setval(_gsys(gsys), "nav");

		// AUTO SET
		if (def && tmp.size() == 0) tmp = GNSS_GNAV()[gsys];

		_gmutex.unlock(); return tmp;
	}




	set<string> t_gsetgnss::obs(GSYS gsys, bool def)
	{
		_gmutex.lock();

		string str;
		set<string> tmp;

		vector<string> type = t_gsetbase::_vecval(_gsys(gsys), "type");
		vector<string> band = t_gsetbase::_vecval(_gsys(gsys), "band");
		vector<string> attr = t_gsetbase::_vecval(_gsys(gsys), "attr");

		vector<string>::const_iterator itT;
		vector<string>::const_iterator itB;
		vector<string>::const_iterator itA;

		string gs = t_gsys::gsys2str(gsys);
		ostringstream os;
		os << gs << "  band: " << band.size() << "  type: " << type.size() << "  attr: " << attr.size() << endl;

		int bset = band.size();
		int tset = type.size();
		int aset = attr.size();

		// AUTO SET
		if (bset == 0) band.assign(_band_str[gsys].begin(), _band_str[gsys].end());
		if (tset == 0) type.assign(_type_str[gsys].begin(), _type_str[gsys].end());
		if (aset == 0) attr.assign(_attr_str[gsys].begin(), _attr_str[gsys].end());

		for (itB = band.begin(); itB != band.end(); ++itB) {
			GOBSBAND gband = char2gobsband((*itB)[0]);
			string       b = gobsband2str(gband);
			if (gband == 999) continue;

			for (itT = type.begin(); itT != type.end(); ++itT) {
				GOBSTYPE gtype = char2gobstype((*itT)[0]);
				string       t = gobstype2str(gtype);
				if (gtype == 999) continue;

				os << gs << "  band: " << b << "  type: " << t << "  attr:";

				for (itA = attr.begin(); itA != attr.end(); ++itA) {
					GOBSATTR gattr = char2gobsattr((*itA)[0]);
					string       a = gobsattr2str(gattr);
					if (gattr == 999) continue;

					str = gobstype2str(gtype);
					str += gobsband2str(gband);
					str += gobsattr2str(gattr);

					if (def || (bset || tset || aset)) tmp.insert(str);
					
					os << " " << a;

				}
				os << endl;
			}
		}


		_gmutex.unlock(); return tmp;
	}


	set<string> t_gsetgnss::gobs(GSYS gsys)
	{
		_gmutex.lock();

		string str;
		set<string> tmp;

		vector<string> vgobs = t_gsetbase::_vecval(_gsys(gsys), "gobs");

		for (auto it = vgobs.begin(); it != vgobs.end(); it++)
		{
			tmp.insert(*it);
		}

		_gmutex.unlock(); return tmp;
	}



	vector<GOBSTYPE> t_gsetgnss::type(GSYS gsys)
	{
		_gmutex.lock();

		vector<GOBSTYPE> v_tmp = _type(gsys);

		_gmutex.unlock(); return v_tmp;
	}


	vector<GOBSBAND> t_gsetgnss::band(GSYS gsys)
	{
		_gmutex.lock();

		vector<GOBSBAND> v_tmp = _band(gsys);

		_gmutex.unlock(); return v_tmp;
	}



	vector<GOBSATTR> t_gsetgnss::attr(GSYS gsys)
	{
		_gmutex.lock();

		vector<GOBSATTR> v_tmp = _attr(gsys);

		_gmutex.unlock(); return v_tmp;
	}


	void t_gsetgnss::check()
	{
		_gmutex.lock();

		_gmutex.unlock(); return;
	}

	vector<FREQ_SEQ> t_gsetgnss::freqs(GSYS gsys)
	{
		_gmutex.lock();

		vector<FREQ_SEQ> v_tmp = _freqs(gsys);

		if (v_tmp.empty())
		{
			v_tmp.push_back(FREQ_1);
			v_tmp.push_back(FREQ_2);
		}

		_gmutex.unlock(); return v_tmp;
	}

	map<FREQ_SEQ, GOBSBAND> t_gsetgnss::band_index(GSYS gsys)
	{
		auto XmlFreq = freqs(gsys);
		auto XmlBand = band(gsys);

		if (XmlFreq.size() != XmlBand.size())
		{
			throw logic_error("Band size should be the same with freq size in xml.");
		}

		map<FREQ_SEQ, GOBSBAND> tmp;
		for (int i = 0; i < XmlBand.size(); i++)
		{
			tmp[XmlFreq[i]] = XmlBand[i];
		}

		return tmp;
	}

	map<GOBSBAND, FREQ_SEQ> t_gsetgnss::freq_index(GSYS gsys)
	{
		auto XmlFreq = freqs(gsys);
		auto XmlBand = band(gsys);

		if (XmlFreq.size() != XmlBand.size())
		{
			throw logic_error("Band size should be the same with freq size in xml.");
		}

		map<GOBSBAND, FREQ_SEQ> tmp;
		for (int i = 0; i < XmlBand.size(); i++)
		{
			tmp[XmlBand[i]] = XmlFreq[i];
		}

		return tmp;
	}


	vector<FREQ_SEQ> t_gsetgnss::_freqs(GSYS gsys)
	{
		vector<string> v_str = t_gsetbase::_vecval(_gsys(gsys), "freq");
		vector<FREQ_SEQ> v_tmp;
		for (vector<string>::const_iterator it = v_str.begin(); it != v_str.end(); ++it)
		{
			string str = *it;
			transform(str.begin(), str.end(), str.begin(), ::toupper);
			FREQ_SEQ gobsband = str2gnssfreq(str);
			if (gobsband != FREQ_SEQ::FREQ_X)
			{
				v_tmp.push_back(gobsband);
			}
		};

		if (v_tmp.empty())
		{
			v_tmp.push_back(FREQ_1);
			v_tmp.push_back(FREQ_2);
			v_tmp.push_back(FREQ_3);
		}

		return v_tmp;
	}

	string t_gsetgnss::_gsys(GSYS gsys)
	{
		string tmp = t_gsys::gsys2str(gsys);
		transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
		return tmp;
	}



	vector<GOBSTYPE> t_gsetgnss::_type(GSYS gsys)
	{
		vector<string> v_str = t_gsetbase::_vecval(_gsys(gsys), "type");
		vector<GOBSTYPE> v_tmp;
		for (vector<string>::const_iterator it = v_str.begin(); it != v_str.end(); ++it) {
			string str = *it;
			transform(str.begin(), str.end(), str.begin(), ::toupper);
			GOBSTYPE gobstype = str2gobstype(str);
			if (gobstype != TYPE) v_tmp.push_back(gobstype);
		};
		return v_tmp;
	}


	vector<GOBSBAND> t_gsetgnss::_band(GSYS gsys)
	{
		vector<string> v_str = t_gsetbase::_vecval(_gsys(gsys), "band");
		vector<GOBSBAND> v_tmp;
		for (vector<string>::const_iterator it = v_str.begin(); it != v_str.end(); ++it)
		{
			string str = *it;
			transform(str.begin(), str.end(), str.begin(), ::toupper);
			GOBSBAND gobsband = str2gobsband(str);
			if (gobsband != BAND)
			{
				v_tmp.push_back(gobsband);
			}
		};


		if (v_tmp.empty())
		{
			switch (gsys)
			{
			case GPS:
				v_tmp.push_back(BAND_1);
				v_tmp.push_back(BAND_2);
				v_tmp.push_back(BAND_5);
				break;
			case GLO:
				v_tmp.push_back(BAND_1);
				v_tmp.push_back(BAND_2);
				v_tmp.push_back(BAND_3);
				break;
			case GAL:
				v_tmp.push_back(BAND_1);
				v_tmp.push_back(BAND_5);
				v_tmp.push_back(BAND_7);
				break;
			case BDS:
				v_tmp.push_back(BAND_2);
				v_tmp.push_back(BAND_7);
				v_tmp.push_back(BAND_6);
				break;
			case QZS:
				v_tmp.push_back(BAND_1);
				v_tmp.push_back(BAND_2);
				v_tmp.push_back(BAND_5);
			default:
				break;
			}

			if (_log) _log->comment(0, "Warning : do not set the band, use default settings");
		}

		return v_tmp;
	}



	vector<GOBSATTR> t_gsetgnss::_attr(GSYS gsys)
	{
		vector<string> v_str = t_gsetbase::_vecval(_gsys(gsys), "attr");
		vector<GOBSATTR> v_tmp;
		for (vector<string>::const_iterator it = v_str.begin(); it != v_str.end(); ++it) {
			string str = *it;
			transform(str.begin(), str.end(), str.begin(), ::toupper);
			GOBSATTR gobsattr = str2gobsattr(str);
			if (gobsattr != ATTR) v_tmp.push_back(gobsattr);
		};
		return v_tmp;
	}


	void t_gsetgnss::help()
	{
		_gmutex.lock();

		cerr << " <gps>            \t\t  <!-- any GNSS constellation: GPS GLO GAL BDS SBS QZS -->\n"
			<< "   <sat>  </sat>  \t\t  <!-- list of GPS satellites: G01 G02 .. or empty(ALL) -->\n"
			<< "   <type> </type> \t\t  <!-- list of GPS  obs types: C L D S P or empty(ALL)  -->\n"
			<< "   <band> </band> \t\t  <!-- list of GPS  obs bands: 1 2 5 or empty(ALL) -->\n"
			<< "   <attr> </attr> \t\t  <!-- list of PGS attributes: A B C D I L M N P Q S W X Y Z or empty(ALL) -->\n"
			<< " </gps>\n\n";

		cerr << " <glo>            \t\t  <!-- any GNSS constellation: GPS GLO GAL BDS SBS QZS -->\n"
			<< "   <sat>  </sat>  \t\t  <!-- list of GPS satellites: R01 R02 .. or empty(ALL) -->\n"
			<< "   <type> </type> \t\t  <!-- list of GPS  obs types: C L D S P or empty(ALL)  -->\n"
			<< "   <band> </band> \t\t  <!-- list of GPS  obs bands: 1 2 3 or empty(ALL) -->\n"
			<< "   <attr> </attr> \t\t  <!-- list of PGS attributes: A B C D I L M N P Q S W X Y Z or empty(ALL) -->\n"
			<< " </glo>\n\n";

		cerr << "\t<!-- gnss observation definition:\n"
			<< "\t      .. ID name\n"
			<< "\t name   .. full name\n"
			<< "\t desc   .. description\n"
			<< "\t -->\n\n";

		_gmutex.unlock(); return;
	}

} // namespace
