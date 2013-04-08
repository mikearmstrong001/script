#ifndef TESTING_TEXTURE_H
#define TESTING_TEXTURE_H

#include "resource_core.h"

namespace hw
{
	struct Texture;
};

enum TextureType
{
	TextureType_2d,
	TextureType_Cube,
	TextureType_3d,
};

class Texture : public Resource
{
	hw::Texture *m_hw;
	int m_w;
	int m_h;
	int m_d;
	int m_m;
	int m_tt;
public:
	Texture();
	virtual ~Texture();

	virtual void Purge();

	void CreateRenderTexture( int w, int h );
	void Load( const char *filename );
	
	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}

	hw::Texture *HW() const { return m_hw; }

	int Width() const { return m_w; }
	int Height() const { return m_h; }
	int Depth() const { return m_d; }
	//TextureType Type() const { return m_tt; }
};

#endif
