#include "dxut.h"
#include "animset.h"
#include "anim.h"
#include "file_system.h"


AnimSet::AnimSet()
{
}

AnimSet::~AnimSet()
{
}

void AnimSet::Purge()
{
	animMap.clear();
}

void AnimSet::Load( const char *filename )
{
	Purge();

	char buffer[512];
	PHYSFS_File *f = PHYSFS_openRead( fs::MakeCanonicalForm( buffer, filename ) );
	if ( f )
	{
		char line[512];
		while ( PHYSFS_gets( line, sizeof(line), f ) )
		{
			char str0[256];
			char str1[256];
			if ( sscanf( line, "action %s %s", str0, str1 ) == 2 )
			{
				Anim *a = animManager()->Load( str1 );
				animMap[str0] = a;
			}
		}
	}
}

AnimSet* AnimSetManager::CreateEmpty( const char *filename )
{
	return NULL;
}

AnimSet* AnimSetManager::LoadResource( const char *filename )
{
	AnimSet *as = new AnimSet;
	as->Load( filename );
	return as;
}

AnimSetManager::AnimSetManager()
{
}

AnimSetManager *animSetManager()
{
	static AnimSetManager mgr;
	return &mgr;
}

