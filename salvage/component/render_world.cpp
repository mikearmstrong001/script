#include "dxut.h"
#include "render_world.h"
#include "renderpass_default.h"
#include "renderpass_lightprepass.h"
#include "components_render.h"
#include "components_light.h"
#include "hw.h"
#include "model_structs.h"
#include <algorithm>

static float DegToRad( float a )
{
	return a/180.f * 3.14159f;
}



RenderWorld::RenderWorld()
{
}

RenderWorld::~RenderWorld()
{
}

void RenderWorld::RegisterRenderable( Renderable *r )
{
	std::vector< Renderable* >::iterator f = std::find( m_renderables.begin(), m_renderables.end(), r );
	if ( f != m_renderables.end() )
	{
		return;
	}
	m_renderables.push_back( r );
}

void RenderWorld::UnregisterRenderable( Renderable *r )
{
	std::vector< Renderable* >::iterator f = std::find( m_renderables.begin(), m_renderables.end(), r );
	if ( f == m_renderables.end() )
	{
		return;
	}
	m_renderables.erase( f );
}

void RenderWorld::RegisterLight( Light *l )
{
	std::vector< Light* >::iterator f = std::find( m_lights.begin(), m_lights.end(), l );
	if ( f != m_lights.end() )
	{
		return;
	}
	m_lights.push_back( l );
}

void RenderWorld::UnregisterLight( Light *l )
{
	std::vector< Light* >::iterator f = std::find( m_lights.begin(), m_lights.end(), l );
	if ( f == m_lights.end() )
	{
		return;
	}
	m_lights.erase( f );
}

void RenderWorld::RegisterCamera( Camera *c )
{
	std::vector< Camera* >::iterator f = std::find( m_cameras.begin(), m_cameras.end(), c );
	if ( f != m_cameras.end() )
	{
		return;
	}
	m_cameras.push_back( c );
}

void RenderWorld::UnregisterCamera( Camera *c )
{
	std::vector< Camera* >::iterator f = std::find( m_cameras.begin(), m_cameras.end(), c );
	if ( f == m_cameras.end() )
	{
		return;
	}
	m_cameras.erase( f );
}

void RenderWorld::RenderFrame( RenderPass **renderPasses )
{
	RenderCommand rcmds[ 2 * 1024 ];
	memset( rcmds, 0, sizeof(rcmds) );
	int rcmdsCount = 0;
	for (unsigned int i=0; i<m_renderables.size(); i++)
	{
		rcmdsCount+=m_renderables[i]->AddRenderCommands( &rcmds[rcmdsCount] );
	}
	//rcmdsCount += g_console->AddRenderCommands( &rcmds[rcmdsCount] );

	for (int i=0; i<rcmdsCount; i++)
	{
		const RenderCommand &rc = rcmds[i];
		renderPasses[rc.passIdx]->AddRenderCommand( &rcmds[i] );
	}

	for (unsigned int i=0; i<m_cameras.size(); i++)
	{
		CTransform *cameraTransform = m_cameras[i]->FindComponent<CTransform>();
		Vectormath::Aos::Matrix4 invView = Vectormath::Aos::inverse( Vectormath::Aos::Matrix4( cameraTransform->world() ) );
		Vectormath::Aos::Matrix4 persp = hw::MakeHWProjection( DegToRad( m_cameras[i]->Fov() ), 1.3f, m_cameras[i]->ZNear(), m_cameras[i]->ZFar() );

		for (int j=_RP_Start; j<_RP_End3D; j++)
		{
			if ( renderPasses[j] == NULL )
			{
				continue;
			}
			DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"RP" ); // These events are to help PIX identify what the code is doing
			renderPasses[j]->Render( invView, persp, m_lights );
			DXUT_EndPerfEvent();
		}
	}

	std::vector< Light* > lightsEmpty;
	for (int j=_RP_Start2D; j<_RP_End; j++)
	{
		if ( renderPasses[j] == NULL )
		{
			continue;
		}
		DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"RP" ); // These events are to help PIX identify what the code is doing
		renderPasses[j]->Render( Vectormath::Aos::Matrix4::identity(), Vectormath::Aos::Matrix4::identity(), lightsEmpty );
		DXUT_EndPerfEvent();
	}

	for (int j=_RP_Start; j<_RP_End; j++)
	{
		if ( renderPasses[j] == NULL )
		{
			continue;
		}
		renderPasses[j]->Empty();
	}
}

