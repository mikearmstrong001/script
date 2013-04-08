#ifndef ER_HEADER_TRITOOLS_H
#define ER_HEADER_TRITOOLS_H

#include "..\erbase.h"
#include "vec.h"
#include "..\vector.h"

struct VecTangent {
	int index;
	vec3 tan;
	vec3 bitan;
	vec3 nrm;
};

void CalculateTangentArray(int vertexCount, 
						   const uint8_t *vertex, int vertex_stride,
						   const uint8_t *normal, int normal_stride,
						   const uint8_t *texcoord, int texcoord_stride,
						   const int  *indices, int numIndices,
						   Vector<VecTangent> &outverts, Vector<int> &outindexes );

#endif