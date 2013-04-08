#include "plane.h"

#if 0
	 -(	d1 ( N2 * N3 ) + d2 ( N3 * N1 ) + d3 ( N1 * N2 ) )
P =	-------------------------------------------------------------------------
 					N1 . ( N2 * N3 )

#endif

vec3 PLANE_Intersect( const Plane &p1, const Plane &p2, const Plane &p3 ) {
	vec3 n2n3 = CrossP( p2.n, p3.n );
	vec3 n3n1 = CrossP( p3.n, p1.n );
	vec3 n1n2 = CrossP( p1.n, p2.n );

	vec3 p = ( p1.d * n2n3 + p2.d * n3n1 + p3.d * n1n2 ) / -( p1.n * n2n3 );

	return p;
}
