#include "dxut.h"
#include "loader_meshmd5.h"
#include <string>
#include <vector>
#include <algorithm>
#include "tokeniser.h"
#include "faff.h"
#include "file_system.h"
#include "model_structs.h"
#include "TangentSpaceCalculation.h"
#include "anim.h"
#include "edge\libedgeanimtool\libedgeanimtool_skeleton.h"

struct vec2
{
	float x, y;
};

struct weight {
	int idx;
	float w;
	int bw;

	bool operator<( const weight &other ) const {
		return w > other.w;
	}
};

struct vertinfo {
	vec2 uv;
	int w;
	int numw;

	Vectormath::Aos::Vector3 final;
	std::vector< weight > weights;
};

struct weightinfo {
	int joint;
	float w;
	Vectormath::Aos::Vector3 ofs;
};

struct vertNormal
{
	int vIndex;
	int tIndex;
};

static int AddUnique( std::vector<Vectormath::Aos::Vector3> &v, Vectormath::Aos::Vector3 *f )
{
	for (unsigned int i=0; i<v.size(); i++)
	{
		if ( memcmp( &v[i], f, sizeof(float)*3 ) == 0 )
		{
			return i;
		}
	}
	Vectormath::Aos::Vector3 nv = *f;
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

static void MakeModelVert( ModelVert &v, ModelVertWeight &vw, const vertinfo &mv, const float *n, const float *tangent, const float *bitangent )
{
	Vectormath::Aos::Vector3 vNormal( n[0], n[1], n[2] );
	Vectormath::Aos::Vector3 vBitangent( bitangent[0], bitangent[1], bitangent[2] );
	Vectormath::Aos::Vector3 vT( tangent[0], tangent[1], tangent[2] );
    // Gram-Schmidt orthogonalize
	Vectormath::Aos::Vector3 vTangent = Vectormath::Aos::normalize(vT - vNormal * Vectormath::Aos::dot(vNormal, vT));
    
    // Calculate handedness
	float handedness = (Vectormath::Aos::dot(Vectormath::Aos::cross(vNormal, vTangent), vBitangent) < 0.f) ? -1.f : 1.f;
	ModelVert ov = { { mv.final[0], mv.final[1], mv.final[2] }, { n[0], n[1], n[2]}, { mv.uv.x, mv.uv.y }, { vTangent.getX(), vTangent.getY(), vTangent.getZ(), handedness }, { 0.f, 0.f }, 0xffffffff, 1 };
	ModelVertWeight ovw = { { (unsigned char)mv.weights[0].idx, (unsigned char)mv.weights[1].idx, (unsigned char)mv.weights[2].idx, (unsigned char)mv.weights[3].idx }, 
	{ (unsigned char)mv.weights[0].bw, (unsigned char)mv.weights[1].bw, (unsigned char)mv.weights[2].bw, (unsigned char)mv.weights[3].bw } };
	v = ov;
	vw = ovw;
}



struct Md5MeshSurfaceProxy
{
	std::vector< unsigned short > &m_indices;
	std::vector<Vectormath::Aos::Vector3> m_pos;
	std::vector<vec2> m_tex;
	std::vector<vertNormal> m_remap;

	Md5MeshSurfaceProxy( std::vector< vertinfo > &verts, std::vector< unsigned short > &indices ) : m_indices(indices) 
	{
		for (unsigned int i=0; i<verts.size(); i++)
		{
			vertNormal vn;
			vn.vIndex = AddUnique( m_pos, &verts[i].final );
			vn.tIndex = AddUnique( m_tex, &verts[i].uv.x );
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
		outdwNorm[0] = 0;//vn0.nIndex;
		outdwNorm[1] = 0;//vn1.nIndex;
		outdwNorm[2] = 0;//vn2.nIndex;
		outdwUV[0] = vn0.tIndex;
		outdwUV[1] = vn1.tIndex;
		outdwUV[2] = vn2.tIndex;
	}

	void GetPos( const unsigned int indwPos, float outfPos[3] ) const
	{
		outfPos[0] = m_pos[indwPos][0];
		outfPos[1] = m_pos[indwPos][1];
		outfPos[2] = m_pos[indwPos][2];
	}
	void GetNormal( const unsigned int indwPos, float outfNorm[3] ) const
	{
		assert(0);
	}
#if 0
	{
		outfNorm[0] = m_normals[indwPos].x;
		outfNorm[1] = m_normals[indwPos].y;
		outfNorm[2] = m_normals[indwPos].z;
	}
#endif

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

static int AddModelVert( std::vector<int> vertHashes[256], std::vector< ModelVert > &verts, std::vector< ModelVertWeight > &vertWeights, const ModelVert &v, const ModelVertWeight &vw )
{
	unsigned int h = hash( v ) & 255;
	for (unsigned int i=0; i<vertHashes[h].size(); i++)
	{
		int idx = vertHashes[h][i];
		if ( memcmp( &verts[idx], &v, sizeof(v) ) == 0 &&
			memcpy( &vertWeights[idx], &vw, sizeof(vw) ) == 0 )
		{
			return idx;
		}
	}
	int idx = verts.size();
	vertHashes[h].push_back( idx );
	verts.push_back( v );
	vertWeights.push_back( vw );
	return idx;
}

static void FinishSurface( std::vector< ModelVert > &newVerts,std::vector< ModelVertWeight > &newVertWeights,std::vector< vertinfo > &verts, std::vector< unsigned short > &indices )
{
	CTangentSpaceCalculation<Md5MeshSurfaceProxy,true> tangentCalculator;

	Md5MeshSurfaceProxy input( verts, indices );

	tangentCalculator.CalculateTangentSpace( input );

	std::vector<int> vertHashes[256];
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

			vertinfo oldMv = verts[ indices[i*3+j] ];

			ModelVertWeight mvw;
			ModelVert mv;
			MakeModelVert( mv, mvw, oldMv, normal, tangent, bitangent );

			newIndices.push_back( (unsigned short)AddModelVert( vertHashes, newVerts, newVertWeights, mv, mvw ) );
		}
	}
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

static void CalcW( Vectormath::Aos::Quat &q )
{
	q.setW( -sqrtf( 1.f - (q[0]*q[0]+q[1]*q[1]+q[2]*q[2]) ) );
}

static int TransMapping( tokeniser &t )
{
	token tok;
	tokGetToken( tok, t.cursor, t.singleCharList );

	int map = 0;
	std::string str;
	tokGetString( str, tok );

	if ( str == "tx" )
	{
		map = kTransX;
	} else
	if ( str == "ty" )
	{
		map = kTransY;
	} else
	if ( str == "tz" )
	{
		map = kTransZ;
	} else
	if ( str == "-tx" )
	{
		map = kNegTransX;
	} else
	if ( str == "-ty" )
	{
		map = kNegTransY;
	} else
	if ( str == "-tz" )
	{
		map = kNegTransZ;
	}

	return map;
}

static int RotMapping( tokeniser &t )
{
	token tok;
	tokGetToken( tok, t.cursor, t.singleCharList );

	int map = 0;
	std::string str;
	tokGetString( str, tok );

	if ( str == "qx" )
	{
		map = kQuatX;
	} else
	if ( str == "qy" )
	{
		map = kQuatY;
	} else
	if ( str == "qz" )
	{
		map = kQuatZ;
	} else
	if ( str == "qw" )
	{
		map = kQuatW;
	} else
	if ( str == "-qx" )
	{
		map = kNegQuatX;
	} else
	if ( str == "-qy" )
	{
		map = kNegQuatY;
	} else
	if ( str == "-qz" )
	{
		map = kNegQuatZ;
	} else
	if ( str == "-qw" )
	{
		map = kNegQuatW;
	}

	return map;
}

static int findJoint( std::vector< modeljoint > &joints, const char *name )
{
	for ( unsigned int i=0; i<joints.size(); i++)
	{
		if ( joints[i].name == name )
		{
			return i;
		}
	}
	return -1;
}

static int sign( float f )
{
	return f < 0 ? -1 : 1;
}

static int guessMapping( Vectormath::Aos::Point3 &p0, Vectormath::Aos::Point3 &p1, int idx )
{
	int closest = idx;
	int   negate = sign( p0[idx] ) != sign( p1[idx] );
	float diff = fabsf( fabsf( p0[idx] ) - fabsf( p1[idx] ) );
	for (int i=1; i<3; i++)
	{
		int newidx = (idx+i) % 3;
		float testdiff = fabsf( fabsf( p0[idx] ) - fabsf( p1[newidx] ) );
		if ( testdiff < diff )
		{
			diff = testdiff;
			closest = newidx;
			negate = sign( p0[idx] ) != sign( p1[newidx] );
		}
	}
	return negate ? closest + kNegate : closest;
}

static int guessMapping( Vectormath::Aos::Quat &p0, Vectormath::Aos::Quat &p1, int idx )
{
	int closest = idx;
	int   negate = sign( p0[idx] ) != sign( p1[idx] );
	float diff = fabsf( fabsf( p0[idx] ) - fabsf( p1[idx] ) );
	for (int i=1; i<4; i++)
	{
		int newidx = (idx+i) % 4;
		float testdiff = fabsf( fabsf( p0[idx] ) - fabsf( p1[newidx] ) );
		if ( testdiff < diff )
		{
			diff = testdiff;
			closest = newidx;
			negate = sign( p0[idx] ) != sign( p1[newidx] );
		}
	}
	return negate ? closest + kNegate : closest;
}

static 	void guessMapping( std::vector< modeljoint > &joints, int jointS, int jointT, int trans[3], int q[4] )
{
	modeljoint &jS = joints[ jointS ];
	modeljoint &jT = joints[ jointT ];

	trans[0] = guessMapping( jS.relT, jT.relT, 0 );
	trans[1] = guessMapping( jS.relT, jT.relT, 1 );
	trans[2] = guessMapping( jS.relT, jT.relT, 2 );

	q[0] = guessMapping( jS.relQ, jT.relQ, 0 );
	q[1] = guessMapping( jS.relQ, jT.relQ, 1 );
	q[2] = guessMapping( jS.relQ, jT.relQ, 2 );
	q[3] = guessMapping( jS.relQ, jT.relQ, 3 );
}


static void ParseMD5Mirror( PHYSFS_File *f, std::vector< modeljoint > &joints ) 
{
	char line[1024];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		tokeniser t;
		t.cursor = line;
		t.singleCharList = "/\\";

		token tok;
		if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
		{
			if ( tokTest( tok, "}" ) )
			{
				break;
			}

			std::string source, target;
			tokGetString( source, tok );
			target = tokGetTokenString( t.cursor, t.singleCharList );

			int jointS = findJoint( joints, source.c_str() );
			int jointT = findJoint( joints, target.c_str() );

			modeljoint &jS = joints[jointS];
			modeljoint &jT = joints[jointT];

			int trans[3];
			int q[4];

			guessMapping( joints, jointS, jointT, trans, q );

			bool explicitMapping = tokExpect( t.cursor, t.singleCharList, "{" );
			if ( explicitMapping )
			{
				trans[0] = TransMapping( t );
				trans[1] = TransMapping( t );
				trans[2] = TransMapping( t );
				tokExpect( t.cursor, t.singleCharList, "}" );

				tokExpect( t.cursor, t.singleCharList, "{" );
				q[0] = RotMapping( t );
				q[1] = RotMapping( t );
				q[2] = RotMapping( t );
				q[3] = RotMapping( t );
				tokExpect( t.cursor, t.singleCharList, "}" );
			} else
			{
				guessMapping( joints, jointS, jointT, trans, q );
			}

			if ( jointS != -1 && jointT != -1 )
			{
				joints[jointS].mirror = target;
				joints[jointS].mirrorFunc = ANIM_MIRROR( q[0], q[1], q[2], q[3], trans[0], trans[1], trans[2] );

				joints[jointT].mirror = source;
				joints[jointT].mirrorFunc = ANIM_MIRROR( q[0], q[1], q[2], q[3], trans[0], trans[1], trans[2] );
			}
		}
	}
}

static void ParseMD5Joints( PHYSFS_File *f, std::vector< modeljoint > &joints, Edge::Tools::Skeleton &edgeSkel ) 
{
	Vectormath::Aos::Transform3 fixup = Vectormath::Aos::Transform3::identity();
	fixup[0][0] = 1.f;
	fixup[1][1] = 0.f;
	fixup[1][2] = -1.f;
	fixup[2][1] = 1.f;
	fixup[2][2] = 0.f;
	fixup[3][3] = 1.f;
	char line[1024];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		tokeniser t;
		t.cursor = line;
		t.singleCharList = "/\\";

		token tok;
		if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
		{
			if ( tokTest( tok, "}" ) )
			{
				break;
			}

			modeljoint j;
			tokGetString( j.name, tok );
			j.parent = tokGetTokenInt( t.cursor, t.singleCharList );
			
			Vectormath::Aos::Vector3 trans;
			Vectormath::Aos::Quat q;

			tokExpect( t.cursor, t.singleCharList, "(" );
			trans[0] = tokGetTokenFloat( t.cursor, t.singleCharList ) * 1.f/32.f;
			trans[1] = tokGetTokenFloat( t.cursor, t.singleCharList ) * 1.f/32.f;
			trans[2] = tokGetTokenFloat( t.cursor, t.singleCharList ) * 1.f/32.f;
			tokExpect( t.cursor, t.singleCharList, ")" );

			tokExpect( t.cursor, t.singleCharList, "(" );
			q[0] = ( tokGetTokenFloat( t.cursor, t.singleCharList ) );
			q[1] = ( tokGetTokenFloat( t.cursor, t.singleCharList ) );
			q[2] = ( tokGetTokenFloat( t.cursor, t.singleCharList ) );
			CalcW( q );
			tokExpect( t.cursor, t.singleCharList, ")" );

			j.m = fixup * Vectormath::Aos::Transform3( q, trans );
			j.im = Vectormath::Aos::inverse( j.m );

			if ( j.parent != -1 )
			{
				Vectormath::Aos::Transform3 relM;
				relM = joints[j.parent].im * j.m;
				j.relQ = Vectormath::Aos::Quat( relM.getUpper3x3() );
				j.relT = Vectormath::Aos::Point3( relM.getTranslation() );
			} else
			{
				j.relQ = Vectormath::Aos::Quat( j.m.getUpper3x3() );
				j.relT = Vectormath::Aos::Point3( j.m.getTranslation() );
			}
			j.mirror = j.name;
			j.mirrorFunc = kNotMirrored;

			edgeSkel.m_parentIndices.push_back( j.parent );
			edgeSkel.m_jointNameHashes.push_back( edgeAnimGenerateNameHash( j.name.c_str() ) );
			Edge::Tools::Joint edgeJoint;
			edgeJoint.m_rotation[0] = j.relQ[0];
			edgeJoint.m_rotation[1] = j.relQ[1];
			edgeJoint.m_rotation[2] = j.relQ[2];
			edgeJoint.m_rotation[3] = j.relQ[3];
			edgeJoint.m_translation[0] = j.relT[0];
			edgeJoint.m_translation[1] = j.relT[1];
			edgeJoint.m_translation[2] = j.relT[2];
			edgeJoint.m_translation[3] = 1.f;
			edgeJoint.m_scale[0] = 1.f;
			edgeJoint.m_scale[1] = 1.f;
			edgeJoint.m_scale[2] = 1.f;
			edgeJoint.m_scale[3] = 1.f;
			edgeSkel.m_basePose.push_back( edgeJoint );
			joints.push_back( j );
		}
	}
}


static void ParseMD5Mesh( std::vector< ModelGeometry * > &geom, std::vector< std::string > &materials, std::vector< std::string > &surfaceNames, PHYSFS_File *f, const std::vector< modeljoint > &joints )
{
	std::vector< vertinfo > verts;
	std::vector< unsigned short > tris;
	std::vector< weightinfo > weights;

	char line[1024];
	while ( PHYSFS_gets( line, sizeof(line), f ) )
	{
		tokeniser t;
		t.cursor = line;
		t.singleCharList = "/\\";

		token tok;
		if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
		{
			if ( tokTest( tok, "}" ) )
			{
				break;
			} else
			if ( tokTest( tok, "name" ) ) 
			{
				surfaceNames.push_back( tokGetTokenString( t.cursor, t.singleCharList ) );
			} else
			if ( tokTest( tok, "shader" ) ) 
			{
				materials.push_back( tokGetTokenString( t.cursor, t.singleCharList ) );
			} else
			if ( tokTest( tok, "flags" ) ) 
			{
				while ( PHYSFS_gets( line, sizeof(line), f ) )
				{
					t.cursor = line;
					if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
					{
						if ( tokTest( tok, "}" ) )
						{
							break;
						}
					}
				}
			} else
			if ( tokTest( tok, "vert" ) ) 
			{
				vertinfo v;
				tokGetTokenInt( t.cursor, t.singleCharList );

				tokExpect( t.cursor, t.singleCharList, "(" );
				v.uv.x = tokGetTokenFloat( t.cursor, t.singleCharList );
				v.uv.y = tokGetTokenFloat( t.cursor, t.singleCharList );
				tokExpect( t.cursor, t.singleCharList, ")" );

				v.w = tokGetTokenInt( t.cursor, t.singleCharList );
				v.numw = tokGetTokenInt( t.cursor, t.singleCharList );

				verts.push_back( v );
			} else
			if ( tokTest( tok, "tri" ) ) {
				tokGetTokenInt( t.cursor, t.singleCharList );

				int v0 = tokGetTokenInt( t.cursor, t.singleCharList );
				int v1 = tokGetTokenInt( t.cursor, t.singleCharList );
				int v2 = tokGetTokenInt( t.cursor, t.singleCharList );

				tris.push_back( v0 );
				tris.push_back( v1 );
				tris.push_back( v2 );
			} else
			if ( tokTest( tok, "weight" ) ) {
				weightinfo w;

				tokGetTokenInt( t.cursor, t.singleCharList );
				w.joint = tokGetTokenInt( t.cursor, t.singleCharList );
				w.w = tokGetTokenFloat( t.cursor, t.singleCharList );
			
				tokExpect( t.cursor, t.singleCharList, "(" );
				w.ofs[0] = tokGetTokenFloat( t.cursor, t.singleCharList ) * 1.f/32.f;
				w.ofs[1] = tokGetTokenFloat( t.cursor, t.singleCharList ) * 1.f/32.f;
				w.ofs[2] = tokGetTokenFloat( t.cursor, t.singleCharList ) * 1.f/32.f;
				tokExpect( t.cursor, t.singleCharList, ")" );

				weights.push_back( w );
			}
		}
	}

	if ( verts.size() == 0 || tris.size() == 0 )
	{
		return;
	}



	for (uint32_t i=0; i<verts.size(); i++) {
		vertinfo &v = verts[i];
		Vectormath::Aos::Vector3 final(0.f,0.f,0.f);
		for (int j=0; j<v.numw; j++) {
			weightinfo &w = weights[v.w+j];
			final += Vectormath::Aos::Vector3( joints[w.joint].m * Vectormath::Aos::Point3(w.ofs) ) * w.w;
			weight vw;
			vw.idx = w.joint;
			vw.w = w.w;
			v.weights.push_back(vw);
		}
		v.final = final;
		std::sort( v.weights.begin(), v.weights.end() );
		if ( v.weights.size() != 4 )
		{
			uint32_t os = v.weights.size();
			v.weights.resize( 4 );
			for (uint32_t j=os; j<v.weights.size(); j++)
			{
				v.weights[j] = v.weights[0];
				v.weights[j].w = 0.f;
				v.weights[j].bw = 0;
			}
		}

		float totw = 0.f;
		for (uint32_t j=0; j<v.weights.size(); j++) {
			totw += v.weights[j].w;
		}
		for (uint32_t j=0; j<v.weights.size(); j++) {
			v.weights[j].w /= totw;
		}
		for (uint32_t j=0; j<v.weights.size(); j++) {
			v.weights[j].bw = (int)((255.f*v.weights[j].w) + 0.5f);
		}
		int totbw = 0;
		for (uint32_t j=0; j<v.weights.size(); j++) {
			totbw += v.weights[j].bw;
		}
		v.weights[0].bw += (255-totbw);
	}

	std::vector< ModelVert > newVerts;
	std::vector< ModelVertWeight > newVertWeights;
	FinishSurface( newVerts, newVertWeights, verts, tris );

	ModelGeometry *outgeom = new ModelGeometry;
	outgeom->m_indices = copyAndAlloc( tris, outgeom->m_numIndices );
	outgeom->m_verts = copyAndAlloc( newVerts, outgeom->m_numVerts );
	
	int outCount;
	outgeom->m_weights = copyAndAlloc( newVertWeights, outCount );

	geom.push_back( outgeom );
}


void LoadMD5Mesh( std::vector< ModelGeometry * > &geom, std::vector< std::string > &materials, std::vector< std::string > &surfaceNames, std::vector< modeljoint > &joints, const char *filename )
{
	PHYSFS_File *f = PHYSFS_openRead( filename );
	if ( f )
	{
		joints.clear();
		Edge::Tools::Skeleton edgeSkel;
		edgeSkel.m_numJoints = 0;
		edgeSkel.m_locoJointIndex = 0;
		edgeSkel.m_numUserChannels = 0;

		int numJoints = 0;
		int numMeshes = 0;
		char line[1024];
		while ( PHYSFS_gets( line, sizeof(line), f ) )
		{
			tokeniser t;
			t.cursor = line;
			t.singleCharList = "/\\";

			token tok;
			if ( tokGetToken( tok, t.cursor, t.singleCharList ) )
			{
				if ( tokTest( tok, "MD5Version" ) )
				{
					continue;
				} else
				if ( tokTest( tok, "commandline" ) )
				{
					continue;
				} else
				if ( tokTest( tok, "numJoints" ) )
				{
					numJoints = tokGetTokenInt( t.cursor, t.singleCharList );
					edgeSkel.m_numJoints = numJoints;
				} else
				if ( tokTest( tok, "numMeshes" ) )
				{
					numMeshes = tokGetTokenInt( t.cursor, t.singleCharList );
				} else
				if ( tokTest( tok, "joints" ) )
				{
					joints.reserve( numJoints );
					ParseMD5Joints( f, joints, edgeSkel );
				} else
				if ( tokTest( tok, "mirror" ) )
				{
					ParseMD5Mirror( f, joints );
				} else
				if ( tokTest( tok, "mesh" ) )
				{
					ParseMD5Mesh( geom, materials, surfaceNames, f, joints );
				}
			}
		}
		PHYSFS_close(f);
		char outputName[256];
		sprintf( outputName, "generated\\%s.edgeskel", filename );
		Edge::Tools::ExportSkeleton( outputName, edgeSkel, 4, false );
	}
}
