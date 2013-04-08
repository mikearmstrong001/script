#include "dxut.h"
#include "world_db.h"
#include "faff.h"
#include "game_object.h"
#include "physics_world.h"
#include "render_world.h"
#include <algorithm>

WorldDB::WorldDB() : m_physicsWorld(NULL), m_renderWorld(NULL)
{
}

WorldDB::~WorldDB()
{
	Purge();
}

void WorldDB::Init()
{
	m_physicsWorld = new PhysicsWorld;
	m_renderWorld = new RenderWorld;
}

void WorldDB::Load( const char *filename )
{
	::ParseXML( m_gos, filename, Vectormath::Aos::Transform3::identity(), this );
}

void WorldDB::Purge()
{
	for ( unsigned int i=0; i<m_gos.size(); i++ )
	{
		delete m_gos[i];
	}
	m_gos.clear();
	delete m_physicsWorld;
	delete m_renderWorld;
	m_physicsWorld = NULL;
	m_renderWorld = NULL;
}

void WorldDB::TransformUpdate()
{
	for ( unsigned int i=0; i<m_gos.size(); i++ )
	{
		m_gos[i]->TransformUpdate();
	}
}

void WorldDB::Update( int idx )
{
	if ( idx == eUPDATE )
	{
		m_physicsWorld->Update();
	}
	for ( unsigned int i=0; i<m_updates[idx].size(); i++ )
	{
		m_updates[idx][i]->Update(idx);
	}
}

void WorldDB::Render( RenderPass **renderPasses )
{
	m_renderWorld->RenderFrame(renderPasses);
}

void WorldDB::RegisterForUpdate( GameObject *o, int idx )
{
	std::vector<GameObject*>::iterator f = std::find( m_updates[idx].begin(), m_updates[idx].end(), o );
	if ( f != m_updates[idx].end() )
	{
		return;
	}
	m_updates[idx].push_back(o);
}

void WorldDB::UnregisterFromUpdate( GameObject *o, int idx )
{
	std::vector<GameObject*>::iterator f = std::find( m_updates[idx].begin(), m_updates[idx].end(), o );
	if ( f == m_updates[idx].end() )
	{
		return;
	}
	m_updates[idx].erase(f);
}
