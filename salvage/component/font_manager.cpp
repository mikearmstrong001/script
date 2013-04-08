#include "dxut.h"
#include "font_manager.h"


Font* FontManager::CreateEmpty( const char *filename )
{
	return new Font;
}

Font* FontManager::LoadResource( const char *filename )
{
	Font *fnt = new Font;
	fnt->Load( filename );
	return fnt;
}

FontManager::FontManager()
{
}


FontManager *fontManager()
{
	static FontManager mgr;
	return &mgr;
}
