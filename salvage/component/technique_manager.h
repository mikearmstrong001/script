#ifndef TESTING_TECHNIQUE_MANAGER_H
#define TESTING_TECHNIQUE_MANAGER_H

#include "technique.h"
#include "resource_core.h"

class TechniqueManager : public TypedResourceManager<Technique>
{
	virtual Technique* CreateEmpty( const char *filename );
	virtual Technique* LoadResource( const char *filename );

public:
	TechniqueManager();
};

TechniqueManager *techniqueManager();

#endif

