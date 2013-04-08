#ifndef ER_HEADER_EULERANGLE_H
#define ER_HEADER_EULERANGLE_H

#include "matrix.h"
#include "maths.h"

struct EulerAngle {
	float x, y, z;

	void Zero( void ) {
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	mat33 ToMatrixXY() {
		mat33 my;
		my.SetY( y );
		mat33 mx;
		mx.SetX( x );
		return mx * my;
	}

	vec3 Forward( void ) {
		float cy, sy;
		Maths::SinCos(sy, cy, y);
		return vec3( sy, 0.f, cy );
	}

	vec3 Right( void ) {
		float cy, sy;
		Maths::SinCos(sy, cy, y);
		return vec3( cy, 0.f, -sy );
	}
};

#endif