#include <iomanip>

#include "thing.h"
#include "logging.h"

using namespace std;

Thing::Thing(std::ifstream & f) : starting_offset(f.tellg())
{
}

Thing::Thing(const int & o) : starting_offset(o)
{
}

void Thing::update_offset (std::ifstream & f)
{
	starting_offset = f.tellg();
}

Thing::~Thing()
{
}

uint32_t Thing::store_uint32(std::ifstream & file)
{
	uint32_t * d = new uint32_t;
	
	datatype.push_back(DT_uint32);
	datacount.push_back(sizeof(uint32_t));
	
	offset = file.tellg();
	dataoffset.push_back( offset - starting_offset );
	
	file.read(reinterpret_cast<char *>(d), sizeof(uint32_t));
	data.push_back(d);
	
//	logfile << "uint32 - " << hex << *d << dec << endl;
	
	return *d;
}

uint16_t Thing::store_uint16(std::ifstream & file)
{
	uint16_t * d = new uint16_t;
	
	datatype.push_back(DT_uint16);
	datacount.push_back(sizeof(uint16_t));
	
	offset = file.tellg();
	dataoffset.push_back( offset - starting_offset );
	
	file.read(reinterpret_cast<char *>(d), sizeof(uint16_t));
	data.push_back(d);
	
//	logfile << "uint16 - " << hex << *d << dec << endl;
	
	return *d;
}

uint8_t Thing::store_uint8(std::ifstream & file)
{
	uint8_t * d = new uint8_t;
	
	datatype.push_back(DT_uint8);
	datacount.push_back(sizeof(uint8_t));
	
	offset = file.tellg();
	dataoffset.push_back( offset - starting_offset );
	
	file.read(reinterpret_cast<char *>(d), sizeof(uint8_t));
	data.push_back(d);
	
//	logfile << "uint8 - " << hex << (unsigned int)(*d) << dec << endl;
	
	return *d;
}

char * Thing::store_data(std::ifstream & file, int count, bool suppress)
{
	datatype.push_back( (suppress) ? DT_data_noshow : DT_data );
	datacount.push_back(count);
		
	offset = file.tellg();
	dataoffset.push_back( offset - starting_offset );
	
	//  allow for a zero-length data segment
	
	char * d = (count==0) ? nullptr : new char[count];
	
	if ( count > 0 )
		file.read(d, count);
		
	data.push_back(d);
	
//	string s = (suppress || (count==0) ) ? ">>not shown<<" : string(d, count);
//	logfile << "store - read " << count << " bytes -- " << s << endl;

	return d;
}

float Thing::store_float(std::ifstream & file)
{
	float * d = new float;
	
	datatype.push_back(DT_float);
	datacount.push_back(4);


	offset = file.tellg();
	dataoffset.push_back( offset - starting_offset );
	
	file.read(reinterpret_cast<char *>(d), 4);
	data.push_back(d);
	
//	logfile << "float - " << showpoint << (float)(*d) << noshowpoint << endl;
	
	return *d;
}

void Thing::data_output (std::ostream & out, const int & index) const
{
	out << setw(4) << index << "|" << setw(10) << dataoffset[index] << " | ";
	
	switch (datatype[index]) {
		case DT_uint32:
			out << *(unsigned int *)(data[index]) << "\n";
			break;
		case DT_uint16:
			out << *(unsigned short *)(data[index]) << "\n";
			break;
		case DT_uint8:
			out << (unsigned int)*(unsigned char *)(data[index]) << "\n";
			break;
		case DT_data:
		{
			string s = (datacount[index]==0) ? ">>EMPTY<<" : string((char *)(data[index]), datacount[index]);
			out << s << "\n";
			break;
		}
		case DT_data_noshow:
		{
			out << ">> not shown - " << datacount[index] << " bytes <<" << "\n";
			break;
		}
		case DT_float:
			out << showpoint << *(float *)(data[index]) << "\n";
			break;
		
	}
}

void Thing::data_save (std::ostream & out, const int & index) const
{
	out.write((char *)(data[index]), datacount[index]);
}