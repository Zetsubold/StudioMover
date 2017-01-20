#pragma once

#include <vector>
#include <fstream>

#include "midpoint.h"
#include "transform.h"

template <typename T>
class ThingGroup
{
	std::vector<T> group;
public:
	Midpoint midpoint;
	void store( T & );
	void write( std::ofstream &, int);
	void transform( TransformGroup *);
};

template <typename T>
void ThingGroup<T>::store( T & object )
{
	group.push_back(object);
}

template <typename T>
void ThingGroup<T>::write( std::ofstream &out, int count )
{
	for (int i=0; i<count; i++) {
		group[i].write(out);
	}
}

template <typename T>
void ThingGroup<T>::transform( TransformGroup * tgroup ) 
{
	for (typename vector<T>::iterator iter=group.begin(); iter!=group.end(); iter++)
		iter->transform(tgroup);

}