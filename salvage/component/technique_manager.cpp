#include "dxut.h"
#include "technique_manager.h"


Technique* TechniqueManager::CreateEmpty( const char * )
{
	return NULL;
}

Technique* TechniqueManager::LoadResource( const char *filename )
{
	Technique *tech = new Technique;
	tech->Load( filename );
	return tech;
}

TechniqueManager::TechniqueManager()
{
}


TechniqueManager *techniqueManager()
{
	static TechniqueManager mgr;
	return &mgr;
}
