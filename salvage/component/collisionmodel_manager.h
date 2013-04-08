#ifndef TESTING_COLLISIONMODEL_MANAGER_H
#define TESTING_COLLISIONMODEL_MANAGER_H

#include "resource_core.h"
#include "collisionmodel.h"

class CollisionModelManager : public TypedResourceManager<CollisionModel>
{
	virtual CollisionModel* CreateEmpty( const char *filename );
	virtual CollisionModel* LoadResource( const char *filename );

public:
	CollisionModelManager();
};

CollisionModelManager *collisionModelManager();


#endif
