#include <iostream>
#include <string>
#include <iomanip>

#include "logging.h"
#include "studio_item.h"

using namespace std;

Item::Item(ifstream &f) : Thing(f), file(f)
{
}

Item::~Item()
{
}

extern 

bool Item::read_string(int & count)
{
	store_data(file, count);
	if (file.eof()) {
		logfile << "Item::process -- ERROR in reading item text.\n\t-- tried reading ";
		logfile << count << " bytes from offset " << offset << " but reached end of file." << endl;
		return false;
	}
	else
		return true;
}


/*  Items
 * 	-----
 * 
 *  0	| uint32		|	|	item type?
 * 	1	| uint8 		| L |	length of text
 *  2	| L bytes		|	|	text (unity asset file)
 *  3	| uint8			| M |	length of text
 *  4	| M bytes		|	|	text (name of asset)
 *  5	| uint8			|	|	??? (usually 0)
 *  6	| uint8			| N |	length of text
 *  7	| N bytes		|	|	object name
 *  8	| uint32		|	|	???
 *  9	| float32		|	|	x position
 *  10	| float32		|	|	y position
 *  11	| float32		|	|	z position
 *  12	| float32		|	|	x rotation (radians)
 *  13	| float32		|	|	y rotation (radians)
 *  14	| float32		|	|	z rotation (radians)
 *  15	| float32		|	|	??? (usually 1.0)
 *  16	| float32		|	|	x scaling
 *  17	| float32		|	|	y scaling
 *  18	| float32		|	|	z scaling
 *	19	| uint8			|	|	visiblity toggle
 */

bool Item::process(Midpoint & midpoint)
{
	int count;
	
	//  item type?
	this->store_uint32(file);
	
	//  text (unity asset file)
	count = this->store_uint8(file);
	if ( !read_string(count) )
		return false;
	
	//  text (name of asset)
	count = this->store_uint8(file);
	if ( !read_string(count) )
		return false;
	
	//	??? (usually 0)
	this->store_uint8(file);
	
	//	text (object name)
	count = this->store_uint8(file);
	if ( !read_string(count) )
		return false;
	
	//  ???
	this->store_uint32(file);
	
	float x, y, z, w;
	
	//	x,y,z location
	x = this->store_float(file);
	y = this->store_float(file);
	z = this->store_float(file);
	
	set_location(x, y, z);
	midpoint.adjust(x, y, z);
	
	log_spatial("LOCATION", x_loc, y_loc, z_loc);
	
	//	x,y,z rotation
	x = this->store_float(file);
	y = this->store_float(file);
	z = this->store_float(file);
	w = this->store_float(file);
	
	set_rotation(x, y, z, w);

	log_spatial("ROTATION", x_rot, y_rot, z_rot, w_rot);

	//	x,y,z scaling
	x = this->store_float(file);
	y = this->store_float(file);
	z = this->store_float(file);
	
	set_scaling(x, y, z);

	log_spatial("SCALING", x_scal, y_scal, z_scal);

	//	visiblity toggle
	this->store_uint8(file);


	return true;
}

ostream& operator<< (ostream & out, const Item & in)
{
	unsigned int index = 0;
	while ( index < in.data.size() ) {
		switch (index) {
			case 12:
			case 13:
			case 14:
				out << *(float *)(in.data[index]) << " -- " << internal2degrees(*(float *)(in.data[index])) << "\n";
				break;
			default:
				in.data_output(out, index);
		}
		index++;
	}
    out << endl;
	return out;
}

bool Item::write(std::ofstream & out)
{
	//	load spatial data back into the objects data array
	
	fix_rotation();

	*(float *)(data[9]) = x_loc;
	*(float *)(data[10]) = y_loc;
	*(float *)(data[11]) = z_loc;
	
	*(float *)(data[12]) = x_rot;
	*(float *)(data[13]) = y_rot;
	*(float *)(data[14]) = z_rot;
	*(float *)(data[15]) = w_rot;
	
	*(float *)(data[16]) = x_scal;
	*(float *)(data[17]) = y_scal;
	*(float *)(data[18]) = z_scal;
	
	//  write data array to disk
	
	ofstream::streampos pos = out.tellp();
	
	for (int i=0; i<data.size(); i++)
		data_save(out, i);
	
	logfile << "Item::write -- wrote " << out.tellp() - pos << " bytes." << endl;
	
	return true;

}
