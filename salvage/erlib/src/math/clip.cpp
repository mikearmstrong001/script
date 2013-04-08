#include "../erbase.h"
#include "clip.h"
#include "vec.h"

static int side( float v ) {
	if ( v < 0.f ) {
		return 1;
	} else
	if ( v > 0.f ) {
		return 2;
	} else {
		return 3;
	}
}

int clipNearPlane0( void* out, int outStride, void* in, int inStride, int numPoints ) {
	if ( numPoints < 3 ) {
		return 0;
	}

	int lp = numPoints - 1;
	int numOut = 0;
	for (int i=0; i<numPoints; i++) {
		int np = i;
		vec4 *lpnt = (vec4*)((uint8_t*)in + lp * inStride);
		vec4 *npnt = (vec4*)((uint8_t*)in + np * inStride);

		int ls = side( lpnt->z );
		int ns = side( npnt->z );

		if ( (ls & ns) == 0 ) {
			vec4 *opnt = (vec4*)((uint8_t*)out + numOut * outStride);
			numOut++;
			float t = -lpnt->z / ( npnt->z - lpnt->z );
			*opnt = *lpnt + t * ( *npnt - *lpnt );
			assert( fabsf( opnt->z ) < 0.001f );
		}

		if ( ns & 2 ) {
			vec4 *opnt = (vec4*)((uint8_t*)out + numOut * outStride);
			numOut++;
			*opnt = *npnt;
		}
		lp = np;
	}

	return numOut;
}

int clipPlaneW( void* out, int outStride, void* in, int inStride, int numPoints, int idx ) {
	if ( numPoints < 3 ) {
		return 0;
	}

	int lp = numPoints - 1;
	int numOut = 0;
	for (int i=0; i<numPoints; i++) {
		int np = i;
		vec4 *lpnt = (vec4*)((uint8_t*)in + lp * inStride);
		vec4 *npnt = (vec4*)((uint8_t*)in + np * inStride);

		int ls = side( lpnt->w - lpnt->v[idx] );
		int ns = side( npnt->w - npnt->v[idx] );

		if ( (ls & ns) == 0 ) {
			vec4 *opnt = (vec4*)((uint8_t*)out + numOut * outStride);
			numOut++;
			float t = (lpnt->v[idx] - lpnt->w) / (( npnt->w - lpnt->w ) - ( npnt->v[idx] - lpnt->v[idx] ));
			*opnt = *lpnt + t * ( *npnt - *lpnt );
			assert( fabsf( opnt->v[idx] - opnt->w ) < 0.001f );
		}

		if ( ns & 2 ) {
			vec4 *opnt = (vec4*)((uint8_t*)out + numOut * outStride);
			numOut++;
			*opnt = *npnt;
		}
		lp = np;
	}

	return numOut;
}

int clipPlaneNegW( void* out, int outStride, void* in, int inStride, int numPoints, int idx ) {
	if ( numPoints < 3 ) {
		return 0;
	}

	int lp = numPoints - 1;
	int numOut = 0;
	for (int i=0; i<numPoints; i++) {
		int np = i;
		vec4 *lpnt = (vec4*)((uint8_t*)in + lp * inStride);
		vec4 *npnt = (vec4*)((uint8_t*)in + np * inStride);

		int ls = side( lpnt->v[idx] + lpnt->w );
		int ns = side( npnt->v[idx] + npnt->w );

		if ( (ls & ns) == 0 ) {
			vec4 *opnt = (vec4*)((uint8_t*)out + numOut * outStride);
			numOut++;
			float t = (lpnt->v[idx] + lpnt->w) / (-( npnt->w - lpnt->w ) - ( npnt->v[idx] - lpnt->v[idx] ));
			*opnt = *lpnt + t * ( *npnt - *lpnt );
			assert( fabsf( opnt->v[idx] - (-opnt->w) ) < 0.001f );
		}

		if ( ns & 2 ) {
			vec4 *opnt = (vec4*)((uint8_t*)out + numOut * outStride);
			numOut++;
			*opnt = *npnt;
		}
		lp = np;
	}

	return numOut;
}


int clipPlanesD3D( void* out, int outStride, void* in, int inStride, int numPoints ) {
	vec4 *work0 = (vec4*)alloca( sizeof(vec4) * (numPoints + 32) );
	vec4 *work1 = (vec4*)alloca( sizeof(vec4) * (numPoints + 32) );
	int out0;
	int out1;

	// specific to D3D persp matrix
	out0 = clipNearPlane0( work0, sizeof(vec4), in, inStride, numPoints );

	out1 = clipPlaneW( work1, sizeof(vec4), work0, sizeof(vec4), out0, 0 );
	out0 = clipPlaneNegW( work0, sizeof(vec4), work1, sizeof(vec4), out1, 0 );

	out1 = clipPlaneW( work1, sizeof(vec4), work0, sizeof(vec4), out0, 1 );
	out0 = clipPlaneNegW( work0, sizeof(vec4), work1, sizeof(vec4), out1, 1 );

	out1 = clipPlaneW( out, outStride, work0, sizeof(vec4), out0, 2 );

	return out1;
}

int clipPlanes( void* out, int outStride, void* in, int inStride, int numPoints ) {
	vec4 *work0 = (vec4*)alloca( sizeof(vec4) * (numPoints + 32) );
	vec4 *work1 = (vec4*)alloca( sizeof(vec4) * (numPoints + 32) );
	int out0;
	int out1;

	out0 = clipPlaneNegW( work0, sizeof(vec4), in, inStride, numPoints, 2 );

	out1 = clipPlaneW( work1, sizeof(vec4), work0, sizeof(vec4), out0, 0 );
	out0 = clipPlaneNegW( work0, sizeof(vec4), work1, sizeof(vec4), out1, 0 );

	out1 = clipPlaneW( work1, sizeof(vec4), work0, sizeof(vec4), out0, 1 );
	out0 = clipPlaneNegW( work0, sizeof(vec4), work1, sizeof(vec4), out1, 1 );

	out1 = clipPlaneW( out, outStride, work0, sizeof(vec4), out0, 2 );

	return out1;
}

int clipPlanesNoNear( void* out, int outStride, void* in, int inStride, int numPoints ) {
	vec4 *work0 = (vec4*)alloca( sizeof(vec4) * (numPoints + 32) );
	vec4 *work1 = (vec4*)alloca( sizeof(vec4) * (numPoints + 32) );
	int out0;
	int out1;

	// specific to D3D persp matrix
	out1 = clipPlaneW( work1, sizeof(vec4), in, inStride, numPoints, 0 );
	out0 = clipPlaneNegW( work0, sizeof(vec4), work1, sizeof(vec4), out1, 0 );

	out1 = clipPlaneW( work1, sizeof(vec4), work0, sizeof(vec4), out0, 1 );
	out0 = clipPlaneNegW( work0, sizeof(vec4), work1, sizeof(vec4), out1, 1 );

	out1 = clipPlaneW( out, outStride, work0, sizeof(vec4), out0, 2 );

	return out1;
}


bool clipRequiredNearPlane0( void* in, int inStride, int numPoints ) {
	if ( numPoints < 3 ) {
		return false;
	}

	int lp = numPoints - 1;
	int numOut = 0;
	for (int i=0; i<numPoints; i++) {
		int np = i;
		vec4 *lpnt = (vec4*)((uint8_t*)in + lp * inStride);
		vec4 *npnt = (vec4*)((uint8_t*)in + np * inStride);

		int ls = side( lpnt->z );
		int ns = side( npnt->z );

		if ( (ls & ns) == 0 ) {
			return true;
		}

		lp = np;
	}

	return false;
}

bool clipRequiredPlaneW( void* in, int inStride, int numPoints, int idx ) {
	if ( numPoints < 3 ) {
		return false;
	}

	int lp = numPoints - 1;
	int numOut = 0;
	for (int i=0; i<numPoints; i++) {
		int np = i;
		vec4 *lpnt = (vec4*)((uint8_t*)in + lp * inStride);
		vec4 *npnt = (vec4*)((uint8_t*)in + np * inStride);

		int ls = side( lpnt->w - lpnt->v[idx] );
		int ns = side( npnt->w - npnt->v[idx] );

		if ( (ls & ns) == 0 ) {
			return true;
		}

		lp = np;
	}

	return false;
}

bool clipRequiredPlaneNegW( void* in, int inStride, int numPoints, int idx ) {
	if ( numPoints < 3 ) {
		return false;
	}

	int lp = numPoints - 1;
	int numOut = 0;
	for (int i=0; i<numPoints; i++) {
		int np = i;
		vec4 *lpnt = (vec4*)((uint8_t*)in + lp * inStride);
		vec4 *npnt = (vec4*)((uint8_t*)in + np * inStride);

		int ls = side( lpnt->v[idx] + lpnt->w );
		int ns = side( npnt->v[idx] + npnt->w );

		if ( (ls & ns) == 0 ) {
			return true;
		}

		lp = np;
	}

	return false;
}

