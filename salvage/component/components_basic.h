#ifndef TESTING_COMPONENTS_BASIC_H
#define TESTING_COMPONENTS_BASIC_H

#include <vector>
#include "components_core.h"
#include "vectormath/cpp/vectormath_aos.h"

class GameObject;

class CTransform : public Component
{
	DECLAREINFO( CTransform, Component )

	Vectormath::Aos::Transform3 m_local;
	Vectormath::Aos::Transform3 m_localToWorld;

	GameObject *m_parent;
	std::vector< GameObject * > m_kids;

	void setParent( GameObject *p )
	{
		m_parent = p;
	}

public:

	CTransform( const char *name, class GameObject *go );

	virtual void Bind( const char *tag, const char *data );

	void AddChild( GameObject *go )
	{
		CTransform *t = go->FindComponent<CTransform>();
		t->setParent( m_owner );
		m_kids.push_back( go );
	}

	const Vectormath::Aos::Transform3 &local() const
	{
		return m_local;
	}

	const Vectormath::Aos::Transform3 &world() const
	{
		return m_localToWorld;
	}

	void setLocal( const Vectormath::Aos::Transform3 &l )
	{
		m_local = l;
	}

	void setWorld( const Vectormath::Aos::Transform3 &l )
	{
		m_localToWorld = l;
	}

	void setWorldTranslation( const Vectormath::Aos::Vector3 &p )
	{
		m_localToWorld.setTranslation( p );
	}
	void setWorldRotation( const Vectormath::Aos::Matrix3 &r )
	{
		m_localToWorld.setUpper3x3( r );
	}

	void setLocalTranslation( const Vectormath::Aos::Vector3 &p )
	{
		m_local.setTranslation( p );
	}
	void setLocalRotation( const Vectormath::Aos::Matrix3 &r )
	{
		m_local.setUpper3x3( r );
	}

	GameObject *Parent() const
	{
		return m_parent;
	}

	const std::vector< GameObject * > &Kids() const
	{
		return m_kids;
	}

	virtual void Enter();
	virtual void Update();

	void UpdateKids();

};


class Renderable : public Component
{
	DECLAREINFO( Renderable, Component );
public:
	Renderable( const char *name, class GameObject *go ) : PARENT(name, go)
	{
	}

	virtual void Enter();
	virtual void Remove();

	virtual int AddRenderCommands( struct RenderCommand *cmd ) = 0;
};


class Camera : public Component
{
	DECLAREINFO( Camera, Component );
	float m_fov;
	float m_znear;
	float m_zfar;
	int   m_priority;
	bool  m_clearColour;
	bool  m_clearDepth;

public:
	Camera( const char *name, class GameObject *go ) : m_fov(80.f), m_znear(0.1f), m_zfar(1000.f), m_priority(32), 
		m_clearColour(true), m_clearDepth(true), PARENT(name, go)
	{
	}

	virtual void Enter();
	virtual void Remove();

	virtual void Bind( const char *tag, const char *data );

	float Fov() const { return m_fov; }
	float ZNear() const { return m_znear; }
	float ZFar() const { return m_zfar; }
};


#endif
