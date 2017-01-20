#ifndef SPATIAL_H
#define SPATIAL_H

#include "quat.h"

class TransformGroup;

class Spatial
{
	quat Q;
protected:
	float x_loc, y_loc, z_loc;
	float x_rot, y_rot, z_rot, w_rot;
	float x_scal, y_scal, z_scal;

	Spatial();
	~Spatial();
	void set_location (const float &, const float &, const float &);
	void set_rotation (const float &, const float &, const float &, const float &);
	void set_scaling (const float &, const float &, const float &);
	
	void fix_rotation();
	
	void translate(const float &, const float &, const float &);
	void scale(const float &, const float &, const float &);
	void rotate(const float &, const float &, const float &);
public:
	void transform (TransformGroup *);
};

extern float internal2degrees (float);

extern void log_spatial(const char *, const float &, const float &, const float &);
extern void log_spatial(const char *, const float &, const float &, const float &, const float &);

#endif // SPATIAL_H
