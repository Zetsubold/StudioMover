#ifndef THING_H
#define THING_H

#include <vector>
#include <fstream>
#include <cstdint>

//	a base thing class, be it a bed, sofa, or little green man.

enum DataType {
	DT_uint32,
	DT_uint16,
	DT_uint8,
	DT_data,
	DT_float,
	DT_data_noshow
};

class Thing {
	std::vector<DataType> datatype;
	std::vector<int> datacount;
	std::vector<unsigned int> dataoffset;
	unsigned int starting_offset;
public:
	std::vector<void *> data;
	
	Thing(std::ifstream &);
	Thing(const int &);
	~Thing();
	
	void update_offset (std::ifstream &);
	
	unsigned int offset;		// the input file offset at the time of the last call to a store function
	
	uint32_t 	store_uint32	(std::ifstream & file);
	uint16_t 	store_uint16	(std::ifstream & file);
	uint8_t 	store_uint8		(std::ifstream & file);
	char * 		store_data		(std::ifstream & file, int count, bool = false);
	float		store_float		(std::ifstream & file);
	
	void data_output (std::ostream &, const int &) const;
	void data_save (std::ostream &, const int &) const;
};




#endif // THING_H
