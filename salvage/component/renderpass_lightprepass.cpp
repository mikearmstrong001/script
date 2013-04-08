#include "dxut.h"
#include "renderpass_lightprepass.h"
#include "hw.h"
#include "model_structs.h"
#include "components_basic.h"
#include "components_light.h"
#include "technique.h"
#include "material_manager.h"
#include "texture_manager.h"
#include "shader.h"
#include "cvar.h"
#include "faff.h"
#include "lightmapbinding.h"


RenderPassLightPrePass::RenderPassLightPrePass()
{
	rt0 = textureManager()->CreateRenderTexture( hw::GetBackbufferWidth(), hw::GetBackbufferHeight(), "_depthNormal" );
}


void RenderPassLightPrePass::Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights)
{
	int hP = hw::GetParamSlot( "Persp", 4 );
	int hM = hw::GetParamSlot( "Model", 4 );
	int hMV = hw::GetParamSlot( "ModelView", 4 );
	int hMVP = hw::GetParamSlot( "ModelViewPersp", 4 );
	int hMJ = hw::GetParamSlot( "ModelJoint", 3 * 64 );
	Vectormath::Aos::Matrix4 vp = p * v;

	hw::SetParamInSlot( hP, p );

	hw::SetRT( rt0->HW(), hw::GetDefaultDepthStencil() );
	hw::ClearRT( true, 0xffffffff, false, 1.f );

	for (unsigned int i=0; i<renderables.size(); i++)
	{
		RenderCommand &rc = *renderables[i];
		Vectormath::Aos::Matrix4 mv = v * rc.m_localToWorld;
		Vectormath::Aos::Matrix4 mvp = vp * rc.m_localToWorld;
		hw::SetParamInSlot( hM, Vectormath::Aos::Matrix4(rc.m_localToWorld) );
		hw::SetParamInSlot( hMV, mv );
		hw::SetParamInSlot( hMVP, mvp );
		if ( rc.numJoints )
		{
			hw::SetParamInSlot( hMJ, rc.joints, rc.numJoints );
		}

		const Material *mat = rc.mat;
		const TechniquePass *tech = rc.tech;
		const Shader *shader = tech->shader;

		mat->Bind();
		tech->Bind();
		shader->Bind( rc.numJoints ? Shader::kSkinned : Shader::kStatic );

		hw::DrawGeometry( rc.geom );
	}

	hw::SetDefaultRT();
}



RenderPassLightPrePassLights::RenderPassLightPrePassLights()
{
	depthNormal = textureManager()->Find( "_depthNormal", false );
	rt0 = textureManager()->CreateRenderTexture( hw::GetBackbufferWidth(), hw::GetBackbufferHeight(), "_light" );
}


void RenderPassLightPrePassLights::Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights)
{
	int hDepthNormal = hw::GetTextureSlot( "depthNormal" );
	hw::SetTextureInSlot( hDepthNormal, depthNormal->HW(), hw::SS_POINT_POINT );

	int hP = hw::GetParamSlot( "Persp", 4 );
	hw::SetParamInSlot( hP, p );

	int hScreenToView = hw::GetParamSlot( "screenToView", 1 );
	float screenToView[4] = { 1.f/p[0][0], -1.f/p[1][1], p[0][0], 0.f };
	hw::SetParamInSlot( hScreenToView, screenToView, 1 );

	Vectormath::Aos::Vector4 light = v * -Vectormath::Aos::normalize( Vectormath::Aos::Vector3( -1.f, -1.f, 0.f ) );
	int hLightPos = hw::GetParamSlot( "lightPos", 1 );
	hw::SetParamInSlot( hLightPos, &light[0], 1 );
	int hLightColour = hw::GetParamSlot( "lightColour", 1 );
	int hLightAttenuation = hw::GetParamSlot( "lightAttenuation", 1 );
	int hLightDirection = hw::GetParamSlot( "lightDirection", 1 );
	hw::SetParamInSlot( hLightColour, 1.f, 0.1f, 0.1f, 1.f );
	hw::SetParamInSlot( hLightAttenuation, 1.f, 0.f, 0.f, 0.5f );
	hw::SetParamInSlot( hLightDirection, -light[0], -light[1], -light[2], 0.f );
//	int hM = hw::GetParamSlot( "Model", 4 );
//	int hMV = hw::GetParamSlot( "ModelView", 4 );
//	int hMVP = hw::GetParamSlot( "ModelViewPersp", 4 );
	Vectormath::Aos::Matrix4 vp = p * v;


	hw::SetRT( rt0->HW(), hw::GetDefaultDepthStencil() );
	hw::ClearRT( true, false );

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

	const Material *mat = materialManager()->Load( "debug/lighttest.material" );
	const Technique *tech = mat->GetTechnique();

	mat->Bind();
	tech->GetPass(0)->Bind();
	tech->GetPass(0)->shader->Bind( Shader::kStatic );

	//hw::DrawIndexed( &verts[0], &indices[0], 4, 6 );

	for (unsigned int i=0; i<lights.size(); i++)
	{
		lights[i]->RenderLight( v );
	}

	hw::SetDefaultRT();
}


RenderPassLightPrePassOpaque::RenderPassLightPrePassOpaque()
{
	light = textureManager()->Find( "_light", false );;
}

void RenderPassLightPrePassOpaque::Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights)
{
	int hLight = hw::GetTextureSlot( "light" );
	hw::SetTextureInSlot( hLight, light->HW(), hw::SS_POINT_POINT );

	int hP = hw::GetParamSlot( "Persp", 4 );
	int hM = hw::GetParamSlot( "Model", 4 );
	int hMV = hw::GetParamSlot( "ModelView", 4 );
	int hMVP = hw::GetParamSlot( "ModelViewPersp", 4 );
	int hMJ = hw::GetParamSlot( "ModelJoint", 3 * 64 );
	Vectormath::Aos::Matrix4 vp = p * v;

	hw::SetParamInSlot( hP, p );

	for (unsigned int i=0; i<renderables.size(); i++)
	{
		RenderCommand &rc = *renderables[i];
		Vectormath::Aos::Matrix4 mv = v * rc.m_localToWorld;
		Vectormath::Aos::Matrix4 mvp = vp * rc.m_localToWorld;
		hw::SetParamInSlot( hM, Vectormath::Aos::Matrix4(rc.m_localToWorld) );
		hw::SetParamInSlot( hMV, mv );
		hw::SetParamInSlot( hMVP, mvp );
		if ( rc.numJoints )
		{
			hw::SetParamInSlot( hMJ, rc.joints, rc.numJoints );
		}

		const Material *mat = rc.mat;
		const TechniquePass *tech = rc.tech;
		const Shader *shader = tech->shader;

		if ( rc.lightmapBinds )
		{
			rc.lightmapBinds->Bind();
		}

		mat->Bind();
		tech->Bind();
		shader->Bind( rc.numJoints ? Shader::kSkinned : Shader::kStatic );

		hw::DrawGeometry( rc.geom );
	}
}

RenderPassLightPrePassStereoscopic::RenderPassLightPrePassStereoscopic()
{
	depthNormal = textureManager()->Find( "_depthNormal", false );
	leftRt0 = textureManager()->CreateRenderTexture( hw::GetBackbufferWidth(), hw::GetBackbufferHeight(), "_leftEye" );
	rightRt0 = textureManager()->CreateRenderTexture( hw::GetBackbufferWidth(), hw::GetBackbufferHeight(), "_rightEye" );
	//const Material *mat = materialManager()->Load( "debug/reproject.material" );
}

static CVar r_s3d_separation( "r_s3d_separation", "0.03" );
static CVar r_s3d_distortDepth( "r_s3d_distortDepth", "0.0" );
static CVar r_s3d_enabled( "r_s3d_enabled", "0" );
static CVar r_s3d_focus( "r_s3d_focus", "0.6" );
static CVar r_s3d_slicedist( "r_s3d_slicedist", "2" );
static CVar r_s3d_numslices( "r_s3d_numslices", "16" );

inline const Vectormath::Aos::Matrix4 StereoSkew( float stereo, float focus )
{
    return Vectormath::Aos::Matrix4(
        Vectormath::Aos::Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
        Vectormath::Aos::Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
        Vectormath::Aos::Vector4( -stereo/focus, 0.0f, 1.0f, 0.f  ),
        Vectormath::Aos::Vector4( stereo, 0.0f, 0.0f, 1.0f )
    );
}


void RenderPassLightPrePassStereoscopic::Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights)
{
	if ( r_s3d_enabled.GetInt() == 0 )
	{
		return;
	}

	if ( r_s3d_enabled.GetInt() == 1 )
	{
		int hDepthNormal = hw::GetTextureSlot( "depthNormal" );
		hw::SetTextureInSlot( hDepthNormal, depthNormal->HW(), hw::SS_POINT_POINT );

		int hP = hw::GetParamSlot( "Persp", 4 );
		hw::SetParamInSlot( hP, p );

		int hSP = hw::GetParamSlot( "StereoParams", 1 );
		hw::SetParamInSlot( hSP	, r_s3d_separation.GetFloat(), r_s3d_focus.GetFloat(), 0.f, 0.f );

		int hScreenToView = hw::GetParamSlot( "screenToView", 1 );
		float screenToView[4] = { 1.f/p[0][0], -1.f/p[1][1], p[0][0], p[2][3] };
		hw::SetParamInSlot( hScreenToView, screenToView, 1 );

		Vectormath::Aos::Matrix4 vp = p * v;

		unsigned short *indices = (unsigned short*)FrameAlloc( 6 * sizeof(unsigned short) );
		ModelVert* verts = (ModelVert*)FrameAlloc( 4 * sizeof(ModelVert) );

		ModelVert *v0 = &verts[0];
		ModelVert *v1 = &verts[1];
		ModelVert *v2 = &verts[2];
		ModelVert *v3 = &verts[3];

		v0->p[0] = -1.f; v0->p[1] = -1.f; v0->p[2] = 0.001f;
		v0->t[0] = r_s3d_distortDepth.GetFloat(); v0->t[1] = 1.f;
		v0->t2[0] = 0.f; v0->t2[1] = 1.f;

		v1->p[0] =  1.f; v1->p[1] = -1.f; v1->p[2] = 0.001f;
		v1->t[0] = 1.f; v1->t[1] = 1.f;
		v1->t2[0] = 1.f-r_s3d_distortDepth.GetFloat(); v1->t2[1] = 1.f;

		v2->p[0] =  1.f; v2->p[1] =  1.f; v2->p[2] = 0.001f;
		v2->t[0] = 1.f; v2->t[1] = 0.f;
		v2->t2[0] = 1.f-r_s3d_distortDepth.GetFloat(); v2->t2[1] = 0.f;

		v3->p[0] = -1.f; v3->p[1] =  1.f; v3->p[2] = 0.001f;
		v3->t[0] = r_s3d_distortDepth.GetFloat(); v3->t[1] = 0.f; 
		v3->t2[0] = 0.f; v3->t2[1] = 0.f;

		indices[0] = 0; indices[1] = 2; indices[2] = 1;
		indices[3] = 0; indices[4] = 3; indices[5] = 2;

		const Material *mat = materialManager()->Load( "debug/reproject.material" );
		const Technique *tech = mat->GetTechnique();

		mat->Bind();
		tech->GetPass(0)->Bind();
		tech->GetPass(0)->shader->Bind( Shader::kStatic );

		hw::DrawIndexed( &verts[0], &indices[0], 4, 6 );
	}
	if ( r_s3d_enabled.GetInt() == 2 )
	{
		int hDepthNormal = hw::GetTextureSlot( "depthNormal" );
		hw::SetTextureInSlot( hDepthNormal, depthNormal->HW(), hw::SS_POINT_POINT );


		int hSP = hw::GetParamSlot( "StereoParams", 1 );
		hw::SetParamInSlot( hSP	, r_s3d_separation.GetFloat(), r_s3d_focus.GetFloat(), 0.f, 0.f );

		int hScreenToView = hw::GetParamSlot( "screenToView", 1 );
		float screenToView[4] = { 1.f/p[0][0], -1.f/p[1][1], p[0][0], p[2][3] };
		hw::SetParamInSlot( hScreenToView, screenToView, 1 );


		unsigned short *indices = (unsigned short*)FrameAlloc( 6 * sizeof(unsigned short) );
		ModelVert* verts = (ModelVert*)FrameAlloc( 4 * sizeof(ModelVert) );

		ModelVert *v0 = &verts[0]; ModelVert *v1 = &verts[1]; ModelVert *v2 = &verts[2]; ModelVert *v3 = &verts[3];

		float zvalue = r_s3d_slicedist.GetFloat() * screenToView[3];
		float xvalue = screenToView[0] * zvalue;
		float yvalue = -screenToView[1] * zvalue;
		v0->p[0] = -xvalue; v0->p[1] = -yvalue; v0->p[2] = zvalue;
		v0->t[0] = 0.f; v0->t[1] = 1.f;
		v0->t2[0] = 0.f; v0->t2[1] = 1.f;

		v1->p[0] =  xvalue; v1->p[1] = -yvalue; v1->p[2] = zvalue;
		v1->t[0] = 1.f; v1->t[1] = 1.f;
		v1->t2[0] = 1.f; v1->t2[1] = 1.f;

		v2->p[0] =  xvalue; v2->p[1] = yvalue; v2->p[2] = zvalue;
		v2->t[0] = 1.f; v2->t[1] = 0.f;
		v2->t2[0] = 1.f; v2->t2[1] = 0.f;

		v3->p[0] = -xvalue; v3->p[1] = yvalue; v3->p[2] = zvalue;
		v3->t[0] = 0.f; v3->t[1] = 0.f; 
		v3->t2[0] = 0.f; v3->t2[1] = 0.f;

		indices[0] = 0; indices[1] = 2; indices[2] = 1;
		indices[3] = 0; indices[4] = 3; indices[5] = 2;

		const Material *mat = materialManager()->Load( "debug/stereo2.material" );
		const Technique *tech = mat->GetTechnique();

		//hw::SetRT( leftRt0->HW(), hw::GetDefaultDepthStencil() );

		hw::CopyScreen();
		hw::ClearRT( true, false );

		for ( int i=0; i<r_s3d_numslices.GetInt(); i++)
		{
			float zvalue = r_s3d_slicedist.GetFloat() * screenToView[3] * (r_s3d_numslices.GetInt()-i) * (r_s3d_numslices.GetInt()-i);
			float xvalue = screenToView[0] * zvalue;
			float yvalue = -screenToView[1] * zvalue;
			v0->p[0] = -xvalue; v0->p[1] = -yvalue; v0->p[2] = zvalue;
			v1->p[0] =  xvalue; v1->p[1] = -yvalue; v1->p[2] = zvalue;
			v2->p[0] =  xvalue; v2->p[1] = yvalue; v2->p[2] = zvalue;
			v3->p[0] = -xvalue; v3->p[1] = yvalue; v3->p[2] = zvalue;
			{
				hw::ColourWrite( true, false, false, false );

				Vectormath::Aos::Matrix4 skewP = StereoSkew( r_s3d_separation.GetFloat(), r_s3d_focus.GetFloat() ) * p;
				//skewP = p;
				int hP = hw::GetParamSlot( "Persp", 4 );
				hw::SetParamInSlot( hP, skewP );
				Vectormath::Aos::Matrix4 offsetV = Vectormath::Aos::Matrix4::translation( Vectormath::Aos::Vector3( r_s3d_separation.GetFloat(), 0.f, 0.f ) );
				Vectormath::Aos::Matrix4 vp = skewP * offsetV;
				int hMVP = hw::GetParamSlot( "ModelViewPersp", 4 );
				hw::SetParamInSlot( hMVP, vp );
				mat->Bind();
				tech->GetPass(0)->Bind();
				tech->GetPass(0)->shader->Bind( Shader::kStatic );
				hw::DrawIndexed( &verts[0], &indices[0], 4, 6 );
			}

			{
				hw::ColourWrite( false, true, true, false );

				Vectormath::Aos::Matrix4 skewP = StereoSkew( -r_s3d_separation.GetFloat(), r_s3d_focus.GetFloat() ) * p;
				//skewP = p;
				int hP = hw::GetParamSlot( "Persp", 4 );
				hw::SetParamInSlot( hP, skewP );
				Vectormath::Aos::Matrix4 offsetV = Vectormath::Aos::Matrix4::translation( Vectormath::Aos::Vector3( -r_s3d_separation.GetFloat(), 0.f, 0.f ) );
				Vectormath::Aos::Matrix4 vp = skewP * offsetV;
				int hMVP = hw::GetParamSlot( "ModelViewPersp", 4 );
				hw::SetParamInSlot( hMVP, vp );
				mat->Bind();
				tech->GetPass(0)->Bind();
				tech->GetPass(0)->shader->Bind( Shader::kStatic );
				hw::DrawIndexed( &verts[0], &indices[0], 4, 6 );
			}
		}

		hw::ColourWrite( true, true, true, true );
	}

	hw::SetDefaultRT();
}
