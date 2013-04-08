#ifndef ER_HEADER_FRUSTUM_H
#define ER_HEADER_FRUSTUM_H

#include "plane.h"

class Frustum {

	Plane planes[6];
	vec3 points[8];

public:

	void FromMatrixD3D( const float *f, bool normalise );

	bool AABBIntersects( const float *mn, const float *mx );

	void GenPoints( vec3 p[8], float minz, float maxz );
};

#endif