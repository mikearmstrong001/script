#ifndef TESTING_TECHNIQUE_H
#define TESTING_TECHNIQUE_H

#include <vector>
#include "resource_core.h"


class Shader;

struct TechniquePass
{
	int pass;
	Shader *shader;
	int blendSrc;
	int blendDst;

	TechniquePass() : pass(-1), shader(NULL), blendSrc(-1), blendDst(-1) {}

	void Bind() const;
};

class Technique : public Resource
{
	std::vector<TechniquePass> passes;


public:

	Technique();
	~Technique();

	virtual void Purge();

	void Load( const char *filename );

	int NumPasses() const { return passes.size(); }
	const TechniquePass *GetPass(int p) const
	{
		return &passes[p];
	}

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}
};

#endif
