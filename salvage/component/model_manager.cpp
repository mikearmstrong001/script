#include "model_manager.h"
#include "model_static.h"
#include "model_jointed.h"


Model* ModelManager::CreateEmpty( const char * )
{
	return new ModelStatic;
}

Model* ModelManager::LoadResource( const char *filename )
{
	if ( strstr( filename, ".obj" ) )
	{
		ModelStatic *mdl = new ModelStatic;
		mdl->LoadObj( filename );
		return mdl;
	} else
	if ( strstr( filename, ".atgi" ) )
	{
		ModelStatic *mdl = new ModelStatic;
		mdl->LoadAtgi( filename );
		return mdl;
	} else
	if ( strstr( filename, ".md5mesh" ) )
	{
		ModelJointed *mdl = new ModelJointed;
		mdl->LoadMd5Mesh( filename );
		return mdl;
	}
	return CreateEmpty( filename );
}

ModelManager::ModelManager()
{
}


ModelManager *modelManager()
{
	static ModelManager mgr;
	return &mgr;
}
