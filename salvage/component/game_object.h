#ifndef TESTING_GAME_OBJECT_H
#define TESTING_GAME_OBJECT_H

#include <string>
#include <vector>

enum
{
	ePREUPDATE,
	eUPDATE,
	ePOSTUPDATE,
	NUM_UPDATES,
};

class Component;


class GameObject
{
	std::string m_name;
	std::string m_tag;
	std::vector< Component * > m_components;

	std::vector< Component * > m_updates[NUM_UPDATES];

	class CTransform *m_transform;
	class WorldDB *m_worlddb;
public:

	GameObject( class WorldDB *db ) : m_transform(NULL), m_worlddb(db)
	{
	}

	virtual ~GameObject()
	{
		RemoveComponents();
	}


	const char *Name() const
	{
		return m_name.c_str();
	}

	CTransform *Transform() const
	{
		return m_transform;
	}

	void Bind( const char *tag, const char *data );
	void RemoveComponents();
	void TransformUpdate();
	void Update( int idx );
	Component* AddComponent( const char *name, const char *type );

	void RegisterForUpdate( Component *c, int idx );
	void UnregisterFromUpdate( Component *c, int idx );
	void UnregisterFromAllUpdates( Component *c );

	void Enter();

	template<class T>
	T * FindComponent() const
	{
		for (unsigned int i=0; i<m_components.size(); i++)
		{
			T *t = dynamic_cast<T*>( m_components[i] );
			if ( t )
			{
				return t;
			}
		}
		return NULL;
	}

	template<class T>
	void FindComponents( std::vector<T*> &vec ) const
	{
		for (unsigned int i=0; i<m_components.size(); i++)
		{
			T *t = dynamic_cast<T*>( m_components[i] );
			if ( t )
			{
				vec.push_back( t );
			}
		}
	}

	template<class T>
	void FindComponentsChildren( std::vector<T*> &vec ) const
	{
		for (unsigned int i=0; i<m_components.size(); i++)
		{
			T *t = dynamic_cast<T*>( m_components[i] );
			if ( t )
			{
				vec.push_back( t );
			}
		}

		const std::vector< GameObject * > &k = Transform()->Kids();
		for (unsigned int i=0; i<k.size(); i++)
		{
			k[i]->FindComponentsChildren( vec );
		}
	}

	WorldDB *World() { return m_worlddb; }
};


#endif
