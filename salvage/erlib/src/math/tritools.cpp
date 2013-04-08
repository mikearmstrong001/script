#include "..\erbase.h"
#include "..\vector.h"
#include "tangentspacecalculation.h"
#include "tritools.h"

struct MyProxy
{
	unsigned int numTris;
	const uint8_t *vertex;
	int vertex_stride;
	const uint8_t *normal;
	int normal_stride;
	const uint8_t *texcoord;
	int texcoord_stride;
	const int  *indices;
	int numIndices;
	Vector<int> posremap;
	Vector<int> nrmremap;
	Vector<int> tcremap;

	//! not virtual to save the call overhead
	//! /return 0..
	unsigned int GetTriangleCount() const {
		return numTris;
	}

	//! not virtual to save the call overhead
	//! /param indwTriNo 0..
	//! /param outdwPos
	//! /param outdwNorm
	//! /param outdwUV
	void GetTriangleIndices( const unsigned int indwTriNo, unsigned int outdwPos[3], unsigned int outdwNorm[3], unsigned int outdwUV[3] ) const {
		outdwPos[0] = posremap[ indices[indwTriNo*3+0] ];
		outdwPos[1] = posremap[ indices[indwTriNo*3+1] ];
		outdwPos[2] = posremap[ indices[indwTriNo*3+2] ];

		outdwNorm[0] = nrmremap[ indices[indwTriNo*3+0] ];
		outdwNorm[1] = nrmremap[ indices[indwTriNo*3+1] ];
		outdwNorm[2] = nrmremap[ indices[indwTriNo*3+2] ];

		outdwUV[0] = tcremap[ indices[indwTriNo*3+0] ];
		outdwUV[1] = tcremap[ indices[indwTriNo*3+1] ];
		outdwUV[2] = tcremap[ indices[indwTriNo*3+2] ];
	}

	//! not virtual to save the call overhead
	//! /param indwPos 0..
	//! /param outfPos
	void GetPos( const unsigned int indwPos, float outfPos[3] ) const {
		outfPos[0] = ((const vec3*)(vertex + vertex_stride * indwPos))->x;
		outfPos[1] = ((const vec3*)(vertex + vertex_stride * indwPos))->y;
		outfPos[2] = ((const vec3*)(vertex + vertex_stride * indwPos))->z;
	}

	//! not virtual to save the call overhead
	//! /param indwPos 0..
	//! /param outfUV 
	void GetUV( const unsigned int indwPos, float outfUV[2] ) const {
		outfUV[0] = ((const vec2*)(texcoord + texcoord_stride * indwPos))->x;
		outfUV[1] = ((const vec2*)(texcoord + texcoord_stride * indwPos))->y;
	}
};


void CalculateTangentArray(int vertexCount, 
						   const uint8_t *vertex, int vertex_stride,
						   const uint8_t *normal, int normal_stride,
						   const uint8_t *texcoord, int texcoord_stride,
						   const int  *indices, int numIndices,
						   Vector<VecTangent> &outverts, Vector<int> &outindexes )
{
	MyProxy proxy;

	proxy.posremap.EnsureAllocated( vertexCount );
	for (int i=0; i<vertexCount; i++) {
		int found = -1;
		for (int j=0; j<i; j++) {
			float d = (*(vec3*)(vertex + i * vertex_stride) - *(vec3*)(vertex + j * vertex_stride)).LengthSquared();
			if ( d < 0.001f ) {
				found = j;
				break;
			}
		}
		if ( found == -1 ) {
			found = i;
		}
		proxy.posremap.PushBack( found ); 
	}

	proxy.nrmremap.EnsureAllocated( vertexCount );
	for (int i=0; i<vertexCount; i++) {
		int found = -1;
		for (int j=0; j<i; j++) {
			float d = (*(vec3*)(normal + i * normal_stride) - *(vec3*)(normal + j * normal_stride)).LengthSquared();
			if ( d < 0.001f ) {
				found = j;
				break;
			}
		}
		if ( found == -1 ) {
			found = i;
		}
		proxy.nrmremap.PushBack( found ); 
	}

	proxy.tcremap.EnsureAllocated( vertexCount );
	for (int i=0; i<vertexCount; i++) {
		int found = -1;
		for (int j=0; j<i; j++) {
			float d = (*(vec2*)(texcoord + i * texcoord_stride) - *(vec2*)(texcoord + j * texcoord_stride)).LengthSquared();
			if ( d < 0.001f ) {
				found = j;
				break;
			}
		}
		if ( found == -1 ) {
			found = i;
		}
		proxy.tcremap.PushBack( found ); 
	}


	proxy.numTris = numIndices/3;
	proxy.vertex = vertex;
	proxy.vertex_stride = vertex_stride;
	proxy.normal = normal;
	proxy.normal_stride = normal_stride;
	proxy.texcoord = texcoord;
	proxy.texcoord_stride = texcoord_stride;
	proxy.indices = indices;

	CTangentSpaceCalculation<MyProxy> MyTangent;
	MyTangent.CalculateTangentSpace(proxy);

	for (int i=0; i<numIndices/3; i++) {
		unsigned int outdwBase[3];
		MyTangent.GetTriangleBaseIndices( i, outdwBase );
		for (int j=0; j<3; j++) {
			VecTangent vt;
			vt.index = indices[i*3+j];
			MyTangent.GetBase( outdwBase[j], &vt.tan.x, &vt.bitan.x, &vt.nrm.x );
			int idx = outverts.Size();
			outverts.PushBack( vt );
			outindexes.PushBack( idx );
		}
	}
}

