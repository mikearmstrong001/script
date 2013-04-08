#ifndef TESTING_RENDERPASS_DEFAULT_H
#define TESTING_RENDERPASS_DEFAULT_H

#include "renderpass.h"

class RenderPassDefault : public RenderPass
{
public:
	~RenderPassDefault() {}


	virtual void Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights);
};

#endif
