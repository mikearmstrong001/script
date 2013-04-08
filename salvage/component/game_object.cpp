#include "game_object.h"
#include "world_db.h"

#include "faff.h"
#include "bind.h"
#include "components_core.h"
#include "components_basic.h"

#include <algorithm>


void GameObject::Bind( const char *tag, const char *data )
{
	::Bind( "name", m_name, tag, data, "" ); 
	::Bind( "tag", m_tag, tag, data, "" ); 
}

void GameObject::Enter()
{
	m_transform = FindComponent<CTransform>();
	for (unsigned int i=0; i<m_components.size(); i++)
	{
		m_components[i]->Enter();
	}
}

void GameObject::RemoveComponents()
{
	for (unsigned int i=0; i<m_components.size(); i++)
	{
		m_components[i]->Remove();
	}
	for (unsigned int i=0; i<m_components.size(); i++)
	{
		m_components[i]->~Component();
		_aligned_free( m_components[i] );
	}
	m_components.clear();
}


void GameObject::Update( int idx )
{
	for (unsigned int i=0; i<m_updates[idx].size(); i++)
	{
		m_updates[idx][i]->Update( idx );
	}
}

void GameObject::TransformUpdate()
{
	m_transform->Update();
}


Component* GameObject::AddComponent( const char *name, const char *type )
{
	Component *c = ComponentConstructor::Create( type, name, this );
	if ( c )
	{
		m_components.push_back( c );
	}
	return c;
}

void GameObject::RegisterForUpdate( Component *c, int idx )
{
//	ASSERT( idx >= 0 && idx < NUM_UPDATES );
	std::vector<Component*>::iterator f = std::find( m_updates[idx].begin(), m_updates[idx].end(), c );
	if ( f != m_updates[idx].end() )
	{
		return;
	}
	if ( m_updates[idx].size() == 0 )
	{
		m_worlddb->RegisterForUpdate( this, idx );
	}
	m_updates[idx].push_back(c);
}

void GameObject::UnregisterFromUpdate( Component *c, int idx )
{
//	ASSERT( idx >= 0 && idx < NUM_UPDATES );
	std::vector<Component*>::iterator f = std::find( m_updates[idx].begin(), m_updates[idx].end(), c );
	if ( f == m_updates[idx].end() )
	{
		return;
	}
	m_updates[idx].erase(f);
	if ( m_updates[idx].size() == 0 )
	{
		m_worlddb->UnregisterFromUpdate( this, idx );
	}
}

void GameObject::UnregisterFromAllUpdates( Component *c )
{
	for ( int i=0; i<NUM_UPDATES; i++)
	{
		UnregisterFromUpdate( c, i );
	}
}

