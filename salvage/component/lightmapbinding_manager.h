#ifndef TESTING_LIGHTMAPBINDING_MANAGER_H
#define TESTING_LIGHTMAPBINDING_MANAGER_H

#include "lightmapbinding.h"

#include "resource_core.h"

class LightMapBindingManager : public TypedResourceManager<LightMapBinding>
{
	virtual LightMapBinding* CreateEmpty( const char *filename );
	virtual LightMapBinding* LoadResource( const char *filename );

public:
	LightMapBindingManager();
};

LightMapBindingManager *lightMapBindingManager();

#endif

