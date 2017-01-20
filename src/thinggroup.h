#pragma once

#include <vector>
#include <iostream>

#include "midpoint.h"

template <typename T>
class ThingGroup
{
	std::vector<T> group;
public:
	void store( T & );
	void write( std::ostream &, int);
};

template <typename T>
void ThingGroup<T>::store( T & object)
{
	group.push_back(object);
}

template <typename T>
void ThingGroup<T>::write( std::ostream &out, int count)
{
	for (int i=0; i<count; i++) {
		group[i].write(out);
	}
}