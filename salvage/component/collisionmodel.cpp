#include "dxut.h"
#include "collisionmodel.h"

#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btConvexPointCloudShape.h"
#include "loader_obj.h"
#include "loader_atgi.h"
#include "LinearMath/btConvexHull.h"
#include "btBulletWorldImporter.h"

#include "faff.h"
#include "file_system.h"

CollisionModel::CollisionModel() : importer(NULL), convexHullShape(NULL), hullVerts(NULL), shapeArrays(NULL), shape(NULL), verts(NULL), indices(NULL), bulletFile(NULL)
{
}

CollisionModel::~CollisionModel()
{
	Purge();
}

void CollisionModel::Purge()
{
	if ( importer )
	{
		importer->deleteAllData();
		shape = NULL;
	}
	delete convexHullShape;
	delete hullVerts;
	delete shapeArrays;
	delete shape;
	delete verts;
	delete indices;
	delete importer;
	_aligned_free( bulletFile );

	convexHullShape = NULL;
	hullVerts = NULL;
	shapeArrays = NULL;
	shape = NULL;
	verts = NULL;
	indices = NULL;
	bulletFile = NULL;
	importer = NULL;

	SetLoaded( false );
	SetDefaulted( false );
}

void CollisionModel::LoadObj( const char *filename )
{
	uint64_t hashedName = HashedString( 0, filename );
	char outfile[256];
	sprintf( outfile, "collision_models/%08x_%08x.cm", (uint32_t)(hashedName>>32), (uint32_t)(hashedName&0xffffffff) );

	PHYSFS_sint64 len = PHYSFS_readFileAligned( outfile, &bulletFile, 16 );
	if ( len > 0 )
	{
		importer = new btBulletWorldImporter(0);//don't store info into the world
		if (importer->loadFileFromMemory( (char*)bulletFile, len ))
		{
			shape = (btBvhTriangleMeshShape*)importer->getCollisionShapeByIndex( 0 );
			return;
		}
	}

	std::vector< ModelGeometry* > geoms;
	std::vector< std::string > materials;
	std::vector< std::string > surfaceNames;
	::LoadObj( geoms, materials, surfaceNames, filename );

	int totIndices = 0;
	totVerts = 0;
	for (unsigned int i=0; i<geoms.size(); i++)
	{
		totIndices += geoms[i]->m_numIndices;
		totVerts += geoms[i]->m_numVerts;
	}

	verts = new btVector3[ totVerts ];
	indices = new int[ totIndices ];

	btVector3 aabbMin(10000,10000,10000),aabbMax(-10000,-10000,-10000);

	int pv = 0;
	int pi = 0;
	for (unsigned int i=0; i<geoms.size(); i++)
	{
		int base = pv;
		for (int j=0; j<geoms[i]->m_numVerts; j++)
		{
			float *p = geoms[i]->m_verts[j].p;
			verts[pv] = btVector3( p[0], p[1], p[2] );
			aabbMax.setMax( verts[pv] );
			aabbMin.setMin( verts[pv] );
			pv++;
		}
		for (int j=0; j<geoms[i]->m_numIndices; j++)
		{
			indices[pi++] = base+geoms[i]->m_indices[j];
			assert( indices[pi-1] >= base );
			assert( indices[pi-1] < pv );
		}
		delete geoms[i]->m_indices;
		delete geoms[i]->m_verts;
		delete geoms[i];
	}

	shapeArrays = new btTriangleIndexVertexArray(totIndices/3,
		indices,
		sizeof(int)*3,
		totVerts,(btScalar*) &verts[0].x(),sizeof(btVector3));

	bool useQuantizedAabbCompression = true;

	shape = new btBvhTriangleMeshShape(shapeArrays,useQuantizedAabbCompression,aabbMin,aabbMax);

	int maxSerializeBufferSize = 1024*1024*5;
	btDefaultSerializer*	serializer = new btDefaultSerializer(maxSerializeBufferSize);
	//serializer->setSerializationFlags(BT_SERIALIZE_NO_BVH);//	or BT_SERIALIZE_NO_TRIANGLEINFOMAP
	serializer->startSerialization();
	shape->serializeSingleShape( serializer );
	serializer->finishSerialization();

	PHYSFS_mkdir( "collision_models/" );
	PHYSFS_File *model = PHYSFS_openWrite( outfile );
	if ( model )
	{
		PHYSFS_write(model, serializer->getBufferPointer(),serializer->getCurrentBufferSize(),1);
		PHYSFS_close(model);
	}
	delete serializer;
}

void CollisionModel::LoadAtgi( const char *filename )
{
	std::vector< ModelGeometry* > geoms;
	std::vector< std::string > materials;
	std::vector< std::string > surfaceNames;
	::LoadAtgi( geoms, materials, surfaceNames, filename );

	int totIndices = 0;
	totVerts = 0;
	for (unsigned int i=0; i<geoms.size(); i++)
	{
		totIndices += geoms[i]->m_numIndices;
		totVerts += geoms[i]->m_numVerts;
	}

	verts = new btVector3[ totVerts ];
	indices = new int[ totIndices ];

	btVector3 aabbMin(10000,10000,10000),aabbMax(-10000,-10000,-10000);

	int pv = 0;
	int pi = 0;
	for (unsigned int i=0; i<geoms.size(); i++)
	{
		int base = pv;
		for (int j=0; j<geoms[i]->m_numVerts; j++)
		{
			float *p = geoms[i]->m_verts[j].p;
			verts[pv] = btVector3( p[0], p[1], p[2] );
			aabbMax.setMax( verts[pv] );
			aabbMin.setMin( verts[pv] );
			pv++;
		}
		for (int j=0; j<geoms[i]->m_numIndices; j++)
		{
			indices[pi++] = base+geoms[i]->m_indices[j];
			assert( indices[pi-1] >= base );
			assert( indices[pi-1] < pv );
		}
		delete geoms[i]->m_indices;
		delete geoms[i]->m_verts;
		delete geoms[i];
	}

	shapeArrays = new btTriangleIndexVertexArray(totIndices/3,
		indices,
		sizeof(int)*3,
		totVerts,(btScalar*) &verts[0].x(),sizeof(btVector3));

	bool useQuantizedAabbCompression = true;

	shape = new btBvhTriangleMeshShape(shapeArrays,useQuantizedAabbCompression,aabbMin,aabbMax);
}

void CollisionModel::GenerateHull()
{
	HullDesc desc;
	HullResult result;

	const btStridingMeshInterface* mi = shape->getMeshInterface();
	const unsigned char *vertexbase = NULL;
	int numverts;
	PHY_ScalarType type;
	int vertexStride;
	const unsigned char *indexbase;
	int indexstride;
	int numfaces;
	PHY_ScalarType indicestype;
	mi->getLockedReadOnlyVertexIndexBase(&vertexbase, numverts,type, vertexStride,&indexbase,indexstride,numfaces,indicestype);

	desc.mVertices = (btVector3*)vertexbase;//verts;
	desc.mVcount = numverts;//totVerts;
	desc.mVertexStride = vertexStride;

	HullLibrary hull;
	hull.CreateConvexHull( desc, result );

	totHullVerts = result.mNumOutputVertices;
	hullVerts = new btVector3[totHullVerts];
	memcpy( hullVerts, &result.m_OutputVertices[0], sizeof(btVector3)*totHullVerts );

	hull.ReleaseResult( result );
	convexHullShape = new btConvexPointCloudShape( hullVerts, totHullVerts, btVector3(1.f,1.f,1.f) );

	mi->unLockReadOnlyVertexBase(0);
}

