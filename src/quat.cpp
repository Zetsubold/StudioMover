#include <cmath>

#include "quat.h"
#include "logging.h"

const double PI_2 = std::atan(1.0)*2;

quat::quat( float angle, const point & axis ) {
	//	convert angle to radians, also divide it by 2
	angle *= PI_2 / 180;
	
	float half_sin = std::sin(angle);
	
    wv = std::cos(angle);
	xv = half_sin * axis.x;
	yv = half_sin * axis.y;
	zv = half_sin * axis.z;
	
	calc();
}

void quat::calc() {
	qxx = 1  - (2*yv*yv) - (2*zv*zv);
	qyx = (2*xv*yv) - (2*wv*zv);
	qzx = (2*xv*zv) + (2*wv*yv);
	
	qxy = (2*xv*yv) + (2*wv*zv);
	qyy = 1  - (2*xv*xv) - (2*zv*zv);
	qzy = (2*yv*zv) + (2*wv*xv);

	qxz = (2*xv*zv) - (2*wv*yv);
	qyz = (2*yv*zv) - (2*wv*xv);
	qzz = 1  - (2*xv*xv) - (2*yv*yv);
	
	logfile << "Quat" << " -- " << xv << ", " << yv << ", " << zv << " -- w = " << wv << std::endl;
}

point quat::apply(const point & p) {

	return point(
		(p.x * qxx + p.y * qxy + p.z * qxz) ,
		(p.x * qyx + p.y * qyy + p.z * qyz) ,
		(p.x * qzx + p.y * qzy + p.z * qzz)
	);
	
}

quat & quat::operator *= (const quat & rhs)
{
	float Qx = ( wv * rhs.xv ) + ( xv * rhs.wv ) + ( yv * rhs.zv ) - ( zv * rhs.yv );
	float Qy = ( wv * rhs.yv ) - ( xv * rhs.zv ) + ( yv * rhs.wv ) + ( zv * rhs.xv );
	float Qz = ( wv * rhs.zv ) - ( xv * rhs.yv ) - ( yv * rhs.xv ) - ( zv * rhs.wv );
	float Qw = ( wv * rhs.wv ) - ( xv * rhs.xv ) - ( yv * rhs.yv ) - ( zv * rhs.zv );
	
	xv = Qx;
	yv = Qy;
	zv = Qz;
	wv = Qw;
	
	logfile << "Quat *=" << std::endl;
	
	calc();
	return *this;
}
