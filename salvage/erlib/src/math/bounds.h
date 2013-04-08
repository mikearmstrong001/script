#ifndef ER_HEADER_BOUNDS_H
#define ER_HEADER_BOUNDS_H

#include "vec.h"
#include "maths.h"

class BoundsVec3 {
	vec3 b[2];
public:

	BoundsVec3() {};
	BoundsVec3( bool clear ) {
		if ( clear ) {
			Clear();
		}
	}

	void Clear( void ) {
		b[0].x = MAX_FLT;
		b[0].y = MAX_FLT;
		b[0].z = MAX_FLT;
		b[1].x = -MAX_FLT;
		b[1].y = -MAX_FLT;
		b[1].z = -MAX_FLT;
	}

	void Add( const vec3 &v ) {
		b[0].x = Maths::Min( b[0].x, v.x );
		b[0].y = Maths::Min( b[0].y, v.y );
		b[0].z = Maths::Min( b[0].z, v.z );
		b[1].x = Maths::Max( b[1].x, v.x );
		b[1].y = Maths::Max( b[1].y, v.y );
		b[1].z = Maths::Max( b[1].z, v.z );
	}

	void Zero( void ) {
		b[0].Zero();
		b[1].Zero();
	}

	void Expand( const vec3 &v ) {
		b[0] -= v;
		b[1] += v;
	}

	vec3 &operator[](int idx) {
		return b[idx];
	}

	const vec3 &operator[](int idx) const {
		return b[idx];
	}

	bool Intersects( const BoundsVec3 &other ) const {
		return !( b[0].x > other.b[1].x || b[1].x < other.b[0].x ||
			b[0].y > other.b[1].y || b[1].y < other.b[0].y ||
			b[0].z > other.b[1].z || b[1].z < other.b[0].z );
	}

	bool Intersects( const float *mn, const float *mx ) const {
		return !( b[0].x > mx[0] || b[1].x < mn[0] ||
			b[0].y > mx[1] || b[1].y < mn[1] ||
			b[0].z > mx[2] || b[1].z < mn[2] );
	}
};


#endif