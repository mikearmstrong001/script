#include "loader_obj.h"
#include "file_system.h"
#include "vectormath/cpp/vectormath_aos.h"
#include "TangentSpaceCalculation.h"

struct vec4
{
	float x, y, z, w;
};

struct vec3
{
	float x, y, z;
};

struct vec2
{
	float x, y;
};

struct Triangle
{
    unsigned short  index[3];
};


struct vertNormal
{
	int vIndex;
	int nIndex;
	int tIndex;
};

static int AddUnique( std::vector<vec3> &v, float *f )
{
	for (unsigned int i=0; i<v.size(); i++)
	{
		if ( memcmp( &v[i], f, sizeof(float)*3 ) == 0 )
		{
			return i;
		}
	}
	vec3 nv = { f[0], f[1], f[2] };
	v.push_back( nv );
	return v.size()-1;
}

static int AddUnique( std::vector<vec2> &v, float *f )
{
	for (unsigned int i=0; i<v.size(); i++)
	{
		if ( memcmp( &v[i], f, sizeof(float)*2 ) == 0 )
		{
			return i;
		}
	}
	vec2 nv = { f[0], f[1] };
	v.push_back( nv );
	return v.size()-1;
}

static ModelVert MakeModelVert( const vec3 &p, const vec3 &n, const vec2 &t )
{
	ModelVert v = { { p.x, p.y, p.z }, { n.x, n.y, n.z }, { t.x, t.y } };
	return v;
}

static ModelVert MakeModelVert( const vec3 &p, const vec3 &n, const vec2 &t, const vec2 &t2 )
{
	ModelVert v = { { p.x, p.y, p.z }, { n.x, n.y, n.z }, { t.x, t.y }, {0.f}, {t2.x, t2.y} };
	return v;
}

#if 0
static ModelVert MakeModelVert( const float *p, const float *n, const float *t, const float *tangent, const float *bitangent )
{
	Vectormath::Aos::Vector3 vNormal( n[0], n[1], n[2] );
	Vectormath::Aos::Vector3 vBitangent( bitangent[0], bitangent[1], bitangent[2] );
	Vectormath::Aos::Vector3 vTangent( tangent[0], tangent[1], tangent[2] );
    // Gram-Schmidt orthogonalize
	vTangent = Vectormath::Aos::normalize(vTangent - vNormal * Vectormath::Aos::dot(vNormal, vTangent));
    
    // Calculate handedness
	float handedness = (Vectormath::Aos::dot(Vectormath::Aos::cross(vNormal, vTangent), vBitangent) < 0.f) ? -1.f : 1.f;
	ModelVert v = { { p[0], p[1], p[2] }, { n[0], n[1], n[2]}, { t[0], t[1] }, { vTangent.getX(), vTangent.getY(), vTangent.getZ(), handedness } };
	return v;
}
#endif

static ModelVert MakeModelVert( const ModelVert &mv, const float *n, const float *tangent, const float *bitangent )
{
	Vectormath::Aos::Vector3 vNormal( n[0], n[1], n[2] );
	Vectormath::Aos::Vector3 vBitangent( bitangent[0], bitangent[1], bitangent[2] );
	Vectormath::Aos::Vector3 vT( tangent[0], tangent[1], tangent[2] );
    // Gram-Schmidt orthogonalize
	Vectormath::Aos::Vector3 vTangent = Vectormath::Aos::normalize(vT - vNormal * Vectormath::Aos::dot(vNormal, vT));
    
    // Calculate handedness
	float handedness = (Vectormath::Aos::dot(Vectormath::Aos::cross(vNormal, vTangent), vBitangent) < 0.f) ? -1.f : 1.f;
	ModelVert v = { { mv.p[0], mv.p[1], mv.p[2] }, { n[0], n[1], n[2]}, { mv.t[0], mv.t[1] }, { vTangent.getX(), vTangent.getY(), vTangent.getZ(), handedness }, { mv.t2[0], mv.t2[1] }, mv.c, mv.pad };
	return v;
}


struct SurfaceProxy
{
	std::vector< unsigned short > &m_indices;
	std::vector<vec3> m_pos;
	std::vector<vec3> m_normals;
	std::vector<vec2> m_tex;
	std::vector<vertNormal> m_remap;

	SurfaceProxy( std::vector< ModelVert > &verts, std::vector< unsigned short > &indices ) : m_indices(indices) 
	{
		for (unsigned int i=0; i<verts.size(); i++)
		{
			vertNormal vn;
			vn.vIndex = AddUnique( m_pos, verts[i].p );
			vn.nIndex = AddUnique( m_normals, verts[i].n );
			vn.tIndex = AddUnique( m_tex, verts[i].t );
			m_remap.push_back(vn);
		}
	}


	unsigned int GetTriangleCount() const
	{
		return m_indices.size()/3;
	}

	void GetTriangleIndices( const unsigned int indwTriNo, unsigned int outdwPos[3], unsigned int outdwNorm[3], unsigned int outdwUV[3] ) const
	{
		int i0 = m_indices[indwTriNo*3+0];
		int i1 = m_indices[indwTriNo*3+1];
		int i2 = m_indices[indwTriNo*3+2];

		const vertNormal &vn0 = m_remap[i0];
		const vertNormal &vn1 = m_remap[i1];
		const vertNormal &vn2 = m_remap[i2];

		outdwPos[0] = vn0.vIndex;
		outdwPos[1] = vn1.vIndex;
		outdwPos[2] = vn2.vIndex;
		outdwNorm[0] = vn0.nIndex;
		outdwNorm[1] = vn1.nIndex;
		outdwNorm[2] = vn2.nIndex;
		outdwUV[0] = vn0.tIndex;
		outdwUV[1] = vn1.tIndex;
		outdwUV[2] = vn2.tIndex;
	}

	void GetPos( const unsigned int indwPos, float outfPos[3] ) const
	{
		outfPos[0] = m_pos[indwPos].x;
		outfPos[1] = m_pos[indwPos].y;
		outfPos[2] = m_pos[indwPos].z;
	}

	void GetNormal( const unsigned int indwPos, float outfNorm[3] ) const
	{
		outfNorm[0] = m_normals[indwPos].x;
		outfNorm[1] = m_normals[indwPos].y;
		outfNorm[2] = m_normals[indwPos].z;
	}

	void GetUV( const unsigned int indwPos, float outfUV[2] ) const
	{
		outfUV[0] = m_tex[indwPos].x;
		outfUV[1] = m_tex[indwPos].y;
	}
};


static unsigned int hash( const ModelVert &v )
{
	return (unsigned int)(v.p[0] * 2876312.f);
}

static void ClearHashes( std::vector<int> vertHashes[256] )
{
	for (int i=0; i<256; i++)
	{
		vertHashes[i].clear();
	}
}

static int AddModelVert( std::vector<int> vertHashes[256], std::vector< ModelVert > &verts, const ModelVert &v )
{
	unsigned int h = hash( v ) & 255;
	for (unsigned int i=0; i<vertHashes[h].size(); i++)
	{
		int idx = vertHashes[h][i];
		if ( memcmp( &verts[idx], &v, sizeof(v) ) == 0 )
		{
			return idx;
		}
	}
	int idx = verts.size();
	vertHashes[h].push_back( idx );
	verts.push_back( v );
	return idx;

	for (unsigned int i=0; i<verts.size(); i++)
	{
		if ( memcmp( &verts[i], &v, sizeof(v) ) == 0 )
		{
			return i;
		}
	}
	verts.push_back( v );
	return verts.size()-1;
}

static void FinishSurface( std::vector< ModelVert > &verts, std::vector< unsigned short > &indices )
{
	CTangentSpaceCalculation<SurfaceProxy,false> tangentCalculator;

	SurfaceProxy input( verts, indices );

	tangentCalculator.CalculateTangentSpace( input );

	std::vector<int> vertHashes[256];
	std::vector< ModelVert > newVerts;
	std::vector< unsigned short > newIndices;
	for (unsigned int i=0; i<indices.size()/3; i++)
	{
		unsigned int baseIndices[3];
		tangentCalculator.GetTriangleBaseIndices( i, baseIndices );
		for (int j=0; j<3; j++)
		{
			float tangent[3];
			float bitangent[3];
			float normal[3];
			tangentCalculator.GetBase( baseIndices[j], tangent, bitangent, normal );

			ModelVert oldMv = verts[ indices[i*3+j] ];

			ModelVert mv = MakeModelVert( oldMv, normal, tangent, bitangent );

			newIndices.push_back( (unsigned short)AddModelVert( vertHashes, newVerts, mv ) );
		}
	}
	verts.swap( newVerts );
	indices.swap( newIndices );
}




template <class T>
static T* copyAndAlloc( std::vector<T> &vec, int &num )
{
	T* out = new T[vec.size()];
	for (unsigned int i=0; i<vec.size(); i++)
	{
		out[i] = vec[i];
	}
	num = vec.size();
	return out;
}


bool LoadObj( std::vector< ModelGeometry * > &geoms, std::vector< std::string > &materials, std::vector< std::string > &surfaceNames, const char *filename )
{
	char buffer[512];
	PHYSFS_File *f = PHYSFS_openRead( fs::MakeCanonicalForm( buffer, filename ) );
	if ( f )
	{
		PHYSFS_setBuffer( f, 128*1024 );

		char line[512];
		std::vector< vec3 > points;  
		std::vector< vec3 > normals;
		std::vector< vec2 > texcoords;

		std::vector< ModelVert > verts;
		std::vector< unsigned short > indices;

		std::vector<int> vertHashes[256];

		points.reserve( 16384 );
		normals.reserve( 16384 );
		texcoords.reserve( 16384 );

		points.push_back( vec3() );
		normals.push_back( vec3() );
		texcoords.push_back( vec2() );

		ModelGeometry *geom = new ModelGeometry;
		std::string currentMaterial = "_default";

		std::string currentGroup;

		while ( PHYSFS_gets( line, sizeof(line), f ) )
		{
			vec3 v3;
			vec2 v2;
			int ip[3];
			int in[3];
			int it[3];
			int it2[3];
			char str[512];

			if ( sscanf( line, "v %f %f %f", &v3.x, &v3.y, &v3.z ) == 3 )
			{
				points.push_back( v3 );
			} else
			if ( sscanf( line, "vn %f %f %f", &v3.x, &v3.y, &v3.z ) == 3 )
			{
				normals.push_back( v3 );
			} else
			if ( sscanf( line, "vt %f %f", &v2.x, &v2.y ) == 2 )
			{
				texcoords.push_back( v2 );
			} else
			if ( sscanf( line, "g %s", str ) == 1 )
			{
				if ( indices.size() )
				{
					FinishSurface( verts, indices );
					geom->m_indices = copyAndAlloc( indices, geom->m_numIndices );
					geom->m_verts = copyAndAlloc( verts, geom->m_numVerts );
					indices.clear();
					verts.clear();
					surfaceNames.push_back( currentGroup );
					geoms.push_back( geom );
					materials.push_back( currentMaterial );
					geom = new ModelGeometry;
					ClearHashes( vertHashes );
				}
				currentGroup = str;
			} else
			if ( sscanf( line, "usemtl %s", str ) == 1 )
			{
				if ( indices.size() )
				{
					FinishSurface( verts, indices );
					geom->m_indices = copyAndAlloc( indices, geom->m_numIndices );
					geom->m_verts = copyAndAlloc( verts, geom->m_numVerts );
					indices.clear();
					verts.clear();
					geoms.push_back( geom );
					surfaceNames.push_back( currentGroup );
					materials.push_back( currentMaterial );
					geom = new ModelGeometry;
					ClearHashes( vertHashes );
				}
				currentMaterial = str;
			} else
			if ( sscanf( line, "f %d/%d/%d/%d %d/%d/%d/%d %d/%d/%d/%d", &ip[0], &it[0], &in[0], &it2[0],
				&ip[1], &it[1], &in[1], &it2[1],
				&ip[2], &it[2], &in[2], &it2[2] ) == 12 )
			{
				for (int i=0; i<3; i++)
				{
					if ( ip[i] < 0 ) ip[i] += points.size();
					if ( it[i] < 0 ) it[i] += texcoords.size();
					if ( in[i] < 0 ) in[i] += normals.size();
					if ( it2[i] < 0 ) it2[i] += texcoords.size();
				}

				ModelVert v0 = MakeModelVert( points[ip[0]], normals[in[0]], texcoords[it[0]], texcoords[it2[0]] );
				ModelVert v1 = MakeModelVert( points[ip[1]], normals[in[1]], texcoords[it[1]], texcoords[it2[1]] );
				ModelVert v2 = MakeModelVert( points[ip[2]], normals[in[2]], texcoords[it[2]], texcoords[it2[2]] );

				int i0 = AddModelVert( vertHashes, verts, v0 );
				int i1 = AddModelVert( vertHashes, verts, v1 );
				int i2 = AddModelVert( vertHashes, verts, v2 );

				indices.push_back( (unsigned short)i0 );
				indices.push_back( (unsigned short)i1 );
				indices.push_back( (unsigned short)i2 );
			} else
			if ( sscanf( line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &ip[0], &it[0], &in[0],
				&ip[1], &it[1], &in[1],
				&ip[2], &it[2], &in[2] ) == 9 )
			{
				for (int i=0; i<3; i++)
				{
					if ( ip[i] < 0 ) ip[i] += points.size();
					if ( it[i] < 0 ) it[i] += texcoords.size();
					if ( in[i] < 0 ) in[i] += normals.size();
				}

				ModelVert v0 = MakeModelVert( points[ip[0]], normals[in[0]], texcoords[it[0]] );
				ModelVert v1 = MakeModelVert( points[ip[1]], normals[in[1]], texcoords[it[1]] );
				ModelVert v2 = MakeModelVert( points[ip[2]], normals[in[2]], texcoords[it[2]] );

				int i0 = AddModelVert( vertHashes, verts, v0 );
				int i1 = AddModelVert( vertHashes, verts, v1 );
				int i2 = AddModelVert( vertHashes, verts, v2 );

				indices.push_back( (unsigned short)i0 );
				indices.push_back( (unsigned short)i1 );
				indices.push_back( (unsigned short)i2 );
			}
		}
		if ( indices.size() )
		{
			FinishSurface( verts, indices );
			geom->m_indices = copyAndAlloc( indices, geom->m_numIndices );
			geom->m_verts = copyAndAlloc( verts, geom->m_numVerts );
			indices.clear();
			verts.clear();
			surfaceNames.push_back( currentGroup );
			geoms.push_back( geom );
			materials.push_back( currentMaterial );
		} else
		{
			delete geom;
		}
		PHYSFS_close(f);
	}
	return true;
}

