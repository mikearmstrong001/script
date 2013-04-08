#include "dxut.h"
#include "lightmapbinding_manager.h"


LightMapBinding* LightMapBindingManager::CreateEmpty( const char *filename )
{
	return new LightMapBinding;
}

LightMapBinding* LightMapBindingManager::LoadResource( const char *filename )
{
	LightMapBinding *lmb = new LightMapBinding;
	lmb->Load( filename );
	return lmb;
}

LightMapBindingManager::LightMapBindingManager()
{
}


LightMapBindingManager *lightMapBindingManager()
{
	static LightMapBindingManager mgr;
	return &mgr;
}
