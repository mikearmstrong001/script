#ifndef TESTING_MODEL_STATIC_H
#define TESTING_MODEL_STATIC_H

#include "texture.h"
#include "resource_core.h"

class TextureManager : public TypedResourceManager<Texture>
{
	virtual Texture* CreateEmpty( const char *filename );
	virtual Texture* LoadResource( const char *filename );

public:
	TextureManager();

	Texture *CreateRenderTexture( int w, int h, const char *name );
};

TextureManager *textureManager();


#endif
