#ifndef TESTING_NAVMESH_H
#define TESTING_NAVMESH_H

#include <vector>
#include "resource_core.h"

class NavMesh : public Resource
{

public:

	NavMesh();
	~NavMesh();

	virtual void Purge();

	void Load( const char *filename );

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}

};

class NavMeshManager : public TypedResourceManager<NavMesh>
{
	virtual NavMesh* CreateEmpty( const char *filename );
	virtual NavMesh* LoadResource( const char *filename );

public:
	NavMeshManager();
};

NavMeshManager *navMeshManager();

#endif
