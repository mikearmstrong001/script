#ifndef ER_HEADER_PLANE_H
#define ER_HEADER_PLANE_H

#include "vec.h"
#include "../erbase.h"

struct Plane {
	vec3 n;
	float d;

	void Normalise() {
		float l = 1.f / n.Length();

		n.x *= l;
		n.y *= l;
		n.z *= l;
		d *= l;
	}

	float *ToFloatPtr( void ) {
		return &n.x;
	}

	void NewellPlane( void *p, int pstride, int c ) {
		vec3 centroid;
		centroid.Zero();
		for (int i=c-1, j=0; j<c; i=j, j++) {
			vec3 *vi = (vec3*)((uint8_t*)p+pstride*i);
			vec3 *vj = (vec3*)((uint8_t*)p+pstride*j);
			n.x += (vi->y - vj->y) * (vi->z + vj->z);
			n.y += (vi->z - vj->z) * (vi->x + vj->x);
			n.z += (vi->x - vj->x) * (vi->y + vj->y);
			centroid += *vj;
		}
		n.Normalise();
		d = -(centroid * n) / (float)c;
	}
};

vec3 PLANE_Intersect( const Plane &p1, const Plane &p2, const Plane &p3 );

#endif
