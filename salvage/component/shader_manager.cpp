#include "shader_manager.h"

Shader* ShaderManager::CreateEmpty( const char *filename )
{
	return new Shader;
}

Shader* ShaderManager::LoadResource( const char *filename )
{
	Shader *s = new Shader;
	s->Load( filename );
	return s;
}

ShaderManager::ShaderManager()
{
}

ShaderManager *shaderManager()
{
	static ShaderManager mgr;
	return &mgr;
}

