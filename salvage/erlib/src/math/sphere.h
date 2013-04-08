#ifndef ER_HEADER_SPHERE_H
#define ER_HEADER_SPHERE_H

#include "vec.h"

struct Sphere {
	vec3 centre;
	float radius;
};

Sphere SPHERE_MinSphere( const vec3 &p0, const vec3 &p1, const vec3 &p2, const vec3 &p3 );


#endif

