#include "dxut.h"
#include "components_light.h"
#include "material_manager.h"
#include "lightmapbinding_manager.h"
#include "bind.h"
#include "faff.h"
#include "model_manager.h"
#include "technique.h"
#include "flare.h"
#include "world_db.h"
#include "render_world.h"
#include "game_object.h"
#include "hw.h"

void Light::Enter()
{
	Owner()->World()->GetRenderWorld()->RegisterLight( this );
}

void Light::Remove()
{
	Owner()->World()->GetRenderWorld()->UnregisterLight( this );
}

void Light::Bind( const char *tag, const char *data )
{
	PARENT::Bind( tag, data );
	::Bind( "Colour", lightColour, tag, data, "1 1 1 1" );
	::Bind( "Intensity", lightIntensity, tag, data, "1" );
}


class PointLight : public Light
{
	DECLAREINFO( PointLight, Light );

	const Material *material;
	Vectormath::Aos::Vector3 lightAttenuation;

	void SetMaterial( const char *value )
	{
		material = materialManager()->Load( value );
	}

public:
	PointLight( const char *name, class GameObject *go ) : PARENT(name, go), material(NULL)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "material", this, &THISCLASS::SetMaterial, tag, data );
		::Bind( "Attenuation", lightAttenuation, tag, data, "1 0.1 0" );
	}

	virtual void RenderLight( const Vectormath::Aos::Matrix4 &v )
	{
		CTransform *t = FindComponent<CTransform>();
		const Vectormath::Aos::Transform3 &xfrom = t->world();
		const Vectormath::Aos::Vector3 trans = xfrom.getTranslation();

		Vectormath::Aos::Vector4 light = v * Vectormath::Aos::Point3( trans );
		int hLightPos = hw::GetParamSlot( "lightPos", 1 );
		int hLightColour = hw::GetParamSlot( "lightColour", 1 );
		int hLightAttenuation = hw::GetParamSlot( "lightAttenuation", 1 );
		int hLightDirection = hw::GetParamSlot( "lightDirection", 1 );
		hw::SetParamInSlot( hLightPos, &light[0], 1 );
		hw::SetParamInSlot( hLightColour, lightColour[0] * lightIntensity, lightColour[1] * lightIntensity, lightColour[2] * lightIntensity, lightColour[3] * lightIntensity );
		hw::SetParamInSlot( hLightAttenuation, lightAttenuation[0], lightAttenuation[1], lightAttenuation[2], -1.f );
		hw::SetParamInSlot( hLightDirection, 0.f, 0.f, 0.f, 0.f );

		unsigned short *indices = (unsigned short*)FrameAlloc( 6 * sizeof(unsigned short) );
		ModelVert* verts = (ModelVert*)FrameAlloc( 4 * sizeof(ModelVert) );

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

		material->Bind();
		tech->GetPass(0)->Bind();
		tech->GetPass(0)->shader->Bind( Shader::kStatic );

		hw::DrawIndexed( &verts[0], &indices[0], 4, 6 );
	}
};

TYPE(PointLight);

class DirectionalLight : public Light
{
	DECLAREINFO( DirectionalLight, Light );

	const Material *material;
	Vectormath::Aos::Vector3 lightDirection;

	void SetMaterial( const char *value )
	{
		material = materialManager()->Load( value );
	}

public:
	DirectionalLight( const char *name, class GameObject *go ) : PARENT(name, go), material(NULL)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "material", this, &THISCLASS::SetMaterial, tag, data );
		::Bind( "Direction", lightDirection, tag, data, "0 0 1" );
	}

	virtual void RenderLight( const Vectormath::Aos::Matrix4 &v )
	{
		CTransform *t = FindComponent<CTransform>();
		const Vectormath::Aos::Transform3 &xform = t->world();

		Vectormath::Aos::Vector4 light = v * xform * lightDirection;
		int hLightPos = hw::GetParamSlot( "lightPos", 1 );
		int hLightColour = hw::GetParamSlot( "lightColour", 1 );
		int hLightAttenuation = hw::GetParamSlot( "lightAttenuation", 1 );
		int hLightDirection = hw::GetParamSlot( "lightDirection", 1 );
		hw::SetParamInSlot( hLightPos, -light[0], -light[1], -light[2], 1 );
		hw::SetParamInSlot( hLightColour, lightColour[0] * lightIntensity, lightColour[1] * lightIntensity, lightColour[2] * lightIntensity, lightColour[3] * lightIntensity );
		hw::SetParamInSlot( hLightAttenuation, 1.f, 0.f, 0.f, -1.f );
		hw::SetParamInSlot( hLightDirection, 0.f, 0.f, 0.f, 0.f );

		unsigned short *indices = (unsigned short*)FrameAlloc( 6 * sizeof(unsigned short) );
		ModelVert* verts = (ModelVert*)FrameAlloc( 4 * sizeof(ModelVert) );

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

		material->Bind();
		tech->GetPass(0)->Bind();
		tech->GetPass(0)->shader->Bind( Shader::kStatic );

		hw::DrawIndexed( &verts[0], &indices[0], 4, 6 );
	}
};

TYPE(DirectionalLight);

class SpotLight : public Light
{
	DECLAREINFO( SpotLight, Light );

	const Material *material;
	Vectormath::Aos::Vector3 lightAttenuation;
	Vectormath::Aos::Vector3 lightDirection;
	float lightConeAngle;

	void SetMaterial( const char *value )
	{
		material = materialManager()->Load( value );
	}

public:
	SpotLight( const char *name, class GameObject *go ) : PARENT(name, go), material(NULL)
	{
	}

	virtual void Bind( const char *tag, const char *data )
	{
		PARENT::Bind( tag, data );
		::Bind< const char* >( "material", this, &THISCLASS::SetMaterial, tag, data );
		::Bind( "Attenuation", lightAttenuation, tag, data, "1 0.1 0" );
		::Bind( "Direction", lightDirection, tag, data, "0 0 1" );
		::Bind( "ConeAngle", lightConeAngle, tag, data, "45" );
	}

	virtual void RenderLight( const Vectormath::Aos::Matrix4 &v )
	{
		CTransform *t = FindComponent<CTransform>();
		const Vectormath::Aos::Transform3 &xform = t->world();
		const Vectormath::Aos::Vector3 trans = xform.getTranslation();

		Vectormath::Aos::Vector4 light = v * Vectormath::Aos::Point3( trans );
		Vectormath::Aos::Vector4 lightD = v * xform * lightDirection;
		int hLightPos = hw::GetParamSlot( "lightPos", 1 );
		int hLightColour = hw::GetParamSlot( "lightColour", 1 );
		int hLightAttenuation = hw::GetParamSlot( "lightAttenuation", 1 );
		int hLightDirection = hw::GetParamSlot( "lightDirection", 1 );
		hw::SetParamInSlot( hLightPos, &light[0], 1 );
		hw::SetParamInSlot( hLightColour, lightColour[0] * lightIntensity, lightColour[1] * lightIntensity, lightColour[2] * lightIntensity, lightColour[3] * lightIntensity );
		hw::SetParamInSlot( hLightAttenuation, lightAttenuation[0], lightAttenuation[1], lightAttenuation[2], (float)cosf( lightConeAngle/180.f * M_PI ) );
		hw::SetParamInSlot( hLightDirection, lightD[0], lightD[1], lightD[2], 0.f );

		unsigned short *indices = (unsigned short*)FrameAlloc( 6 * sizeof(unsigned short) );
		ModelVert* verts = (ModelVert*)FrameAlloc( 4 * sizeof(ModelVert) );

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

		material->Bind();
		tech->GetPass(0)->Bind();
		tech->GetPass(0)->shader->Bind( Shader::kStatic );

		hw::DrawIndexed( &verts[0], &indices[0], 4, 6 );
	}
};

TYPE(SpotLight);
