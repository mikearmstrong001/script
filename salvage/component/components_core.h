#ifndef TESTING_COMPONENTS_CORE_H
#define TESTING_COMPONENTS_CORE_H

#include <string>
#include "game_object.h"


class Component
{
protected:
	std::string m_name;
	GameObject *m_owner;
public:

	Component( const char *name, GameObject *owner )
	{
		m_name = name;
		m_owner = owner;
	}

	virtual ~Component()
	{
		m_owner->UnregisterFromAllUpdates( this );
	}

	virtual void Bind( const char *, const char * )
	{
	}

	const char *Name() const
	{
		return m_name.c_str();
	}

	virtual void Enter()
	{
	}

	virtual void Update( int /*idx*/ )
	{
	}

	virtual void Remove()
	{
	}

	template<class T>
	T * FindComponent()
	{
		return m_owner->FindComponent<T>();
	}

	GameObject *Owner()
	{
		return m_owner;
	}
};


class ComponentConstructor
{
	static ComponentConstructor *root;
	const char *name;
	ComponentConstructor *next;

public:

	ComponentConstructor( const char *n )
	{
		name = n;
		next = root;
		root = this;
	}

	virtual Component * Create( const char *name, class GameObject *owner ) = 0;
	static Component *Create( const char *type, const char *name, class GameObject *owner )
	{
		ComponentConstructor *c = root;
		while ( c )
		{
			if ( strcmp( c->name, type ) == 0 )
			{
				return c->Create( name, owner );
			}
			c=c->next;
		}
 		return NULL;
	}

	static void Destroy( Component * c )
	{
		c->~Component();
		_aligned_free( c );
	}
};


template<typename T>
class TypedComponentConstruct : public ComponentConstructor
{
public:

	TypedComponentConstruct( const char *n ) : ComponentConstructor(n)
	{
	}

	virtual Component *Create( const char *name, class GameObject *owner )
	{
		void *mem = _aligned_malloc( sizeof(T), 16 );
		return new(mem) T(name, owner);
	}
};

#define TYPE(c) \
	TypedComponentConstruct<c> g_Constructor_##c(#c);

#define TYPENAME(c,b) \
	TypedComponentConstruct<c> g_Constructor_##c(#b);

#define DECLAREINFO(c,p) \
	public: \
	typedef p PARENT; \
	typedef c THISCLASS; \
	private:



#endif

