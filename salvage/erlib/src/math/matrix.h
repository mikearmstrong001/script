#ifndef ER_HEADER_MATRIX_H
#define ER_HEADER_MATRIX_H

#include "vec.h"
#include "maths.h"
#include "../misc.h"

struct mat33 {
	float v[9];

	mat33 Transpose() {
		mat33 o;
		o.v[0] = v[0];
		o.v[1] = v[3];
		o.v[2] = v[6];

		o.v[3] = v[1];
		o.v[4] = v[4];
		o.v[5] = v[7];

		o.v[6] = v[2];
		o.v[7] = v[5];
		o.v[8] = v[8];

		return o;
	}

	void TransposeSelf() {
		Swap( v[1], v[3] );
		Swap( v[2], v[6] );
		Swap( v[5], v[7] );
	}

	void I() {
		v[0] = 1.f;
		v[1] = 0.f;
		v[2] = 0.f;

		v[3] = 0.f;
		v[4] = 1.f;
		v[5] = 0.f;

		v[6] = 0.f;
		v[7] = 0.f;
		v[8] = 1.f;
	}

	void SetX( float a ) {
		float sx, cx;
		Maths::SinCos( sx, cx, a );
		v[0] = 1.f;
		v[1] = 0.f;
		v[2] = 0.f;

		v[3] = 0.f;
		v[4] = cx;
		v[5] = sx;

		v[6] = 0.f;
		v[7] = -sx;
		v[8] = cx;
	}

	void SetY( float a ) {
		float sy, cy;
		Maths::SinCos( sy, cy, a );
		v[0] = cy;
		v[1] = 0.f;
		v[2] = -sy;

		v[3] = 0.f;
		v[4] = 1.f;
		v[5] = 0.f;

		v[6] = sy;
		v[7] = 0.f;
		v[8] = cy;
	}

	void SetZ( float a ) {
		float sz, cz;
		Maths::SinCos( sz, cz, a );
		v[0] = cz;
		v[1] = sz;
		v[2] = 0.f;
		
		v[3] = -sz;
		v[4] = cz;
		v[5] = 0.f;

		v[6] = 0.f;
		v[7] = 0.f;
		v[8] = 1.f;
	}

	const vec3 &operator[](int idx) {
		return *(vec3*)&v[idx*3];
	}

	vec3 Forward( void ) {
		vec3 o;
		o.x = v[6];
		o.y = v[7];
		o.z = v[8];
		return o;
	}

	vec3 Up( void ) {
		vec3 o;
		o.x = v[3];
		o.y = v[4];
		o.z = v[5];
		return o;
	}

	vec3 Right( void ) {
		vec3 o;
		o.x = v[0];
		o.y = v[1];
		o.z = v[2];
		return o;
	}

	void LookAt( const vec3 &zdir, const vec3 &up ) {
		vec3 r = CrossP( up, zdir );
		r.Normalise();

		vec3 u = CrossP( zdir, r );
		u.Normalise();

		v[0] = r.x;
		v[1] = r.y;
		v[2] = r.z;

		v[3] = u.x;
		v[4] = u.y;
		v[5] = u.z;

		v[6] = zdir.x;
		v[7] = zdir.y;
		v[8] = zdir.z;
	}

	void LookAtPos( const vec3 &from, const vec3 &to, const vec3 &up ) {
		vec3 d = to - from;
		d.Normalise();
		LookAt( d, up );
	}
};

inline mat33 operator*( mat33 const &a, mat33 const &b ) {
	mat33 o;
	o.v[0] = a.v[0] * b.v[0] + a.v[1] * b.v[3] + a.v[2] * b.v[6];
	o.v[1] = a.v[0] * b.v[1] + a.v[1] * b.v[4] + a.v[2] * b.v[7];
	o.v[2] = a.v[0] * b.v[2] + a.v[1] * b.v[5] + a.v[2] * b.v[8];

	o.v[3] = a.v[3] * b.v[0] + a.v[4] * b.v[3] + a.v[5] * b.v[6];
	o.v[4] = a.v[3] * b.v[1] + a.v[4] * b.v[4] + a.v[5] * b.v[7];
	o.v[5] = a.v[3] * b.v[2] + a.v[4] * b.v[5] + a.v[5] * b.v[8];

	o.v[6] = a.v[6] * b.v[0] + a.v[7] * b.v[3] + a.v[8] * b.v[6];
	o.v[7] = a.v[6] * b.v[1] + a.v[7] * b.v[4] + a.v[8] * b.v[7];
	o.v[8] = a.v[6] * b.v[2] + a.v[7] * b.v[5] + a.v[8] * b.v[8];

	return o;
}

inline vec3 operator*( vec3 const &a, mat33 const &b ) {
	vec3 o;

	o.x = a.x * b.v[0] + a.y * b.v[3] + a.z * b.v[6];
	o.y = a.x * b.v[1] + a.y * b.v[4] + a.z * b.v[7];
	o.z = a.x * b.v[2] + a.y * b.v[5] + a.z * b.v[8];

	return o;
}

struct mat44 {
	union {
		float v[16];
		float f[4][4];
	};

	void Set( mat33 const &a, vec3 const &t ) {
		v[0] = a.v[0];
		v[1] = a.v[1];
		v[2] = a.v[2];
		v[3] = 0.f;

		v[4] = a.v[3];
		v[5] = a.v[4];
		v[6] = a.v[5];
		v[7] = 0.f;
	
		v[8] = a.v[6];
		v[9] = a.v[7];
		v[10] = a.v[8];
		v[11] = 0.f;

		v[12] = t.x;
		v[13] = t.y;
		v[14] = t.z;
		v[15] = 1.f;
	}

	void Perspective( float fov, float aspect, float zn, float zf ) {
		float f = 1.0f / tanf( fov / 2.f );
		v[0] = f / aspect;
		v[1] = v[2] = v[3] = 0.f;
		v[4] = 0.f;
		v[5] = f;
		v[6] = v[7] = 0.f;
		v[8] = v[9] = 0.f;
		v[10] = (zf) / (zf-zn);
		v[11] = 1.f;
		v[12] = v[13] = 0.f;
		v[14] = -zn*zf/(zf-zn);
		v[15] = 0.f;
	}

	void PerspectiveInfinite( float fov, float aspect, float zn ) {
		float f = 1.0f / tanf( fov / 2.f );
		v[0] = f / aspect;
		v[1] = v[2] = v[3] = 0.f;
		v[4] = 0.f;
		v[5] = f;
		v[6] = v[7] = 0.f;
		v[8] = v[9] = 0.f;
		v[10] = 0.999f;
		v[11] = 1.f;
		v[12] = v[13] = 0.f;
		v[14] = -zn;
		v[15] = 0.f;
	}

	void ToOpenGL( void ) {
		double n = -v[14] / v[10];
		double d = (1.-(double)v[10]);

		if ( fabs( d ) > 0.0001 ) {
			double f = -((double)v[10]*n) / d;
			v[10] = (float)((f+n)/(f-n));  // f/(f-n) + n/(f-n)
		}
		//v[10] -= 
		//v[11] = -1.f;
		//v[14] = -2.f*f*n/(f-n);
		v[14] *= 2.f;
	}

	void Orthographic( float mnx, float mny, float mxx, float mxy, float zn, float zf ) {
		float w = mxx - mnx;
		float h = mxy - mny;

		v[0] = 2.f/w;
		v[1] = 0.f;
		v[2] = 0.f;
		v[3] = 0.f;

		v[4] = 0.f;
		v[5] = 2.f/h;
		v[6] = 0.f;
		v[7] = 0.f;

		v[8] = 0.f;
		v[9] = 0.f;
		v[10] = 1.f / (zf-zn);
		v[11] = 0.f;

		v[12] = (mnx+mxx) / (mnx-mxx);
		v[13] = (mny+mxy) / (mxy-mny);
		v[14] = zn/(zn-zf);
		v[15] = 1.f;
	}

	mat44 Invert( void ) const {
		mat44 dst;
		float tmp[12]; /* temp array for pairs */
		float src[16]; /* array of transpose source matrix */
		float det; /* determinant */
		/* transpose matrix */
		for (int i = 0; i < 4; i++) {
			src[i] = v[i*4];
			src[i + 4] = v[i*4 + 1];
			src[i + 8] = v[i*4 + 2];
			src[i + 12] = v[i*4 + 3];
		}
		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];
		/* calculate first 8 elements (cofactors) */
		dst.v[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
		dst.v[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
		dst.v[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
		dst.v[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
		dst.v[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
		dst.v[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
		dst.v[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
		dst.v[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
		dst.v[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
		dst.v[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
		dst.v[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
		dst.v[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
		dst.v[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
		dst.v[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
		dst.v[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
		dst.v[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
		/* calculate pairs for second 8 elements (cofactors) */
		tmp[0] = src[2]*src[7];
		tmp[1] = src[3]*src[6];
		tmp[2] = src[1]*src[7];
		tmp[3] = src[3]*src[5];
		tmp[4] = src[1]*src[6];
		tmp[5] = src[2]*src[5];

		tmp[6] = src[0]*src[7];
		tmp[7] = src[3]*src[4];
		tmp[8] = src[0]*src[6];
		tmp[9] = src[2]*src[4];
		tmp[10] = src[0]*src[5];
		tmp[11] = src[1]*src[4];
		/* calculate second 8 elements (cofactors) */
		dst.v[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
		dst.v[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
		dst.v[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
		dst.v[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
		dst.v[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
		dst.v[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
		dst.v[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
		dst.v[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
		dst.v[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
		dst.v[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
		dst.v[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
		dst.v[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
		dst.v[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
		dst.v[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
		dst.v[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
		dst.v[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
		/* calculate determinant */
		det=src[0]*dst.v[0]+src[1]*dst.v[1]+src[2]*dst.v[2]+src[3]*dst.v[3];
		/* calculate matrix inverse */
		det = 1.f/det;
		for (int j = 0; j < 16; j++) {
			dst.v[j] *= det;
		}

		return dst;
	}


	struct quat ToQuat() const;

	vec3 Transform() const {
		return vec3( f[3][0], f[3][1], f[3][2] );
	}
};


inline mat44 operator*( mat44 const &a, mat44 const &b ) {
	mat44 o;
	o.v[0] = a.v[0] * b.v[0] + a.v[1] * b.v[4] + a.v[2] * b.v[8] + a.v[3] * b.v[12];
	o.v[1] = a.v[0] * b.v[1] + a.v[1] * b.v[5] + a.v[2] * b.v[9] + a.v[3] * b.v[13];
	o.v[2] = a.v[0] * b.v[2] + a.v[1] * b.v[6] + a.v[2] * b.v[10] + a.v[3] * b.v[14];
	o.v[3] = a.v[0] * b.v[3] + a.v[1] * b.v[7] + a.v[2] * b.v[11] + a.v[3] * b.v[15];

	o.v[4] = a.v[4] * b.v[0] + a.v[5] * b.v[4] + a.v[6] * b.v[8] + a.v[7] * b.v[12];
	o.v[5] = a.v[4] * b.v[1] + a.v[5] * b.v[5] + a.v[6] * b.v[9] + a.v[7] * b.v[13];
	o.v[6] = a.v[4] * b.v[2] + a.v[5] * b.v[6] + a.v[6] * b.v[10] + a.v[7] * b.v[14];
	o.v[7] = a.v[4] * b.v[3] + a.v[5] * b.v[7] + a.v[6] * b.v[11] + a.v[7] * b.v[15];

	o.v[8] = a.v[8] * b.v[0] + a.v[9] * b.v[4] + a.v[10] * b.v[8] + a.v[11] * b.v[12];
	o.v[9] = a.v[8] * b.v[1] + a.v[9] * b.v[5] + a.v[10] * b.v[9] + a.v[11] * b.v[13];
	o.v[10] = a.v[8] * b.v[2] + a.v[9] * b.v[6] + a.v[10] * b.v[10] + a.v[11] * b.v[14];
	o.v[11] = a.v[8] * b.v[3] + a.v[9] * b.v[7] + a.v[10] * b.v[11] + a.v[11] * b.v[15];

	o.v[12] = a.v[12] * b.v[0] + a.v[13] * b.v[4] + a.v[14] * b.v[8] + a.v[15] * b.v[12];
	o.v[13] = a.v[12] * b.v[1] + a.v[13] * b.v[5] + a.v[14] * b.v[9] + a.v[15] * b.v[13];
	o.v[14] = a.v[12] * b.v[2] + a.v[13] * b.v[6] + a.v[14] * b.v[10] + a.v[15] * b.v[14];
	o.v[15] = a.v[12] * b.v[3] + a.v[13] * b.v[7] + a.v[14] * b.v[11] + a.v[15] * b.v[15];

	return o;
}

inline vec3 operator*( vec3 const &a, mat44 const &b ) {
	vec3 o;

	o.x = a.x * b.v[0] + a.y * b.v[4] + a.z * b.v[8] + b.v[12];
	o.y = a.x * b.v[1] + a.y * b.v[5] + a.z * b.v[9] + b.v[13];
	o.z = a.x * b.v[2] + a.y * b.v[6] + a.z * b.v[10] + b.v[14];

	return o;
}

#endif