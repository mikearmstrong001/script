#ifndef TESTING_MATERIAL_MANAGER_H
#define TESTING_MATERIAL_MANAGER_H

#include "material.h"

#include "resource_core.h"

class MaterialManager : public TypedResourceManager<Material>
{
	virtual Material* CreateEmpty( const char *filename );
	virtual Material* LoadResource( const char *filename );

public:
	MaterialManager();
};

MaterialManager *materialManager();

#endif

