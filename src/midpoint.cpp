#include <limits>

#include "midpoint.h"

using namespace std;

Midpoint::Midpoint()
{
	upper_x = numeric_limits<float>::lowest();
	upper_y = numeric_limits<float>::lowest();
	upper_z = numeric_limits<float>::lowest();
	lower_x = numeric_limits<float>::max();
	lower_y = numeric_limits<float>::max();
	lower_z = numeric_limits<float>::max();
}

Midpoint::~Midpoint()
{
}

void Midpoint::adjust(const float & x, const float & y, const float & z)
{
	if ( upper_x < x ) upper_x = x;
	if ( upper_y < y ) upper_y = y;
	if ( upper_z < z ) upper_z = z;
	if ( lower_x > x ) lower_x = x;
	if ( lower_y > y ) lower_y = y;
	if ( lower_z > z ) lower_z = z;
}

void Midpoint::adjust(Midpoint & mpoint)
{
	if ( upper_x < mpoint.upper_x ) upper_x = mpoint.upper_x;
	if ( upper_y < mpoint.upper_y ) upper_y = mpoint.upper_y;
	if ( upper_z < mpoint.upper_z ) upper_z = mpoint.upper_z;
	if ( lower_x > mpoint.lower_x ) lower_x = mpoint.lower_x;
	if ( lower_y > mpoint.lower_y ) lower_y = mpoint.lower_y;
	if ( lower_z > mpoint.lower_z ) lower_z = mpoint.lower_z;
}

point Midpoint::fetch()
{
	return point( (upper_x+lower_x)/2, (upper_y+lower_y)/2, (upper_z+lower_z)/2 );
}