#ifndef TESTING_MODEL_STRUCTS_H
#define TESTING_MODEL_STRUCTS_H

#include <vector>
#include "vectormath/cpp/vectormath_aos.h"

enum
{
	_RP_Start = 0,

	RP_LightPrepass = _RP_Start,
	RP_LightPrepass_Lights,
	RP_LightPrepass_LitOpaque,
	RP_LitOpaque,
	RP_Opaque,
	RP_Transparent,
	RP_LightPrepass_Stereoscopic,
	RP_Postprocess,
	RP_Debug,

	_RP_End3D,
	_RP_Start2D = _RP_End3D,

	RP_Debug2D = _RP_Start2D,

	_RP_End,
};


class Material;
struct TechniquePass;

struct ModelVert
{
	float p[3]; // 12
	float n[3]; // 12
	float t[2]; // 8
	float tan[4]; // 16
	float t2[2]; // 8
	unsigned int c; // 4
	unsigned int pad; // 4
};

struct ModelVertWeight
{
	unsigned char Index[4];
	unsigned char Weight[4];
};

static int checksize[ sizeof(ModelVert) == 64 ? 1 : -1 ] = {0};

struct ModelGeometry
{
	ModelVert *m_verts;
	ModelVertWeight *m_weights;
	int m_numVerts;
	unsigned short *m_indices;
	int m_numIndices;

	mutable unsigned int m_verts_VC;
	mutable unsigned int m_weights_VC;
	mutable unsigned int m_indices_VC;

	ModelGeometry() : m_verts(NULL), m_weights(NULL), m_numVerts(0), m_indices(NULL), m_numIndices(0), m_verts_VC(0), m_weights_VC(0), m_indices_VC(0)
	{
	}
};

struct RenderSurface
{
	std::string name;
	const ModelGeometry *geom;
	const Material *mat;
};

struct RenderCommand
{
	Vectormath::Aos::Transform3 m_localToWorld;
	int passIdx;
	const struct MeshLightmapBindSet *lightmapBinds;
	const struct ModelGeometry *geom;
	const class  Material *mat;
	const struct TechniquePass *tech;
	const Vectormath::Aos::Transform3 *joints;
	int  numJoints;
};


#endif


