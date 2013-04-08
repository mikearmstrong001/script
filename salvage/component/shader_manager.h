#ifndef TESTING_SHADER_MANAGER_H
#define TESTING_SHADER_MANAGER_H

#include "shader.h"

#include "resource_core.h"

class ShaderManager : public TypedResourceManager<Shader>
{
	virtual Shader* CreateEmpty( const char *filename );
	virtual Shader* LoadResource( const char *filename );

public:
	ShaderManager();
};

ShaderManager *shaderManager();

#endif

