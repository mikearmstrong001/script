#ifndef TESTING_RENDERPASS_LIGHTPREPASS_H
#define TESTING_RENDERPASS_LIGHTPREPASS_H

#include "renderpass.h"

namespace hw
{
	struct DepthStencil;
};

class Texture;

class RenderPassLightPrePass : public RenderPass
{
	Texture *rt0;
public:
	RenderPassLightPrePass();
	virtual void Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights);
};

class RenderPassLightPrePassLights : public RenderPass
{
	Texture *depthNormal;
	Texture *rt0;
public:
	RenderPassLightPrePassLights();
	virtual void Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights);
};

class RenderPassLightPrePassOpaque : public RenderPass
{
	Texture *light;
public:
	RenderPassLightPrePassOpaque();
	virtual void Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights);
};

class RenderPassLightPrePassStereoscopic : public RenderPass
{
	Texture *depthNormal;
	Texture *leftRt0;
	Texture *rightRt0;
public:
	RenderPassLightPrePassStereoscopic();
	virtual void Render(const Vectormath::Aos::Matrix4 &v, const Vectormath::Aos::Matrix4 &p, const std::vector< Light* > &lights);
};

#endif
