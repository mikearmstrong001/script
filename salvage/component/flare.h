#ifndef TESTING_FLARE_H
#define TESTING_FLARE_H

#include <vector>
#include "resource_core.h"

struct FlareItem
{
	float uv[4];
	float factor;
	float scale;
};

class Flare : public Resource
{
	std::vector<FlareItem> items;


public:

	Flare();
	~Flare();

	virtual void Purge();

	void Load( const char *filename );

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}

	int NumItems() const
	{
		return items.size();
	}

	const FlareItem &Item( int v ) const
	{
		return items[v];
	}
};

class FlareManager : public TypedResourceManager<Flare>
{
	virtual Flare* CreateEmpty( const char *filename );
	virtual Flare* LoadResource( const char *filename );

public:
	FlareManager();
};

FlareManager *flareManager();

#endif
