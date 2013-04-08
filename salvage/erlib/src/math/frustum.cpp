#include "frustum.h"

#define MIDX(a,b) ((((a)-1)*4)+((b)-1))

void Frustum::FromMatrixD3D( const float *f, bool normalise ) {
	// Left clipping plane
	planes[0].n.x = f[MIDX(1,4)] + f[MIDX(1,1)];//comboMatrix._14 + comboMatrix._11;
	planes[0].n.y = f[MIDX(2,4)] + f[MIDX(2,1)];//comboMatrix._24 + comboMatrix._21;
	planes[0].n.z = f[MIDX(3,4)] + f[MIDX(3,1)];//comboMatrix._34 + comboMatrix._31;
	planes[0].d = f[MIDX(4,4)] + f[MIDX(4,1)];//comboMatrix._44 + comboMatrix._41;
	// Right clipping plane
	planes[1].n.x = f[MIDX(1,4)] - f[MIDX(1,1)];//comboMatrix._14 - comboMatrix._11;
	planes[1].n.y = f[MIDX(2,4)] - f[MIDX(2,1)];//comboMatrix._24 - comboMatrix._21;
	planes[1].n.z = f[MIDX(3,4)] - f[MIDX(3,1)];//comboMatrix._34 - comboMatrix._31;
	planes[1].d = f[MIDX(4,4)] - f[MIDX(4,1)];//comboMatrix._44 - comboMatrix._41;
	// Top clipping plane
	planes[2].n.x = f[MIDX(1,4)] - f[MIDX(1,2)];//comboMatrix._14 - comboMatrix._12;
	planes[2].n.y = f[MIDX(2,4)] - f[MIDX(2,2)];//comboMatrix._24 - comboMatrix._22;
	planes[2].n.z = f[MIDX(3,4)] - f[MIDX(3,2)];//comboMatrix._34 - comboMatrix._32;
	planes[2].d = f[MIDX(4,4)] - f[MIDX(4,2)];//comboMatrix._44 - comboMatrix._42;
	// Bottom clipping plane
	planes[3].n.x = f[MIDX(1,4)] + f[MIDX(1,2)];//comboMatrix._14 + comboMatrix._12;
	planes[3].n.y = f[MIDX(2,4)] + f[MIDX(2,2)];//comboMatrix._24 + comboMatrix._22;
	planes[3].n.z = f[MIDX(3,4)] + f[MIDX(3,2)];//comboMatrix._34 + comboMatrix._32;
	planes[3].d = f[MIDX(4,4)] + f[MIDX(4,2)];//comboMatrix._44 + comboMatrix._42;
	// Near clipping plane
	planes[4].n.x = f[MIDX(1,3)];//comboMatrix._13;
	planes[4].n.y = f[MIDX(2,3)];//comboMatrix._23;
	planes[4].n.z = f[MIDX(3,3)];//comboMatrix._33;
	planes[4].d = f[MIDX(4,3)];//comboMatrix._43;
	// Far clipping plane
	planes[5].n.x = f[MIDX(1,4)] - f[MIDX(1,3)];//comboMatrix._14 - comboMatrix._13;
	planes[5].n.y = f[MIDX(2,4)] - f[MIDX(2,3)];//comboMatrix._24 - comboMatrix._23;
	planes[5].n.z = f[MIDX(3,4)] - f[MIDX(3,3)];//comboMatrix._34 - comboMatrix._33;
	planes[5].d = f[MIDX(4,4)] - f[MIDX(4,3)];//comboMatrix._44 - comboMatrix._43;
	// Normalize the plane equations, if requested
	if ( normalise ) {
		planes[0].Normalise();
		planes[1].Normalise();
		planes[2].Normalise();
		planes[3].Normalise();
		planes[4].Normalise();
		planes[5].Normalise();
	}

	points[0] = PLANE_Intersect( planes[4], planes[0], planes[2] );
	points[1] = PLANE_Intersect( planes[4], planes[2], planes[1] );
	points[2] = PLANE_Intersect( planes[4], planes[1], planes[3] );
	points[3] = PLANE_Intersect( planes[4], planes[3], planes[0] );

	points[4] = PLANE_Intersect( planes[5], planes[0], planes[2] );
	points[5] = PLANE_Intersect( planes[5], planes[2], planes[1] );
	points[6] = PLANE_Intersect( planes[5], planes[1], planes[3] );
	points[7] = PLANE_Intersect( planes[5], planes[3], planes[0] );

}

bool Frustum::AABBIntersects( const float *mn, const float *mx ) {
	for (int i=0; i<6; i++) {
		float closetPnt[3];
		Plane &pl = planes[i];

		closetPnt[0] = pl.n.x > 0.f ? mx[0] : mn[0];
		closetPnt[1] = pl.n.y > 0.f ? mx[1] : mn[1];
		closetPnt[2] = pl.n.z > 0.f ? mx[2] : mn[2];

		float dist = pl.n.x * closetPnt[0] +
			pl.n.y * closetPnt[1] +
			pl.n.z * closetPnt[2] +
			pl.d;

		if ( dist < 0.f ) {
			return false;
		}

	}

	return true;
}


void Frustum::GenPoints( vec3 p[8], float minz, float maxz ) {
	float dist = planes[5].d - -planes[4].d;

	Plane nearp = planes[4];
	nearp.d -= minz * dist;
	p[0] = PLANE_Intersect( nearp, planes[0], planes[2] );
	p[1] = PLANE_Intersect( nearp, planes[2], planes[1] );
	p[2] = PLANE_Intersect( nearp, planes[1], planes[3] );
	p[3] = PLANE_Intersect( nearp, planes[3], planes[0] );

	Plane farp = planes[5];
	farp.d = -planes[4].d + maxz * dist;
	p[4] = PLANE_Intersect( farp, planes[0], planes[2] );
	p[5] = PLANE_Intersect( farp, planes[2], planes[1] );
	p[6] = PLANE_Intersect( farp, planes[1], planes[3] );
	p[7] = PLANE_Intersect( farp, planes[3], planes[0] );
}
