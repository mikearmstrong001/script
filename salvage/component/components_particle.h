#ifndef TESTING_COMPONENTS_PARTICLE_H
#define TESTING_COMPONENTS_PARTICLE_H

#include "components_core.h"
#include "components_basic.h"

class Model;
class Material;

struct Particle
{
	Vectormath::Aos::Point3 pos;
	Vectormath::Aos::Vector3 vel;
	Vectormath::Aos::Vector3 acc;
	Vectormath::Aos::Vector4 rgba;
	Vectormath::Aos::Vector4 sizeRotTime;
};

class ParticleEmitter : public Component
{
	DECLAREINFO( ParticleEmitter, Component );
protected:
	float m_rate;
	float m_rateAccumError;

	Particle *m_particles;

	int   m_maxParticles;
	int	  m_numParticles;

	virtual void Spawn( Particle *p, int num ) = 0;
public:
	ParticleEmitter( const char *name, GameObject *owner );
	~ParticleEmitter();

	virtual void Bind( const char *key, const char *value );
	virtual void Enter();
	virtual void Update( int idx );

	Particle *GetParticles( int &num ) { num = m_numParticles; return m_particles; }
};

class ParticleModelVertexEmitter : public ParticleEmitter
{
	DECLAREINFO( ParticleModelVertexEmitter, ParticleEmitter );

	Model *model;

	void SetModel( const char *data );

	virtual void Spawn( Particle *p, int num );

public:
	ParticleModelVertexEmitter( const char *name, GameObject *owner ) : PARENT(name,owner), model(NULL)  {}

	virtual void Bind( const char *key, const char *value );
};

class ParticleCuboidEmitter : public ParticleEmitter
{
	DECLAREINFO( ParticleCuboidEmitter, ParticleEmitter );

	Vectormath::Aos::Vector3 minBox;
	Vectormath::Aos::Vector3 maxBox;

	virtual void Spawn( Particle *p, int num );

public:
	ParticleCuboidEmitter( const char *name, GameObject *owner ) : PARENT(name,owner) {}

	virtual void Bind( const char *key, const char *value );
};

class ParticleEllipsoidEmitter : public ParticleEmitter
{
	DECLAREINFO( ParticleEllipsoidEmitter, ParticleEmitter );

	Vectormath::Aos::Vector3 dim;

	virtual void Spawn( Particle *p, int num );

public:
	ParticleEllipsoidEmitter( const char *name, GameObject *owner ) : PARENT(name,owner) {}

	virtual void Bind( const char *key, const char *value );
};

class ParticleAnimator : public Component
{
	DECLAREINFO( ParticleAnimator, Component );
public:
	ParticleAnimator( const char *name, GameObject *owner );

	virtual void Bind( const char *key, const char *value );
	virtual void Animate();
};

class ParticleRenderer : public Renderable
{
	DECLAREINFO( ParticleRenderer, Renderable );
public:
	ParticleRenderer( const char *name, GameObject *owner ) : PARENT(name,owner) {}

	virtual void Bind( const char *key, const char *value );
};

class ParticleBillboardRenderer : public ParticleRenderer
{
	DECLAREINFO( ParticleBillboardRenderer, ParticleRenderer );

	Material *material;

	void SetMaterial( const char *data );

public:
	ParticleBillboardRenderer( const char *name, GameObject *owner ) : material(NULL), PARENT(name,owner) {}

	virtual void Bind( const char *key, const char *value );

	virtual int AddRenderCommands( struct RenderCommand *cmds );
};

class ParticleModelRenderer : public ParticleRenderer
{
	DECLAREINFO( ParticleModelRenderer, ParticleRenderer );

	Model *model;

	void SetModel( const char *data );

public:
	ParticleModelRenderer( const char *name, GameObject *owner ) : model(NULL), PARENT(name,owner) {}

	virtual void Bind( const char *key, const char *value );

	virtual int AddRenderCommands( struct RenderCommand *cmds );
};

#endif
