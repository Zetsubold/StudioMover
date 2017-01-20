#pragma once

struct point {
	float x;
	float y;
	float z;
	point (const float & fx, const float & fy, const float & fz) : x(fx), y(fy), z(fz) {};
	point & operator += (const point & rhs);
	point & operator *= (const point & rhs);
};


