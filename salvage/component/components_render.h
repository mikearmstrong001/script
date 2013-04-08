#ifndef TESTING_COMPONENTS_RENDER_H
#define TESTING_COMPONENTS_RENDER_H

#include "components_basic.h"

class Flare;
class Material;
class Model;
class LightMapBinding;

class CFlare : public Component
{
	DECLAREINFO( CFlare, Component );
	const Flare *flare;

	void SetFlare( const char *value );

public:
	CFlare( const char *name, class GameObject *go );

	virtual void Bind( const char *tag, const char *data );

	void AddFlare( const Vectormath::Aos::Matrix4 mvp, struct ModelGeometry *geom );

	const Flare *GetFlare() const
	{
		return flare;
	}
};

class JointedMeshModel : public Renderable
{
	DECLAREINFO( JointedMeshModel, Renderable );

	Model *model;
	LightMapBinding *lightmapBinds;
	Vectormath::Aos::Transform3 *joints;

	void SetModel( const char *data );
	void SetLightMapBinds( const char *data );
public:
	JointedMeshModel( const char *name, class GameObject *go );
	~JointedMeshModel();

	virtual void Bind( const char *tag, const char *data );
	virtual int AddRenderCommands( struct RenderCommand *cmds );

	Model *GetModel() { return model; }
	Vectormath::Aos::Transform3 *GetJoints() { return joints; }
};

#endif
