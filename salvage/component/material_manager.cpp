#include "material_manager.h"


Material* MaterialManager::CreateEmpty( const char *filename )
{
	return new Material;
}

Material* MaterialManager::LoadResource( const char *filename )
{
	Material *mtr = new Material;
	mtr->Load( filename );
	return mtr;
}

MaterialManager::MaterialManager()
{
}


MaterialManager *materialManager()
{
	static MaterialManager mgr;
	return &mgr;
}
