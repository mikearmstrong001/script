#ifndef TESTING_MATERIAL_H
#define TESTING_MATERIAL_H

#include "resource_core.h"

class Texture;
class Technique;

class Material : public Resource
{
	int blendSrc;
	int blendDst;
	int diffuseSlot;
	int normalSlot;
	Texture *diffuse;
	Texture *normal;
	Technique *technique;
	bool depthWrite;
	bool copyScreen;

public:

	Material();
	~Material();

	virtual void Purge();

	void Load( const char *filename );

	void Bind() const;

	Technique *GetTechnique() const
	{
		return technique;
	}

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}


};

#endif

