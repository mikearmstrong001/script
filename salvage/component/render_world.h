#ifndef TESTING_RENDERWORD_H
#define TESTING_RENDERWORD_H

#include <vector>

class Renderable;
class RenderPass;
class Light;
class Camera;

class RenderWorld
{
	std::vector< Renderable* > m_renderables;
	std::vector< Light* > m_lights;
	std::vector< Camera* > m_cameras;

public:

	RenderWorld();
	virtual ~RenderWorld();

	void RegisterRenderable( Renderable *r );
	void UnregisterRenderable( Renderable *r );

	void RegisterLight( Light *l );
	void UnregisterLight( Light *l );

	void RegisterCamera( Camera *c );
	void UnregisterCamera( Camera *c );

	void RenderFrame( RenderPass **renderPasses );
};

#endif
