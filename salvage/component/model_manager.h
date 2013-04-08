#ifndef TESTING_MODEL_MANAGER_H
#define TESTING_MODEL_MANAGER_H

#include "model.h"
#include "resource_core.h"

class ModelManager : public TypedResourceManager<Model>
{
	virtual Model* CreateEmpty( const char *filename );
	virtual Model* LoadResource( const char *filename );

public:
	ModelManager();
};

ModelManager *modelManager();


#endif
