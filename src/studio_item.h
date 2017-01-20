#ifndef ITEM_H
#define ITEM_H

#include <fstream>
#include <vector>

#include "thing.h"		// Base class: Thing
#include "spatial.h"	// Base Class: Spatial	
#include "midpoint.h"

//	contain data about an Item object

class Item : private Thing, public Spatial
{
	bool read_string(int &);
	std::ifstream & file;
public:
	Item(std::ifstream &f);
	~Item();
	bool process(Midpoint &);
	bool write(std::ofstream & out);
	friend std::ostream& operator<< (std::ostream &, const Item &);

};

#endif // ITEM_H
