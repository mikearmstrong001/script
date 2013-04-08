#include "dxut.h"
#include "components_particle.h"
#include "bind.h"
#include "model_manager.h"
#include "material_manager.h"
#include "faff.h"
#include "technique.h"

TYPE(ParticleModelVertexEmitter);
TYPE(ParticleCuboidEmitter);
TYPE(ParticleEllipsoidEmitter);
TYPE(ParticleAnimator);
TYPE(ParticleBillboardRenderer);
TYPE(ParticleModelRenderer);

static float randf()
{
	return rand()/(float)RAND_MAX;
}

ParticleEmitter::ParticleEmitter( const char *name, GameObject *owner ) : m_rate(1.f), m_rateAccumError(0.f), m_particles(NULL), 
	m_maxParticles(0), m_numParticles(0), PARENT(name,owner) 
{
	owner->RegisterForUpdate( this, eUPDATE );
}

ParticleEmitter::~ParticleEmitter()
{
	_aligned_free(m_particles);
}

void ParticleEmitter::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );

	::Bind( "rate", m_rate, key, value, "1" );
	::Bind( "maxParticle", m_maxParticles, key, value, "100" );
}

void ParticleEmitter::Enter()
{
	m_rateAccumError = 0.f;
	m_particles = (Particle*)_aligned_malloc( sizeof(Particle) * m_maxParticles, 16 );
	m_numParticles = 0;
}

void ParticleEmitter::Update( int /*idx*/ )
{
	Vectormath::Aos::Vector4 age( 0.f, 0.f, 0.f, 1.f/30.f );
	int i=0;
	while ( i<m_numParticles )
	{
		Particle *p = &m_particles[i];
		p->sizeRotTime -= age;
		if ( p->sizeRotTime.getW() < 0.f )
		{
			m_numParticles--;
			m_particles[i] = m_particles[m_numParticles];
		} else
		{
			i++;
		}
	}

	float perUpdate = m_rate / 30.f;
	m_rateAccumError += perUpdate;

	int toEmit = 0;
	while ( m_rateAccumError >= 1.f )
	{
		if ( m_numParticles < m_maxParticles )
		{
			toEmit++;
		}

		m_rateAccumError -= 1.f;
	}
	if ( toEmit )
	{
		Particle *p = &m_particles[ m_numParticles ];
		Spawn( p, toEmit );
		m_numParticles+=toEmit;
	}

	ParticleAnimator *animator = FindComponent<ParticleAnimator>();
	if ( animator )
	{
		animator->Animate();
	}
}

void ParticleModelVertexEmitter::SetModel( const char *data )
{
	model = modelManager()->Load( data );
}

void ParticleModelVertexEmitter::Spawn( Particle *p, int num )
{
	CTransform *t = FindComponent<CTransform>();
	Vectormath::Aos::Transform3 const &xform = t->world();
	for (int i=0; i<num; i++)
	{
		int r = rand() % model->NumSurfaces();
		const RenderSurface *surf = model->Surface( r );
		int v = rand() % surf->geom->m_numVerts;

		const ModelVert &mv = surf->geom->m_verts[v];
		p->pos = xform * Vectormath::Aos::Point3( mv.p[0], mv.p[1], mv.p[2] );
		p->vel = Vectormath::Aos::Vector3( 0.f, 10.f, 0.f );
		p->acc = Vectormath::Aos::Vector3( 0.f,-10.f, 0.f );
		p->rgba = Vectormath::Aos::Vector4( 1.f, 1.f, 1.f, 1.f );
		p->sizeRotTime = Vectormath::Aos::Vector4( 1.f, 1.f, 0.f, 10.f );
		p++;
	}
}

void ParticleModelVertexEmitter::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );
	::Bind< const char* >( "model", this, &ParticleModelVertexEmitter::SetModel, key, value );
}

void ParticleCuboidEmitter::Spawn( Particle *p, int num )
{
	CTransform *t = FindComponent<CTransform>();
	Vectormath::Aos::Transform3 const &xform = t->world();
	Vectormath::Aos::Vector3 dim = maxBox - minBox;
	for (int i=0; i<num; i++)
	{
		Vectormath::Aos::Vector3 random  = Vectormath::Aos::Vector3( randf(), randf(), randf() );
		p->pos = xform * Vectormath::Aos::Point3( Vectormath::Aos::mulPerElem( dim, random ) + minBox );
		p->vel = Vectormath::Aos::Vector3( 0.f, 10.f, 0.f );
		p->acc = Vectormath::Aos::Vector3( 0.f,-10.f, 0.f );
		p->rgba = Vectormath::Aos::Vector4( 1.f, 1.f, 1.f, 1.f );
		p->sizeRotTime = Vectormath::Aos::Vector4( 1.f, 1.f, 0.f, 10.f );
		p++;
	}
}

void ParticleCuboidEmitter::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );
	::Bind( "minBox", minBox, key, value, "-1 -1 -1" );
	::Bind( "maxBox", minBox, key, value, "1 1 1" );
}

void ParticleEllipsoidEmitter::Spawn( Particle *p, int num )
{
	CTransform *t = FindComponent<CTransform>();
	Vectormath::Aos::Transform3 const &xform = t->world();
	for (int i=0; i<num; i++)
	{
		Vectormath::Aos::Vector3 random  = Vectormath::Aos::mulPerElem( Vectormath::Aos::Vector3( randf(), randf(), randf() ), Vectormath::Aos::Vector3( 2.f, 2.f, 2.f ) ) - Vectormath::Aos::Vector3( 1.f, 1.f, 1.f );
		while ( Vectormath::Aos::lengthSqr( random ) < 1.f )
		{
			random  = Vectormath::Aos::mulPerElem( Vectormath::Aos::Vector3( randf(), randf(), randf() ), Vectormath::Aos::Vector3( 2.f, 2.f, 2.f ) ) - Vectormath::Aos::Vector3( 1.f, 1.f, 1.f );
		}
		p->pos = xform * Vectormath::Aos::Point3( Vectormath::Aos::mulPerElem( dim, random ) );
		p->vel = Vectormath::Aos::Vector3( 0.f, 10.f, 0.f );
		p->acc = Vectormath::Aos::Vector3( 0.f,-10.f, 0.f );
		p->rgba = Vectormath::Aos::Vector4( 1.f, 1.f, 1.f, 1.f );
		p->sizeRotTime = Vectormath::Aos::Vector4( 1.f, 1.f, 0.f, 10.f );
		p++;
	}
}

void ParticleEllipsoidEmitter::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );
	::Bind( "dim", dim, key, value, "-1 -1 -1" );
}

ParticleAnimator::ParticleAnimator( const char *name, GameObject *owner ) : PARENT(name,owner) 
{
}

void ParticleAnimator::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );
}

void ParticleAnimator::Animate( )
{
	ParticleEmitter *emitter = FindComponent<ParticleEmitter>();

	int numParticles;
	Particle *particles = emitter->GetParticles( numParticles );

	Vectormath::Aos::Vector3 dt(1.f/30.f);
	for (int i=0; i<numParticles; i++)
	{
		particles[i].pos += Vectormath::Aos::mulPerElem( particles[i].vel, dt ); 
		particles[i].vel += Vectormath::Aos::mulPerElem( particles[i].acc, dt ); 
	}
}



void ParticleRenderer::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );
}

void ParticleBillboardRenderer::SetMaterial( const char *value )
{
	material = materialManager()->Load( value );
}

void ParticleBillboardRenderer::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );
	::Bind< const char* >( "material", this, &ParticleBillboardRenderer::SetMaterial, key, value );
}

int ParticleBillboardRenderer::AddRenderCommands( struct RenderCommand *cmds )
{
	ParticleEmitter *emitter = FindComponent<ParticleEmitter>();

	int numParticles;
	Particle *particles = emitter->GetParticles( numParticles );

	if ( numParticles == 0 )
	{
		return 0;
	}

	ModelGeometry *geom = new(FrameAlloc( sizeof(ModelGeometry) )) ModelGeometry;

	int num = 0;
	const Technique *tech = material->GetTechnique();
	for (int i=0; i<tech->NumPasses();i++)
	{
		cmds->lightmapBinds = NULL;
		cmds->geom = geom;
		cmds->mat = material;
		cmds->tech = tech->GetPass(num);
		cmds->m_localToWorld = Vectormath::Aos::Transform3::identity();
		cmds->passIdx = cmds->tech->pass;
		cmds++;
		num++;
	}

	geom->m_indices = (unsigned short*)FrameAlloc( numParticles * 6 * sizeof(unsigned short) );
	geom->m_numIndices = numParticles * 6;
	geom->m_verts = (ModelVert*)FrameAlloc( numParticles * 4 * sizeof(ModelVert) );
	geom->m_numVerts = numParticles * 4;
	for ( int i=0; i<numParticles; i++ )
	{
		unsigned short vbase = (unsigned short)i*4;
		int ibase = i * 6;

		geom->m_indices[ibase+0] = ( vbase + 2 );
		geom->m_indices[ibase+1] = ( vbase + 1 );
		geom->m_indices[ibase+2] = ( vbase + 0 );

		geom->m_indices[ibase+3] = ( vbase + 3 );
		geom->m_indices[ibase+4] = ( vbase + 2 );
		geom->m_indices[ibase+5] = ( vbase + 0 );

		ModelVert *v0 = &geom->m_verts[vbase+0];
		ModelVert *v1 = &geom->m_verts[vbase+1];
		ModelVert *v2 = &geom->m_verts[vbase+2];
		ModelVert *v3 = &geom->m_verts[vbase+3];

		v0->p[0] = v1->p[0] = v2->p[0] = v3->p[0] = particles->pos.getX();
		v0->p[1] = v1->p[1] = v2->p[1] = v3->p[1] = particles->pos.getY();
		v0->p[2] = v1->p[2] = v2->p[2] = v3->p[2] = particles->pos.getZ();

		v0->n[0] =  1.f; v0->n[1] =  1.f;
		v1->n[0] = -1.f; v1->n[1] =  1.f;
		v2->n[0] = -1.f; v2->n[1] = -1.f;
		v3->n[0] =  1.f; v3->n[1] = -1.f;

		v0->t[0] = 0.f; v0->t[1] = 0.f;
		v1->t[0] = 1.f; v1->t[1] = 0.f;
		v2->t[0] = 1.f; v2->t[1] = 1.f;
		v3->t[0] = 0.f; v3->t[1] = 1.f;

		particles++;
	}

	return num;
}

void ParticleModelRenderer::SetModel( const char *data )
{
	model = modelManager()->Load( data );
}

void ParticleModelRenderer::Bind( const char *key, const char *value )
{
	PARENT::Bind( key, value );
	::Bind< const char* >( "model", this, &ParticleModelRenderer::SetModel, key, value );
}

int ParticleModelRenderer::AddRenderCommands( struct RenderCommand *cmds )
{
	ParticleEmitter *emitter = FindComponent<ParticleEmitter>();

	int numParticles;
	Particle *particles = emitter->GetParticles( numParticles );

	int count = 0;
	int numSurfaces = model->NumSurfaces();
	for ( int i=0; i<numParticles; i++ )
	{
		Vectormath::Aos::Transform3 tform = Vectormath::Aos::Transform3::identity();
		tform.setTranslation( Vectormath::Aos::Vector3( particles[i].pos ) );

		for ( int j=0; j<numSurfaces; j++ )
		{
			const RenderSurface *surf = model->Surface(j);
			const Material *mat = surf->mat;
			const ModelGeometry *geom = surf->geom;
			const Technique *tech = mat->GetTechnique();
			for (int k=0; k<tech->NumPasses(); k++)
			{
				cmds->lightmapBinds = NULL;
				cmds->geom = geom;
				cmds->mat = mat;
				cmds->tech = tech->GetPass(k);
				cmds->m_localToWorld = tform;
				cmds->passIdx = cmds->tech->pass;
				count++;
				cmds++;
			}
		}
	}

	return count;
}

