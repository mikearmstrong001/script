#include "dxut.h"
#include "renderpass_default.h"
#include "hw.h"
#include "model_structs.h"
#include "components_basic.h"
#include "technique.h"
#include "material_manager.h"
#include "shader.h"
#include "cvar.h"
#include "lightmapbinding.h"

static CVar r_renderSingleTri( "r_renderSingleTri", "0" );
static CVar r_renderTangents( "r_renderTangents", "0" );

void RenderPassDefault::Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights)
{
	const Material *tagentMat = materialManager()->Load( "showtangents.material" );

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


		if ( r_renderTangents.GetInt() )
		{
			mat = tagentMat;
		}

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

