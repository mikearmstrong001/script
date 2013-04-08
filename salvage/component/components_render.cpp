#include "dxut.h"
#include "components_render.h"
#include "material_manager.h"
#include "lightmapbinding_manager.h"
#include "bind.h"
#include "faff.h"
#include "model_manager.h"
#include "technique.h"
#include "flare.h"
#include "world_db.h"


class Filter : public Renderable
{
	DECLAREINFO( Filter, Renderable );
	const Material *material;

	void SetMaterial( const char *data )
	{
		material = materialManager()->Load( data );
	}
public:
	Filter( const char *name, class GameObject *go ) : PARENT(name, go)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "material", this, &THISCLASS::SetMaterial, tag, data );
	}

	virtual int AddRenderCommands( struct RenderCommand *cmds )
	{
		unsigned short *indices = (unsigned short*)FrameAlloc( 6 * sizeof(unsigned short) );
		ModelVert* verts = (ModelVert*)FrameAlloc( 4 * sizeof(ModelVert) );
		ModelGeometry* geom = (ModelGeometry*)FrameAlloc( sizeof(ModelGeometry) );

		ModelVert *v0 = &verts[0];
		ModelVert *v1 = &verts[1];
		ModelVert *v2 = &verts[2];
		ModelVert *v3 = &verts[3];

		v0->p[0] = -1.f;
		v0->p[1] = -1.f;
		v0->p[2] = 0.001f;

		v1->p[0] =  1.f;
		v1->p[1] = -1.f;
		v1->p[2] = 0.001f;

		v2->p[0] =  1.f;
		v2->p[1] =  1.f;
		v2->p[2] = 0.001f;

		v3->p[0] = -1.f;
		v3->p[1] =  1.f;
		v3->p[2] = 0.001f;

		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;

		indices[3] = 0;
		indices[4] = 3;
		indices[5] = 2;

		const Technique *tech = material->GetTechnique();

		geom->m_indices = indices;
		geom->m_verts = verts;
		geom->m_numIndices = 6;
		geom->m_numVerts = 4;

		for ( int i=0; i<tech->NumPasses(); i++)
		{
			cmds[i].lightmapBinds = NULL;
			cmds[i].geom = geom;
			cmds[i].m_localToWorld = Vectormath::Aos::Transform3::identity();
			cmds[i].mat = material;
			cmds[i].tech = tech->GetPass(i);
			cmds[i].passIdx = cmds[i].tech->pass;
		}

		return tech->NumPasses();
	}
};

TYPE(Filter);


class Model;
class MeshModel : public Renderable
{
	DECLAREINFO( MeshModel, Renderable );

	Model *model;
	LightMapBinding *lightmapBinds;

	void SetModel( const char *data )
	{
		model = modelManager()->Load( data );
	}

	void SetLightMapBinds( const char *data )
	{
		lightmapBinds = lightMapBindingManager()->Load( data );
	}
public:
	MeshModel( const char *name, class GameObject *go ) : model(NULL), lightmapBinds(NULL), PARENT(name, go)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "model", this, &THISCLASS::SetModel, tag, data );
		::Bind< const char* >( "lightmapbinds", this, &THISCLASS::SetLightMapBinds, tag, data );
	}

	virtual int AddRenderCommands( struct RenderCommand *cmds )
	{
		CTransform *t = FindComponent<CTransform>();
		int numSurf = 0;//model->NumSurfaces();
		for (int i=0; i<model->NumSurfaces(); i++)
		{
			const struct MeshLightmapBindSet *lmBinds = NULL;
			const RenderSurface *surf = model->Surface(i);
			const Material *mat = surf->mat;
			const ModelGeometry *geom = surf->geom;
			const Technique *tech = mat->GetTechnique();
			
			if ( lightmapBinds )
			{
				lmBinds = lightmapBinds->FindBind( surf->name.c_str() );
			}

			for (int k=0; k<tech->NumPasses(); k++)
			{
				cmds->lightmapBinds = lmBinds;
				cmds->geom = geom;
				cmds->mat = mat;
				cmds->tech = tech->GetPass(k);
				cmds->m_localToWorld = t->world();
				cmds->passIdx = cmds->tech->pass;
				numSurf++;
				cmds++;
			}
		}
		return numSurf;
	}
};

TYPE(MeshModel);


void JointedMeshModel::SetModel( const char *data )
{
	model = modelManager()->Load( data );
	assert( model->NumJoints() );
	joints = (Vectormath::Aos::Transform3*)_aligned_malloc( sizeof(Vectormath::Aos::Transform3) * model->NumJoints(), 16 );
	for (int i=0; i<model->NumJoints(); i++)
	{
		joints[i] = Vectormath::Aos::Transform3::identity();
	}
}

void JointedMeshModel::SetLightMapBinds( const char *data )
{
	lightmapBinds = lightMapBindingManager()->Load( data );
}

JointedMeshModel::JointedMeshModel( const char *name, class GameObject *go ) : model(NULL), lightmapBinds(NULL), PARENT(name, go)
{
}

JointedMeshModel::~JointedMeshModel()
{
	_aligned_free(joints);
}

void JointedMeshModel::Bind( const char *tag, const char *data )
{
	PARENT::Bind( tag, data );
	::Bind< const char* >( "model", this, &THISCLASS::SetModel, tag, data );
	::Bind< const char* >( "lightmapbinds", this, &THISCLASS::SetLightMapBinds, tag, data );
}

int JointedMeshModel::AddRenderCommands( struct RenderCommand *cmds )
{
	CTransform *t = FindComponent<CTransform>();
	int numSurf = 0;//model->NumSurfaces();
	for (int i=0; i<model->NumSurfaces(); i++)
	{
		const struct MeshLightmapBindSet *lmBinds = NULL;
		const RenderSurface *surf = model->Surface(i);
		const Material *mat = surf->mat;
		const ModelGeometry *geom = surf->geom;
		const Technique *tech = mat->GetTechnique();
		
		if ( lightmapBinds )
		{
			lmBinds = lightmapBinds->FindBind( surf->name.c_str() );
		}

		for (int k=0; k<tech->NumPasses(); k++)
		{
			cmds->lightmapBinds = lmBinds;
			cmds->geom = geom;
			cmds->mat = mat;
			cmds->tech = tech->GetPass(k);
			cmds->m_localToWorld = t->world();
			cmds->passIdx = cmds->tech->pass;
			cmds->joints = joints;
			cmds->numJoints = model->NumJoints();
			numSurf++;
			cmds++;
		}
	}
	return numSurf;
}

TYPE(JointedMeshModel);

CFlare::CFlare( const char *name, class GameObject *go ) : PARENT(name, go)
{
}

void CFlare::SetFlare( const char *value )
{
	flare = flareManager()->Load( value );
}

void CFlare::Bind( const char *tag, const char *data )
{
	PARENT::Bind( tag, data );
	::Bind< const char* >( "flare", this, &THISCLASS::SetFlare, tag, data );
}

void CFlare::AddFlare( const Vectormath::Aos::Matrix4 mvp, struct ModelGeometry *geom )
{
	CTransform *t = FindComponent<CTransform>();
	const Vectormath::Aos::Transform3 &xfrom = t->world();
	const Vectormath::Aos::Vector3 trans = xfrom.getTranslation();

	Vectormath::Aos::Vector4 clipSpace = mvp * Vectormath::Aos::Point3(trans);

	if ( clipSpace.getZ() < clipSpace.getW() && clipSpace.getZ() > -clipSpace.getW() )
	{
		for (int i=0; i<flare->NumItems(); i++)
		{
			const FlareItem &fi = flare->Item(i);
			float sx = (clipSpace.getX()/clipSpace.getW()) * fi.factor;
			float sy = (clipSpace.getY()/clipSpace.getW()) * fi.factor;

			unsigned short *indices = geom->m_indices + geom->m_numIndices;
			ModelVert* verts = geom->m_verts + geom->m_numVerts;

			ModelVert *v0 = &verts[0];
			ModelVert *v1 = &verts[1];
			ModelVert *v2 = &verts[2];
			ModelVert *v3 = &verts[3];

			v0->p[0] = -fi.scale + sx;
			v0->p[1] = -fi.scale + sy;
			v0->p[2] = 0.001f;
			v0->t[0] = fi.uv[0];
			v0->t[1] = fi.uv[1];

			v1->p[0] =  fi.scale + sx;
			v1->p[1] = -fi.scale + sy;
			v1->p[2] = 0.001f;
			v1->t[0] = fi.uv[2];
			v1->t[1] = fi.uv[1];

			v2->p[0] =  fi.scale + sx;
			v2->p[1] =  fi.scale + sy;
			v2->p[2] = 0.001f;
			v2->t[0] = fi.uv[2];
			v2->t[1] = fi.uv[3];

			v3->p[0] = -fi.scale + sx;
			v3->p[1] =  fi.scale + sy;
			v3->p[2] = 0.001f;
			v3->t[0] = fi.uv[0];
			v3->t[1] = fi.uv[3];

			indices[0] = geom->m_numVerts+0;
			indices[1] = geom->m_numVerts+2;
			indices[2] = geom->m_numVerts+1;

			indices[3] = geom->m_numVerts+0;
			indices[4] = geom->m_numVerts+3;
			indices[5] = geom->m_numVerts+2;

			geom->m_numVerts += 4;
			geom->m_numIndices += 6;
		}
	}
}



TYPE(CFlare);

static float DegToRad( float a )
{
	return a/180.f * 3.14159f;
}

class FlareRenderer : public Renderable
{
	DECLAREINFO( FlareRenderer, Renderable );
	const Material *material;

	void SetMaterial( const char *value )
	{
		material = materialManager()->Load( value );
	}
public:
	FlareRenderer( const char *name, class GameObject *go ) : PARENT(name, go)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "material", this, &THISCLASS::SetMaterial, tag, data );
	}

	virtual int AddRenderCommands( struct RenderCommand *cmds )
	{
		Camera *camera = FindComponent<Camera>();
		if ( camera == NULL )
		{
			return 0;
		}

		CTransform *cameraTransform = FindComponent<CTransform>();
		Vectormath::Aos::Matrix4 invView = Vectormath::Aos::inverse( Vectormath::Aos::Matrix4( cameraTransform->world() ) );
		Vectormath::Aos::Matrix4 scale = Vectormath::Aos::Matrix4::scale( Vectormath::Aos::Vector3( 1.f, 1.f, -1.f ) );
		Vectormath::Aos::Matrix4 persp = Vectormath::Aos::Matrix4::perspective( DegToRad( camera->Fov() ), 1.3f, camera->ZNear(), camera->ZFar() );
		Vectormath::Aos::Matrix4 mvp = persp * scale * invView;

		std::vector< CFlare* > flares;
		Owner()->World()->GatherOfType( flares );

		int numItems = 0;
		for ( int i=0; i<flares.size(); i++)
		{
			const Flare *f = flares[i]->GetFlare();
			numItems += f->NumItems();
		}

		if ( numItems == 0 )
		{
			return 0;
		}

		unsigned short *indices = (unsigned short*)FrameAlloc( numItems * 6 * sizeof(unsigned short) );
		ModelVert* verts = (ModelVert*)FrameAlloc( numItems * 4 * sizeof(ModelVert) );
		ModelGeometry* geom = (ModelGeometry*)FrameAlloc( sizeof(ModelGeometry) );

		const Technique *tech = material->GetTechnique();

		geom->m_indices = indices;
		geom->m_verts = verts;
		geom->m_numIndices = 0;
		geom->m_numVerts = 0;

		for ( unsigned int i=0; i<flares.size(); i++ )
		{
			flares[i]->AddFlare( mvp, geom );
		}

		for ( int i=0; i<tech->NumPasses(); i++)
		{
			cmds[i].lightmapBinds = NULL;
			cmds[i].geom = geom;
			cmds[i].m_localToWorld = Vectormath::Aos::Transform3::identity();
			cmds[i].mat = material;
			cmds[i].tech = tech->GetPass(i);
			cmds[i].passIdx = cmds[i].tech->pass;
		}

		return tech->NumPasses();
	}
};

TYPE(FlareRenderer);

