#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

#include "logging.h"

ofstream logfile;

logging loglevel;

bool logging_suppress;


void log_cleanup()
{
	if (logfile.is_open()) {
		logfile << '\n' << setw(80) << setfill('=') << "=" << "\n";
		logfile.close();
	}
}



void open_log(const string & filename, const logging level, const string & starttext)
{
    logfile.open(filename, ios_base::app);

	atexit(&log_cleanup);

	time_t runtime = time(NULL);

	logfile << starttext << "\t\t - session run at " << ctime(&runtime) << endl;
	
	loglevel = level;
}

string logsuppress(const string & s)
{
	if (logging_suppress) {
		logging_suppress = false;
		return ">>not shown<<";
	}
	else
		return s;
}