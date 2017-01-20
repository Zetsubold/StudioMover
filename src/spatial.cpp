#include "spatial.h"
#include "transform.h"
#include "logging.h"

#include <cmath>

using namespace std;

Spatial::Spatial() : Q(0,0,0,0)
{
}

Spatial::~Spatial()
{
}

const double PI_2 = std::atan(1.0)*2;

float internal2degrees (float f)
{
	return asin(f) * 180.0 / PI_2;
}

//	prepare rotation values for outputting back into studio file
void Spatial::fix_rotation() {
		Q.get(x_rot, y_rot, z_rot, w_rot);
}

void Spatial::set_location (const float &x, const float &y, const float &z)
{
	x_loc = x;
	y_loc = y;
	z_loc = z;
}

void Spatial::set_rotation (const float &x, const float &y, const float &z, const float &w)
{
	x_rot = x;
	y_rot = y;
	z_rot = z;
	w_rot = w;

	Q.set(x_rot, y_rot, z_rot, w_rot);
}

void Spatial::set_scaling (const float &x, const float &y, const float &z)
{
	x_scal = x;
	y_scal = y;
	z_scal = z;
}

void Spatial::translate (const float &x, const float &y, const float &z)
{
	x_loc += x;
	y_loc += y;
	z_loc += z;
}

void Spatial::rotate (const float &x, const float &y, const float &z)
{
	//	x,y,z are a single axis rotation angle
	quat Qrot( x + y + z,
			   point( ( fabsf(x) > 0.0f ? 1.0f : 0.0f ), 
					  ( fabsf(y) > 0.0f ? 1.0f : 0.0f ),
					  ( fabsf(z) > 0.0f ? 1.0f : 0.0f )
					)
			 );
			 
	// apply rotation
	Q *= Qrot;
}


void Spatial::scale (const float &x, const float &y, const float &z)
{
	x_scal *= x;
	y_scal *= y;
	z_scal *= z;
}

void Spatial::transform ( TransformGroup * tgroup )
{
	for (vector<Transform>::iterator iter=tgroup->transforms.begin(); iter!=tgroup->transforms.end(); iter++) {
		switch( iter->type ) {
		case TR_translate:
			translate( iter->P.x,  iter->P.y, iter->P.z );
			break;
		case TR_rotate_inplace:
			rotate( iter->P.x,  iter->P.y, iter->P.z );
			break;
		case TR_scale_inplace:
			scale( iter->P.x,  iter->P.y, iter->P.z );
			break;
		default :
			break;
		}
	}

}

void log_spatial(const char * s, const float & x, const float & y, const float & z)
{
		logfile << s << " -- " << x << ", " << y << ", " << z << endl;
}

void log_spatial(const char * s, const float & x, const float & y, const float & z, const float & w)
{
		logfile << s << " -- " << x << ", " << y << ", " << z << " -- w = " << w << endl;
}
