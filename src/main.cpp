#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

#include "dir_list.h"
#include "studio.h"
#include "logging.h"

#define VERSION "0.1.0"

/*	StudioMover
 * 	-----------
 * 
 * 	A program to spatially move things about a studio scene file. can also change the scale and rotate objects, too.
 * 
 * 	Use   - drag a studio file onto the program, it will then check for a .studiomover.txt file that contains the
 * 			instructions for the translation, rotation, and scaling operations. 
 * 
 *	v0.1.0		- initial development
 */

// my standard "do things to files" template


/*
bool process_file (const string & filename)
{
//    debug_bucket << filename << " | ";
	
	bool candidate = true;		// is file a deletion candidate?
	
    ifstream in(filename.c_str());
	string line;
	
	// read in each line and check for non default data

    while (!in.eof()) {
		
        getline(in, line);
		
		// skip empty lines
		if (line.length() == 0)
			continue;
			
		// check the data
        if (line.find("False,1,1,1,1")==string::npos) {
//			debug_bucket << line.length() << " ";
			candidate = false;
        }
    }
	
	in.close();
	// delete file if required
	if (candidate) {
		remove(filename.c_str());
//		debug_bucket << "deleted";
	}
	
//	debug_bucket << endl;
	
	return candidate;
}

        
void handle_files (const vector<FileListing> & input)
{
	int deletion_count = 0;
    vector<FileListing>::const_iterator iter;
	
    for (iter = input.begin(); iter != input.end(); iter++) {
		if ( process_file(iter->name) )
			deletion_count++;
    }
	
	cout << deletion_count << " files deleted." << endl;
}
*/

static void convert_uppercase (string & s)
{
	for (string::iterator iter=s.begin(); iter!=s.end(); iter++)
		*iter = toupper(*iter);
}

int main(int argc, char *argv[])
{
	ostringstream logheader;
	
	logheader << "StudioMover v" << VERSION;
	
	open_log("SceneMover log.txt", logging::full, logheader.str());
	
	//	do we even have input?
    if (argc==1) {
		logfile <<	"ERROR - no input file given, session aborted." << endl;
		cout << "NO INPUT FILE DETECTED" << endl;
		cout << "Try dragging a scene file onto the program. :)" << endl;
		system("PAUSE");
		return EXIT_FAILURE;
	}
	
	vector<SceneFile> scenes;
       
    //	iterate through all files on command line

    for (int i=1; i<argc; i++) {
        string argstring = argv[i];
        
        //	check for path or just file name
        string::size_type pos = argstring.find_last_of("\\");
		
		bool isfolder = (pos==string::npos);
		
		/*	TODO - check the actual directory entry if the simple test fails */
        
        if (isfolder) {
			//  handling a whole folder is not implemented
			logfile <<	"ERROR - cannot handle whole folders, session aborted." << endl;
			cout << "HELP! A FOLDER DETECTED" << endl;
			cout << "I can't handle a whole folder yet. :)" << endl;
			system("PAUSE");
			return EXIT_FAILURE;
		}
        else {
			// make sure we have a proper PNG file and then store it in _scenes_
			string check = argstring;
			convert_uppercase(check);
			if ( check.rfind(".PNG") == ( check.size()-4 ) )
				scenes.push_back(SceneFile(argstring));
			else
				logfile << "SKIPPED, NOT .PNG FILE -- " << argstring << endl;
		}

    }
	
	
	//	Open each scene file and do things to it
	
	bool show_check_message = false;
	
	for (vector<SceneFile>::iterator iter=scenes.begin(); iter!=scenes.end(); iter++) {
		if ( iter->process() == false )
			show_check_message = true;
	}

	if ( show_check_message )
		cout << "Please check StudioMover.log for details" << endl;

    system("PAUSE");
    return EXIT_SUCCESS;

}
