#ifndef ER_HEADER_VEC_H
#define ER_HEADER_VEC_H

#pragma warning( disable : 4201 ) // nonstandard extension used : nameless struct/union

#include <math.h>

struct vec2 {
	float x, y;

	vec2() {
	}
	vec2( float _x, float _y ) : x(_x), y(_y) {
	}

	float LengthSquared( void ) const {
		return x*x+y*y;
	}
};

inline vec2 operator-(vec2 const &a) {
	return vec2(-a.x, -a.y);
}

inline vec2 operator+(vec2 const &a, vec2 const &b) {
	return vec2(a.x+b.x, a.y+b.y);
}

inline vec2 operator-(vec2 const &a, vec2 const &b) {
	return vec2(a.x-b.x, a.y-b.y);
}

struct vec3 {
	float x, y, z;

	vec3() {}

	vec3( float _x, float _y, float _z ) : x(_x), y(_y), z(_z) {}

	vec3( float *v ) {
		Set( v );
	}

	void Set( float *v ) {
		x = v[0];
		y = v[1];
		z = v[2];
	}

	void Zero() {
		x = y = z = 0.f;
	}

	float LengthSquared( void ) const {
		return x*x+y*y+z*z;
	}

	float Length( void ) const {
		return sqrt( LengthSquared() );
	}

	float Normalise( void ) {
		float l = Length();
		float ool = 1.f / l;
		x *= ool;
		y *= ool;
		z *= ool;
		return l;
	}

	vec3 &operator+=( const vec3 &b ) {
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}

	vec3 &operator-=( const vec3 &b ) {
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return *this;
	}

	vec3 &operator*=( float v ) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	vec3 &operator*=( const vec3 &b ) {
		x *= b.x;
		y *= b.y;
		z *= b.z;
		return *this;
	}

	void FixDEN( void ) {
		if ( fabsf(x) < 0.0001f ) {
			x = 0.f;
		}
		if ( fabsf(y) < 0.0001f ) {
			y = 0.f;
		}
		if ( fabsf(z) < 0.0001f ) {
			z = 0.f;
		}
	}

	float Dot( const vec3 &b ) {
		return x * b.x + y * b.y + z * b.z;
	}

	float *ToFloatPtr( void ) {
		return &x;
	}
};

inline vec3 operator+( vec3 const &a, vec3 const &b ) {
	vec3 o;
	o.x = a.x + b.x;
	o.y = a.y + b.y;
	o.z = a.z + b.z;
	return o;
}

inline vec3 operator-( vec3 const &a, vec3 const &b ) {
	vec3 o;
	o.x = a.x - b.x;
	o.y = a.y - b.y;
	o.z = a.z - b.z;
	return o;
}

inline vec3 operator*( vec3 const &a, float const &b ) {
	vec3 o;
	o.x = a.x * b;
	o.y = a.y * b;
	o.z = a.z * b;
	return o;
}

inline vec3 operator*( float const &b, vec3 const &a ) {
	vec3 o;
	o.x = a.x * b;
	o.y = a.y * b;
	o.z = a.z * b;
	return o;
}

inline vec3 operator/( vec3 const &a, float const &b ) {
	vec3 o;
	o.x = a.x / b;
	o.y = a.y / b;
	o.z = a.z / b;
	return o;
}


inline float operator*( vec3 const &b, vec3 const &a ) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vec3 CrossP( vec3 const &a, vec3 const &b ) {
	vec3 o;

	o.x = a.y * b.z - a.z * b.y;
	o.y = a.z * b.x - a.x * b.z;
	o.z = a.x * b.y - a.y * b.x;

	return o;
}

struct vec4 {
	union {
		struct {
			float x, y, z, w;
		};
		float v[4];
	};

	vec4() {}

	vec4( const vec3 &a ) : x(a.x), y(a.y), z(a.z) {}

	void Set( const vec3 &a ) {
		x = a.x;
		y = a.y;
		z = a.z;
		w = 1.f;
	}
};

inline vec4 operator+( vec4 const &a, vec4 const &b ) {
	vec4 o;
	o.x = a.x + b.x;
	o.y = a.y + b.y;
	o.z = a.z + b.z;
	o.w = a.w + b.w;
	return o;
}

inline vec4 operator-( vec4 const &a, vec4 const &b ) {
	vec4 o;
	o.x = a.x - b.x;
	o.y = a.y - b.y;
	o.z = a.z - b.z;
	o.w = a.w - b.w;
	return o;
}

inline vec4 operator*( vec4 const &a, float const &b ) {
	vec4 o;
	o.x = a.x * b;
	o.y = a.y * b;
	o.z = a.z * b;
	o.w = a.w * b;
	return o;
}

inline vec4 operator*( float const &b, vec4 const &a ) {
	vec4 o;
	o.x = a.x * b;
	o.y = a.y * b;
	o.z = a.z * b;
	o.w = a.w * b;
	return o;
}
#endif