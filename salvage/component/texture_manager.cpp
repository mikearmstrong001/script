#include "texture_manager.h"

Texture* TextureManager::CreateEmpty( const char * )
{
	return new Texture;
}

Texture* TextureManager::LoadResource( const char *filename )
{
	Texture *t = new Texture;
	t->Load( filename );
	return t;
}

Texture *TextureManager::CreateRenderTexture( int w, int h, const char *name )
{
	Texture *t = new Texture;
	t->CreateRenderTexture( w, h );
	t->SetName( name );
	t->SetLoaded( true );
	m_resources[name] = t;
	return t;
}


TextureManager::TextureManager()
{
}


TextureManager *textureManager()
{
	static TextureManager mgr;
	return &mgr;
}
