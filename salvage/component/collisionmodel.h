#ifndef TESTING_COLLISIONMODEL_H
#define TESTING_COLLISIONMODEL_H

#include "resource_core.h"
#include "collisionmodel.h"

class btTriangleIndexVertexArray;
class btBvhTriangleMeshShape;
class btConvexPointCloudShape;
class btVector3;
class btBulletWorldImporter;

class CollisionModel : public Resource
{
public:

	CollisionModel();
	~CollisionModel();

	virtual void Purge();

	void LoadObj( const char *filename );
	void LoadAtgi( const char *filename );

	void GenerateHull();

	virtual void Reload()
	{
		Purge();
		LoadObj( Name() );
	}

	btBulletWorldImporter *importer;

	btConvexPointCloudShape *convexHullShape;
	int totHullVerts;
	btVector3* hullVerts;
	
	btTriangleIndexVertexArray *shapeArrays;
	btBvhTriangleMeshShape *shape;
	int totVerts;
	btVector3* verts;
	int *indices;

	void *bulletFile;
};


#endif

