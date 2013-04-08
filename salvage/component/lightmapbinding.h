#ifndef TESTING_LIGHTMAPBINDING_H
#define TESTING_LIGHTMAPBINDING_H

#include "resource_core.h"
#include <string>
#include <vector>
#include <map>

class Texture;

struct LightmapBind
{
	int bindIndex;
	std::string bind;
	const Texture *tex;
};

struct MeshLightmapBindSet
{
	std::vector<LightmapBind> binds;

	void Bind() const;
};

class LightMapBinding : public Resource
{
	std::map<std::string,MeshLightmapBindSet> bindings;
public:

	LightMapBinding();
	~LightMapBinding();

	virtual void Purge();

	void Load( const char *filename );

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}

	const MeshLightmapBindSet *FindBind( const char *name );
};

#endif
