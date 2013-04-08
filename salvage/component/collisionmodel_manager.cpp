#include "dxut.h"
#include "collisionmodel_manager.h"

CollisionModel* CollisionModelManager::CreateEmpty( const char *filename )
{
	return new CollisionModel();
}

CollisionModel* CollisionModelManager::LoadResource( const char *filename )
{
	if ( strstr( filename, ".obj" ) )
	{
		CollisionModel *mdl = new CollisionModel;
		mdl->LoadObj( filename );
		return mdl;
	} else
	if ( strstr( filename, ".atgi" ) )
	{
		CollisionModel *mdl = new CollisionModel;
		mdl->LoadAtgi( filename );
		return mdl;
	}
	return CreateEmpty( filename );
}

CollisionModelManager::CollisionModelManager()
{
}

CollisionModelManager *collisionModelManager()
{
	static CollisionModelManager mgr;
	return &mgr;
}
