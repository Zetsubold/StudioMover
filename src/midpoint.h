#pragma once

#include "point.h"

class Midpoint
{
	float upper_x;
	float lower_x;
	float upper_y;
	float lower_y;
	float upper_z;
	float lower_z;
public:
	Midpoint();
	~Midpoint();
	void adjust(const float &, const float &, const float &);
	void adjust(Midpoint &);
	point fetch();
};
