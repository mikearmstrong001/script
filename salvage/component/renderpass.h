#ifndef TESTING_RENDERPASS_H
#define TESTING_RENDERPASS_H

#include <vector>
#include "vectormath/cpp/vectormath_aos.h"

class Light;
struct RenderCommand;

class RenderPass
{
protected:
	std::vector< RenderCommand* > renderables;
public:

	virtual ~RenderPass() {}

	void Empty()
	{
		renderables.clear();
	}

	virtual void Render( const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights ) = 0;

	inline void AddRenderCommand( struct RenderCommand *rc )
	{
		renderables.push_back( rc );
	}
};

#endif
