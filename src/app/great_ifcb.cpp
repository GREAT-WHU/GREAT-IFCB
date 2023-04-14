
#include <chrono>
#include <signal.h>
#include "gset/gcfg_ifcb.h"
#include "gproc/gifcbproc.h"
#include "gcoders/biabernese.h"
#include "gcoders/ambflag.h"
#include "gcoders/rinexo.h"
#include "gcoders/rinexn.h"
#include "gio/gfile.h"
#include "gdata/gifcb.h"
#include "gcoders/ifcb.h"

using namespace std;
using namespace gnut;
using namespace great;
using namespace std::chrono;

void catch_signal(int) { cout << "Program interrupted by Ctrl-C [SIGINT,2]\n"; exit(1); }


// MAIN
// ----------
int main(int argc, char** argv)
{
	// Only to cout the Reminder here
	signal(SIGINT, catch_signal);

	// Creat and set the log file : ifcb.log
	t_glog glog; glog.mask("ifcb.log"); glog.cache_size(99); glog.tsys(t_gtime::GPS);
	glog.time_stamp(true);

	// Construct the gset class and init some values in the class
	t_gcfg_ifcb gset;

	gset.app("GREAT-IFCB", "1.0", "$Rev: 2448 $", "(hjzheng@whu.edu.cn)", __DATE__, __TIME__);

	// Get the arguments from the command line
	gset.arg(argc, argv, true, false);

	// Get sample intval from gset. if not, init with the default value
	int sample = int(dynamic_cast<t_gsetgen*>(&gset)->sampling());
	if (!sample) sample = int(dynamic_cast<t_gsetgen*>(&gset)->sampling_default());

	// Prepare input files list form gset
	multimap<IFMT, string> inp = gset.inputs_all();
	//get the beginning time and the ending time
	t_gtime begin = t_gtime(dynamic_cast<t_gsetgen*>(&gset)->beg());
	t_gtime end = t_gtime(dynamic_cast<t_gsetgen*>(&gset)->end());


	set<string> sys = dynamic_cast<t_gsetgen*>(&gset)->sys();

	//--- INITIALIZATIONS ---
	t_gdata*        gdata = nullptr;
	t_gallbias*     gbia  = nullptr;    if (gset.input_size("biabern") > 0 ) { gbia = new t_gallbias;  gbia->glog(&glog); }
	t_gallobs*      gobs = nullptr;     if (gset.input_size("rinexo") > 0) { gobs = new t_gallobs();  gobs->glog(&glog); gobs->gset(&gset); }
	t_gallnav*      gallnav = nullptr; if (gset.input_size("rinexn") > 0) { gallnav = new t_gallnav(); gallnav->glog(&glog); }
	

	// runepoch for the time costed each epoch (i guess)
	t_gtime runepoch(t_gtime::GPS);

	// lstepoch for the time of all epoches (i guess)
	t_gtime lstepoch(t_gtime::GPS);

	// DATA READING
	int tab = 0;
	multimap<IFMT, string>::const_iterator itINP = inp.begin();
	for (size_t i = 0; i < inp.size() && itINP != inp.end(); ++i, ++itINP)
	{
		tab++;
		// Get the file format/path, which will be used in decoder
		IFMT   ifmt(itINP->first);
		string path(itINP->second);
		string id("ID" + int2str(i));

		t_gio*    gio = 0;
		t_gcoder* gcoder = 0;

		// For different file format, we prepare different data container and decoder for them.   
		if (ifmt == RINEXO_INP){ gdata = gobs; gcoder = new t_rinexo(&gset, "", 4096); }
		else if (ifmt == RINEXN_INP) { gdata = gallnav; gcoder = new t_rinexn(&gset, "", 4096); }
		else if (ifmt == BIABERN_INP) { gdata = gbia; gcoder = new t_biabernese(&gset, "", 20480); }
		else { glog.comment(0, "main", "Error: unrecognized format in IFCB Estimation --- " + int2str(ifmt)); gdata = 0; }

		// Check the file path
		if (path.substr(0, 7) != "file://") {
			glog.comment(0, "main", "path is not file (skipped)!");
			continue;
		}

		// READ DATA FROM FILE
		if (gcoder) {
			// prepare gio for the file
			gio = new t_gfile;
			gio->glog(&glog);
			gio->path(path);

			// Put the file into gcoder
			gcoder->clear();
			gcoder->path(path);
			gcoder->glog(&glog);

			// Put the data container into gcoder
			gcoder->add_data(id, gdata);

			// Put the gcoder into the gio
			// Note, gcoder contain the gdata and gio contain the gcoder
			gio->coder(gcoder);

			runepoch = t_gtime::current_time(t_gtime::GPS);

			// Read the data from file here
			gio->run_read();
			lstepoch = t_gtime::current_time(t_gtime::GPS);

			// Write the information of reading process to log file
			glog.comment(0, "main", "READ: " + path + " time: "
				+ dbl2str(lstepoch.diff(runepoch)) + " sec");

			// Delete 
			delete gio;
			delete gcoder;
		}
	}

	//PROCESSING
	runepoch = t_gtime::current_time(t_gtime::GPS);

	// add all data
	t_gallproc* data = new t_gallproc();

	// BDS need to read brdm
	if (sys.find("BDS") != sys.end())
	{
		if (!data->Add_Data(gallnav)) return -1;
	}
	if (!data->Add_Data(gobs)) return -1;
	if (!data->Add_Data(gbia)) return -1;

	// ifcb est. structure
	t_gifcbproc*  gifcbest = new t_gifcbproc(&gset, data, &glog);

    glog.comment(0, "main", " processing started [ IFCB Estimation ]");
	// The main processing code : processBatch
	if (!gifcbest->processBatch(begin, end, sample))
	{
		cerr << "unknown mistake!" << endl;
		return -1;
	}

	// The time when process ends
	lstepoch = t_gtime::current_time(t_gtime::GPS);
	// Write the log file
	glog.comment(0, "main", " processing finished : duration  "
			+ dbl2str(lstepoch.diff(runepoch)) + " sec");

	// Encoder ifcb files
	gifcbest->GenerateProduct();

	//Delete pointer	
	if (gifcbest)      { delete gifcbest; gifcbest = nullptr; }
	if (gbia)         { delete gbia; gbia = nullptr; }
	if (gobs)         { delete gobs; gobs = nullptr; }
	if (gallnav)      { delete gallnav; gallnav = nullptr; }
	if (data)         { delete data; data = nullptr; }
	

	// Normal End
	glog.comment(0, "main", " Normal End! ");

	return 0;
}