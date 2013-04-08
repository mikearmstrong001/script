#ifndef ER_HEADER_POSITION_H
#define ER_HEADER_POSITION_H

#include "../erbase.h"
#include "vec.h"

struct position3 {
	int64_t x, y, z;

	void Zero() {
		x = y = z = 0;
	}

	vec3 ToVec3( void ) {
		vec3 v;
		v.x = (x) * (1.f/65535.f);
		v.y = (y) * (1.f/65535.f);
		v.z = (z) * (1.f/65535.f);
		return v;
	}

	void Set( float _x, float _y, float _z ) {
		x = (int64_t)((_x) * (65535.f));
		y = (int64_t)((_y) * (65535.f));
		z = (int64_t)((_z) * (65535.f));
	}

	void Set( const vec3 &p ) {
		Set( p.x, p.y, p.z );
	}

	position3 &operator+=( const vec3 &b ) {
		x += (int64_t)(b.x * (65535.f));
		y += (int64_t)(b.y * (65535.f));
		z += (int64_t)(b.z * (65535.f));
		return *this;
	}

	position3 &operator-=( const vec3 &b ) {
		x -= (int64_t)(b.x * (65535.f));
		y -= (int64_t)(b.y * (65535.f));
		z -= (int64_t)(b.z * (65535.f));
		return *this;
	}
};

inline vec3 operator-(position3 const &a, position3 const &b) {
	vec3 v;
	v.x = (a.x - b.x) * (1.f/65535.f);
	v.y = (a.y - b.y) * (1.f/65535.f);
	v.z = (a.z - b.z) * (1.f/65535.f);
	return v;
}

inline position3 operator+(position3 const &a, vec3 const &b) {
	position3 v;
	v.x = (a.x) + (int64_t)(b.x*65535.f);
	v.y = (a.y) + (int64_t)(b.y*65535.f);
	v.z = (a.z) + (int64_t)(b.z*65535.f);
	return v;
}

inline position3 operator-(position3 const &a, vec3 const &b) {
	position3 v;
	v.x = (a.x) - (int64_t)(b.x*65535.f);
	v.y = (a.y) - (int64_t)(b.y*65535.f);
	v.z = (a.z) - (int64_t)(b.z*65535.f);
	return v;
}

#endif