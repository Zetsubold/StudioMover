#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

#include "studio.h"
#include "logging.h"

SceneFile::SceneFile(const string &f) : base(0)
{
	string::size_type pos = f.find_last_of("\\");
	pathname = f.substr(0,pos+1);
	filename = f.substr(pos+1,f.length());
	
	image_length = 0;
	file_length = 0;
	
	logfile << "SceneFile::SceneFile\n-- [PATH] " << pathname << "\n-- [FILE] " << filename << endl;
}

unsigned int read_BE32 (ifstream & file)
{
	unsigned int data;
	file.read(reinterpret_cast<char *>(&data), 4);
	
	unsigned int result = 0;
	result |= (data & 0x000000ff) << 24;
	result |= (data & 0x0000ff00) << 8;
	result |= (data & 0x00ff0000) >> 8;
	result |= (data & 0xff000000) >> 24;
	
	return result;
}

bool SceneFile::process_image(ifstream & file)
{
	unsigned int running = 0;

	//	check the PNG header
	unsigned char PNG_header[] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
	unsigned char * header = new unsigned char [8];
	file.read( reinterpret_cast<char *>(header), 8 );
	bool match;
	for (int i=0; i!=8; i++) {
		match = ( header[i]==PNG_header[i] );
		if (!match) break;
	}
	if (!match) {
		logfile << "SceneFile::process_image -- invalid .png file\n-- [FILE] " << filename << endl;
	}
	
	running+=8;

	//	check that scene data exists
	bool more_data = true;
	while (more_data) {
		unsigned int chunk_length = read_BE32(file);
		unsigned int chunk_type = read_BE32(file);
		
		running+=8;
	
		//	skip over the chunk data
		file.seekg(chunk_length, ios::cur);
		running+=chunk_length;
		
		unsigned int chunk_chksum = read_BE32(file);
		
		running+=4;
		
		more_data = ( file.eof() == false ) and ( chunk_type != 0x49454e44 ) ;		//	reached the end of file or IEND chunk?
		
		if (loglevel==logging::extra) {
			logfile << showbase << hex;
			logfile << setw(10) << chunk_length << " -- ";
			logfile << setw(10) << chunk_type << " -- ";
			logfile << setw(10) << chunk_chksum << " - ";
			logfile << noshowbase << dec;
			logfile << "\tp = " << setw(15) << file.tellg() << "\tr = " << setw(15) << running << endl;
		}
		
	}
	
	logfile << noshowbase << dec;
	
	//	did we reach the end of the file unexpectedly?
	if (file.eof()) {
		logfile << "SceneFile::process_image -- ERROR unexpected end of file reached.\n-- [FILE] " << filename << endl;
		return false;
	}
	
	image_length = file.tellg();
	file.seekg(0,ios::end);		//	find the actual end of the file
	file_length = file.tellg();
	
	if ( image_length == file_length ) {
		logfile << "SceneFile::process_image -- file is a normal PNG.\n-- [FILE] " << filename << endl;
		return false;
	}
	
	file.seekg(image_length); 	//	put us back at the end of the image
	return true;
}

/*	Honey Select studio file format
 * 
 *  uint32		|	|	Version?
 *  uint32		|	|	objects in male group
 * 
 *  0	| uint32		|	|	Version?
 * 	1	| uint32 		| M |	count of objects in male group
 *  	| ??? bytes		|	|	M blocks of male data
 * 	2	| uint32 		| F |	count of objects in female group
 *  	| ??? bytes		|	|	F blocks of female data
 * 	3	| uint32 		| I |	count of objects in iyem group
 *  	| ??? bytes		|	|	I blocks of item data
 *  4	| 345 bytes		|   |	other scene data
 * 	
 * 
 * 	groups are in order males, females, items
 * 
 */


bool SceneFile::process_studio(ifstream & file)
{
	bool on_fire = false;
	
	base.update_offset(file);
	
	base.store_uint32(file);							//	version
	
	unsigned int mcount = base.store_uint32(file);		//	male count
	if (mcount > 0) {
		logfile << mcount <<" Guys!" << endl;
		
		for (unsigned int i=0; i<mcount; i++) {
			
			Male * guy = new Male(file);
			
			if (guy->process(males.midpoint)) {
				logfile << *guy;
				males.store(*guy);
			}
			else {
				logfile << *guy;
				logfile << "SceneFile::process_studio -- ERROR in processing male." << endl;
				on_fire = true;
				break;
			}
		}
		
		if (!on_fire)
			logfile << "read in " << mcount << " guys." << endl;
	}
	
	if (on_fire) return false;
	
	
	unsigned int fcount = base.store_uint32(file);		//	female count
	if (fcount > 0) {
		logfile << fcount <<" Gals!" << endl;
		
		for (unsigned int i=0; i<fcount; i++) {
			
			Female * gal = new Female(file);
			
			if (gal->process(females.midpoint)) {
				logfile << *gal;
				females.store(*gal);
			}
			else {
				logfile << *gal;
				logfile << "SceneFile::process_studio -- ERROR in processing female." << endl;
				on_fire = true;
				break;
			}
		}
		
		if (!on_fire)
			logfile << "read in " << fcount << " gals." << endl;
	}
	
	if (on_fire) return false;


	unsigned int icount = base.store_uint32(file);		//	item count
	if (icount > 0) {
		logfile << icount <<" Items!" << endl;
		
		for (unsigned int i=0; i<icount; i++) {
			
			Item * item = new Item(file);
			
			if (item->process(items.midpoint)) {
				logfile << *item;
				items.store(*item);
			}
			else {
				logfile << *item;
				logfile << "SceneFile::process_studio -- ERROR in processing item." << endl;
				on_fire = true;
				break;
			}
		}
		
		if (!on_fire)
			logfile << "read in " << icount << " items." << endl;

	}
	
	if (on_fire) return false;
	
	//	set the scene midpoint based on the ones in the object groups.
	//	--	remember that a midpoint is an upper and lower bound until we .fetch() it.
	midpoint.adjust(males.midpoint);
	midpoint.adjust(females.midpoint);
	midpoint.adjust(items.midpoint);
	
	base.store_data(file, 345, true);
	
	if (file.eof()) {
		logfile << "SceneFile::process_studio -- ERROR in reading post-item data.\n\t-- tried reading ";
		logfile << 345 << " bytes from offset " << base.offset << " but reached end of file." << endl;
		return false;
	}

	if ( file.tellg() == file_length ) {
		logfile << "Read in scene file successfully." << endl;
		return true;
	}
	else {
		logfile << "SceneFile::process_studio -- ERROR in reading scene file.\n";
		logfile << "\t-- at position " << file.tellg() << " of " << file_length << endl;
		return false;
	}
		
}


void SceneFile::write_studio(ofstream & out)
{
	int count;
	
	ofstream::streampos pos = out.tellp();
	
	out.write((char *)(base.data[0]), 4);
	
	count = *(unsigned int *)(base.data[1]);
	out.write((char *)(base.data[1]), 4);
	
	if ( count > 0 ) {
		males.write(out, count);
	}
	
	count = *(unsigned int *)(base.data[2]);
	out.write((char *)(base.data[2]), 4);
	
	if ( count > 0 ) {
		females.write(out, count);
	}
	
	count = *(unsigned int *)(base.data[3]);
	out.write((char *)(base.data[3]), 4);
	
	if ( count > 0 ) {
		items.write(out, count);
	}

	out.write((char *)(base.data[4]), 345);
	
	logfile << "SceneFile::write_studio -- wrote " << out.tellp() - pos << " bytes." << endl;
}

void SceneFile::process_transforms(TransformGroup * tgroup)
{
	males.transform(tgroup);
	
	females.transform(tgroup);
	
	items.transform(tgroup);
}



bool SceneFile::process()
{
	string fullname = pathname + filename;
	
	ifstream file(fullname, ios::binary);

	if (process_image(file))
		if (process_studio(file)) {
			
			string movename = fullname + ".StudioMover.txt";
			
			TransformGroup * tgroup = load_transforms(movename);
			
			if ( tgroup != nullptr ) {
			
				string outname = pathname + filename.substr(0,filename.size()-4) + "_MOVED.png";
				ofstream out(outname, ios::binary);
				
				process_transforms(tgroup);
				
				file.seekg(0);
				char * imagedata = new char[image_length];
				file.read(imagedata, image_length);
				out.write(imagedata, image_length);
				
				write_studio(out);
				
			}
			else {
				 cout << "Either " << filename << ".StudioMover.txt is missing, or it has no usable entries." << endl;
				 logfile << "-- [FILE] " << movename << endl;
				 return false;
			}
			
		}
		
	
	file.close();
	
	return true;
}