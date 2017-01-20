#ifndef FEMALE_H
#define FEMALE_H

#include "thing.h" 		// Base class: Thing
#include "spatial.h" 	// Base class: Spatial
#include "midpoint.h"

//	contain data about an Female object

class Female : private Thing, public Spatial
{
	bool read_string(int &);
	std::ifstream & file;
public:
	Female(std::ifstream &f);
	~Female();
	bool process(Midpoint &);
	bool write(std::ofstream & out);
	friend std::ostream& operator<< (std::ostream &, const Female &);

};

#endif // FEMALE_H
