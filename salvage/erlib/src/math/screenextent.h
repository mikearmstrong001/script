#ifndef ER_HEADER_SCREENEXTENT_H
#define ER_HEADER_SCREENEXTENT_H

#include "maths.h"

struct ScreenExtent {
	float bb[2][2];

	void Init( void ) {
		bb[0][0] = MAX_FLT;
		bb[0][1] = MAX_FLT;
		bb[1][0] = -MAX_FLT;
		bb[1][1] = -MAX_FLT;
	}

	void Init( float mnx, float mny, float mxx, float mxy ) {
		bb[0][0] = mnx;
		bb[0][1] = mny;
		bb[1][0] = mxx;
		bb[1][1] = mxy;
	}


	bool Valid( void ) {
		return bb[0][0] < bb[1][0] && bb[0][1] < bb[1][1];
	}

	void Intersect( const ScreenExtent &other ) {
		bb[0][0] = Maths::Max( bb[0][0], other.bb[0][0] );
		bb[0][1] = Maths::Max( bb[0][1], other.bb[0][1] );
		bb[1][0] = Maths::Min( bb[1][0], other.bb[1][0] );
		bb[1][1] = Maths::Min( bb[1][1], other.bb[1][1] );
	}

	void Combine( const ScreenExtent &other ) {
		bb[0][0] = Maths::Min( bb[0][0], other.bb[0][0] );
		bb[0][1] = Maths::Min( bb[0][1], other.bb[0][1] );
		bb[1][0] = Maths::Max( bb[1][0], other.bb[1][0] );
		bb[1][1] = Maths::Max( bb[1][1], other.bb[1][1] );
	}

	void Add( float x, float y ) {
		bb[0][0] = Maths::Min( bb[0][0], x );
		bb[0][1] = Maths::Min( bb[0][1], y );
		bb[1][0] = Maths::Max( bb[1][0], x );
		bb[1][1] = Maths::Max( bb[1][1], y );
	}
};


#endif
