#include <iostream>
#include <string>
#include <iomanip>

#include "logging.h"
#include "studio_male.h"

using namespace std;

Male::Male(ifstream &f) : Thing(f), file(f)
{
}

Male::~Male()
{
}

bool Male::read_string(int &count)
{
	store_data(file, count);
	if (file.eof()) {
		logfile << "Male::process -- ERROR in reading item text.\n\t-- tried reading ";
		logfile << count << " bytes from offset " << offset << " but reached end of file." << endl;
		return false;
	}
	else
		return true;
}


/*  Males
 * 	-----
 * 
 *  0	| uint32		|	|	???
 *  1	| uint8			| N	|	length of text
 * 	2	| N bytes 		|	|	text - "[HoneySelectCharaMale]"
 *  3	| uint32		|	|	???
 *  4	| uint32		|	|	???
 * 
 *  [5..10] [11..16] [17..22] [23..28] [29..34]
 *  	| 128 bytes		|	|	text label
 *  	| uint32		|	|	???
 *  	| uint32		|	|	offset ? [O1 to O5]
 *  	| uint32		|	|	???
 *  	| uint32		|	|	length ? [L1 to L5]
 *  	| uint32		|	|	???
 * 
 * 	[35..39]
 *  	| Lx bytes		|	|	data segments
 * 
 * 84 bytes
 * 
 *  41	| uint8			| N	|	length of text
 * 	42	| N bytes 		|	|	text - animation name
 *  43	| uint8			| N	|	length of text
 * 	44	| N bytes 		|	|	text - animation asset file
 *  45	| uint8			| N	|	length of text
 * 	46	| N bytes 		|	|	text - animation bank
 *  47	| uint8			| N	|	length of text
 * 	48	| N bytes 		|	|	text - animation internal name
 * 
 * 5 bytes
 * a float
 * 5 bytes
 * 
 *  52	| float32		|	|	x position
 *  53	| float32		|	|	y position
 *  54	| float32		|	|	z position
 *  55	| float32		|	|	x rotation (radians)
 *  56	| float32		|	|	y rotation (radians)
 *  57	| float32		|	|	z rotation (radians)
 *  58	| float32		|	|	??? (usually 1.0)
 *  59	| float32		|	|	x scaling
 *  60	| float32		|	|	y scaling
 *  61	| float32		|	|	z scaling
 * 
 * 636 bytes
 */

bool Male::process(Midpoint & midpoint)
{
	int count;
	
	vector<uint32_t> data_length;
	
	store_uint32(file);

	//	index 1 & 2 - [HoneySelectCharaMale]
	count = store_uint8(file);
	store_data(file, count);

	store_uint32(file);
	store_uint32(file);
	
	//	index 5...34 - 5 groups of label data + offse
	//	"Preview information"
	//	"Custom information"
	//	"Coordination information"
	//	"Status information"
	//	"Parameter information"
	for (count=0; count<5; count++) {
		store_data(file, 128, true);
		logfile << (char *)(data[data.size()-1]) << endl;
		
		store_uint32(file);
		store_uint32(file);								//	offset
		store_uint32(file);
		data_length.push_back( store_uint32(file) );	//	length
		store_uint32(file);
	}
	
	//	store data according to _datastore_
	
	for (count=0; count<5; count++) {
		store_data(file, data_length[count], true);
	}

	store_data(file, 80, true);

	//	animation name
	count = store_uint8(file);
	store_data(file, count);
	//	animation asset file
	count = store_uint8(file);
	store_data(file, count);
	//	animation bank
	count = store_uint8(file);
	store_data(file, count);
	//	animation internal name
	count = store_uint8(file);
	store_data(file, count);

	store_data(file, 5, true);
	
	store_float(file);

	store_data(file, 5, true);

	float x, y, z;
	
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
	float w = this->store_float(file);
	
	set_rotation(x, y, z, w);

	log_spatial("ROTATION", x_rot, y_rot, z_rot, w_rot);

	//	x,y,z scaling
	x = this->store_float(file);
	y = this->store_float(file);
	z = this->store_float(file);
	
	set_scaling(x, y, z);

	log_spatial("SCALING", x_scal, y_scal, z_scal);

	store_data(file, 571, true);

	return true;
}


ostream& operator<< (ostream & out, const Male & in)
{
	unsigned int index = 0;
	while ( index < in.data.size() ) {
		switch (index) {
			case 5:
			case 11:
			case 17:
			case 23:
			case 29:
				out << (char *)(in.data[index]) << "\n";
				break;
				
			case 55:
			case 56:
			case 57:
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


bool Male::write(std::ofstream & out)
{
	//	load spatial data back into the objects data array
	
	fix_rotation();

	*(float *)(data[52]) = x_loc;
	*(float *)(data[53]) = y_loc;
	*(float *)(data[54]) = z_loc;
	
	*(float *)(data[55]) = x_rot;
	*(float *)(data[56]) = y_rot;
	*(float *)(data[57]) = z_rot;
	*(float *)(data[58]) = w_rot;
	
	*(float *)(data[59]) = x_scal;
	*(float *)(data[60]) = y_scal;
	*(float *)(data[61]) = z_scal;
	
	//  write data array to disk
	
	ofstream::streampos pos = out.tellp();
	
	for (vector<void *>::size_type i=0; i<data.size(); i++)
		data_save(out, i);
	
	logfile << "Male::write -- wrote " << out.tellp() - pos << " bytes." << endl;
	
	return true;

}
