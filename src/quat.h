#pragma once

#include "point.h"

class quat
{
	float xv;
	float yv;
	float zv;
	float wv;
	
	//	stores the transformation matrix
	//	we only need to calculate this when the quaternion is initalized or changed
	float qxx, qyx, qzx;
	float qxy, qyy, qzy;
	float qxz, qyz, qzz;
	
	void calc();
public:
	quat( const float & xx, const float & yy, const float & zz, const float & ww ) :
		xv(xx), yv(yy), zv(zz), wv(ww)
	{ calc(); };
	quat( float, const point & );

	void set( const float & xx, const float & yy, const float & zz, const float & ww )
		{ xv = xx;  yv = yy;  zv = zz;  wv = ww;  calc(); };

	void get( float & xx, float & yy, float & zz, float & ww )
		{ xx = xv;  yy = yv;  zz = zv;  ww = wv; };
		
	point apply(const point &);
	quat & operator *= (const quat & rhs);
};

