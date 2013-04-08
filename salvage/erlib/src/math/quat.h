#ifndef ER_HEADER_QUAT_H
#define ER_HEADER_QUAT_H

#include "matrix.h"

struct quat {
	union {
		struct {
			float x, y, z, w;
		};
		float f[4];
	};

	void CalcW( void ) {
		float v = 1.f - ((x*x) + (y*y) + (z*z));
		w = v > 0.f ? sqrtf( v ) : 0.f;
	}

	mat33 ToMat33( void ) const {
		mat33 m;

		float x2 = x + x;
		float y2 = y + y;
		float z2 = z + z;
		{
			float xx2 = x * x2;
			float yy2 = y * y2;
			float zz2 = z * z2;
			m.v[0*3+0] = 1.0f - yy2 - zz2;
			m.v[1*3+1] = 1.0f - xx2 - zz2;
			m.v[2*3+2] = 1.0f - xx2 - yy2;
		}
		{
			float yz2 = y * z2;
			float wx2 = w * x2;
			m.v[2*3+1] = yz2 + wx2;
			m.v[1*3+2] = yz2 - wx2;
		}
		{
			float xy2 = x * y2;
			float wz2 = w * z2;
			m.v[1*3+0] = xy2 + wz2;
			m.v[0*3+1] = xy2 - wz2;
		}
		{
			float xz2 = x * z2;
			float wy2 = w * y2;
			m.v[0*3+2] = xz2 + wy2;
			m.v[2*3+0] = xz2 - wy2;
		}
		return m;
	}

	void Normalise( void ) {
		float m = 1.f / sqrtf(x*x+y*y+z*z+w*w);
		x *= m;
		y *= m;
		z *= m;
		w *= m;
	}

	void NLerpQuick( const quat &a, const quat &b, float dt ) {
		float omdt = 1.f - dt;
		x = omdt * a.x + dt * b.x;
		y = omdt * a.y + dt * b.y;
		z = omdt * a.z + dt * b.z;
		w = omdt * a.w + dt * b.w;
		Normalise();
	}

	void NLerp( const quat &a, const quat &b, float dt ) {
		float omdt = 1.f - dt;
		float dp = a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;
		if ( dp < 0.f ) {
			dt = -dt;
		}
		x = omdt * a.x + dt * b.x;
		y = omdt * a.y + dt * b.y;
		z = omdt * a.z + dt * b.z;
		w = omdt * a.w + dt * b.w;
		Normalise();
	}
};

#endif