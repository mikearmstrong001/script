#include "matrix.h"
#include "quat.h"

quat mat44::ToQuat() const {
	quat		q;
	float		trace;
	float		s;
	float		t;
	int     	i;
	int			j;
	int			k;

	static int 	next[ 3 ] = { 1, 2, 0 };

	trace = f[ 0 ][ 0 ] + f[ 1 ][ 1 ] + f[ 2 ][ 2 ];

	if ( trace > 0.0f ) {

		t = trace + 1.0f;
		s = 1.f/sqrtf( t ) * 0.5f;

		q.w = s * t;
		q.x = ( f[ 2 ][ 1 ] - f[ 1 ][ 2 ] ) * s;
		q.y = ( f[ 0 ][ 2 ] - f[ 2 ][ 0 ] ) * s;
		q.z = ( f[ 1 ][ 0 ] - f[ 0 ][ 1 ] ) * s;

	} else {

		i = 0;
		if ( f[ 1 ][ 1 ] > f[ 0 ][ 0 ] ) {
			i = 1;
		}
		if ( f[ 2 ][ 2 ] > f[ i ][ i ] ) {
			i = 2;
		}
		j = next[ i ];
		k = next[ j ];

		t = ( f[ i ][ i ] - ( f[ j ][ j ] + f[ k ][ k ] ) ) + 1.0f;
		s = 1.f/sqrtf( t ) * 0.5f;

		q.f[i] = s * t;
		q.f[3] = ( f[ k ][ j ] - f[ j ][ k ] ) * s;
		q.f[j] = ( f[ j ][ i ] + f[ i ][ j ] ) * s;
		q.f[k] = ( f[ k ][ i ] + f[ i ][ k ] ) * s;
	}
	return q;
}
