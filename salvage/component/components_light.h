#ifndef TESTING_COMPONENTS_LIGHT_H
#define TESTING_COMPONENTS_LIGHT_H

#include "components_basic.h"

class Light : public Component
{
	DECLAREINFO( Light, Component );

protected:

	Vectormath::Aos::Vector4 lightColour;
	float					 lightIntensity;

public:
	Light( const char *name, class GameObject *go ) : PARENT(name, go)
	{
	}

	virtual void Enter();
	virtual void Remove();
	virtual void Bind( const char *tag, const char *data );
	virtual void RenderLight( const Vectormath::Aos::Matrix4 &v ) = 0;
};

#endif
