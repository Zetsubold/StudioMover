#pragma once

#include "thing.h" 		// Base class: Thing
#include "spatial.h"	// Base class: Spatial
#include "midpoint.h"

//	contain data about an Male object

class Male : private Thing, public Spatial
{
	bool read_string(int &);
	std::ifstream & file;
public:
	Male(std::ifstream &f);
	~Male();
	bool process(Midpoint &);
	bool write(std::ofstream & out);
	friend std::ostream& operator<< (std::ostream &, const Male &);
};

