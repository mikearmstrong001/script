#ifndef TESTING_ANIMSET_H
#define TESTING_ANIMSET_H

#include "resource_core.h"
#include <map>
#include <string>

class Anim;

class AnimSet : public Resource
{
	std::map< std::string, const Anim * > animMap;

public:

	AnimSet();
	~AnimSet();

	virtual void Purge();

	void Load( const char *filename );

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}

	const Anim *Find( const char *a ) const
	{
		std::map< std::string, const Anim * >::const_iterator f = animMap.find( a );
		if ( f != animMap.end() )
		{
			return f->second;
		} else
		{
			return NULL;
		}
	}
};

class AnimSetManager : public TypedResourceManager<AnimSet>
{
	virtual AnimSet* CreateEmpty( const char *filename );
	virtual AnimSet* LoadResource( const char *filename );

public:
	AnimSetManager();
};

AnimSetManager *animSetManager();

#endif
