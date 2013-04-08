#ifndef ER_HEADER_MATHS_H
#define ER_HEADER_MATHS_H

#include "../erbase.h"
#include "math.h"

#if defined(ERLIB_CPU_X86)
#include < xmmintrin.h >
#endif

const static float PI = 3.14159265358979323846f;
const static float MAX_FLT = 3.402823466e+38F;
static const double floatutil_xs_doublemagic = (6755399441055744.0); // 2^52 * 1.5
static const double floatutil_xs_doublemagicdelta = (1.5e-8);                         //almost .5f = .5f + 1e^(number of exp bit)
static const double floatutil_xs_doublemagicroundeps = (0.5f - floatutil_xs_doublemagicdelta);       //almost .5f = .5f - 1e^(number of exp bit)

union DoubleAnd64
{
  uint64_t  i;
  double  d;
};

class Maths {
public:


	ERINLINE static float Atof( const char *str ) {
		return (float)atof(str);
	}

	ERINLINE static float Deg2Rad( float d ) {
		return d / 180.f * PI;
	}

	ERINLINE static int Clamp( int v, int mn, int mx ) {
		if ( v < mn ) {
			return mn;
		} else
		if ( v > mx ) {
			return mx;
		} else {
			return v;
		}
	}

	ERINLINE static float Clampf( float v, float mn, float mx ) {
		if ( v < mn ) {
			return mn;
		} else
		if ( v > mx ) {
			return mx;
		} else {
			return v;
		}
	}

	ERINLINE static void SinCos( float &sa, float &ca, float a ) {
		sa = sinf( a );
		ca = cosf( a );
	}

	ERINLINE static float Min( float a, float b ) {
		return a<b ? a : b;
	}

	ERINLINE static float Max( float a, float b ) {
		return a>b ? a : b;
	}

	ERINLINE static int Min( int a, int b ) {
		return a<b ? a : b;
	}

	ERINLINE static int Max( int a, int b ) {
		return a>b ? a : b;
	}

	ERFORCEINLINE static float FSel( float a, float x, float y ) {
		return a >= 0.f ? x : y;
	}

	ERFORCEINLINE static int ISel( int a, int x, int y ) {
		int mask = a >> 31;
		return x + ((y-x) & mask);
	}

	// ftoi_round : *banker* rounding!
	ERFORCEINLINE static  int Ftoi_round(const double val)
	{
	  DoubleAnd64 dunion;
	  dunion.d = val + floatutil_xs_doublemagic;
	  return (int) dunion.i; // just cast to grab the bottom bits
	}

#if defined(ERLIB_CPU_X86)
	ERFORCEINLINE static int Ftoi_trunc_sse(const float f)
	{
	  return _mm_cvtt_ss2si( _mm_set_ss( f ) );
	}
#endif

#if defined(ERLIB_CPU_X86)
	ERFORCEINLINE static int Ftoi_round_sse(const float f)
	{
	  return _mm_cvt_ss2si( _mm_set_ss( f ) );
	}
#endif

	ERFORCEINLINE static int Ftoi_floor(const double val)
	{
		return Ftoi_round(val - floatutil_xs_doublemagicroundeps);
	}

	ERFORCEINLINE static int Ftoi_ceil(const double val)
	{
		return Ftoi_round(val + floatutil_xs_doublemagicroundeps);
	}

	// ftoi_trunc_xs = Sree's truncate
	ERFORCEINLINE static int Ftoi_trunc_xs(const double val)
	{
	  return (val<0) ? Ftoi_round(val+floatutil_xs_doublemagicroundeps) : 
					   Ftoi_round(val-floatutil_xs_doublemagicroundeps);
	}

};

#endif