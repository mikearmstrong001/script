#ifndef TESTING_WORLD_DB_H
#define TESTING_WORLD_DB_H

#include <vector>
#include "game_object.h"

class GameObject;
class PhysicsWorld;
class RenderWorld;
class RenderPass;

class WorldDB
{
	std::vector< GameObject* > m_gos;
	std::vector< GameObject* > m_updates[NUM_UPDATES];
	PhysicsWorld *m_physicsWorld;
	RenderWorld *m_renderWorld;
public:

	WorldDB();
	~WorldDB();

	void Init();

	void Load( const char *filename );
	void Purge();

	void TransformUpdate();
	void Update( int idx );

	void Render( RenderPass **renderPasses );

	void RegisterForUpdate( GameObject *o, int idx );
	void UnregisterFromUpdate( GameObject *o, int idx );

	template< class T >
	void GatherOfType( std::vector<T*> &gather )
	{
		std::vector< GameObject* >::const_iterator c = m_gos.begin();
		while ( c != m_gos.end() )
		{
			(*c)->FindComponentsChildren( gather );
			c++;
		}
	}

	PhysicsWorld *GetPhysicsWorld() { return m_physicsWorld; }
	RenderWorld *GetRenderWorld() { return m_renderWorld; }
};

#endif
