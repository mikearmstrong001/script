#ifndef TESTING_FONT_MANAGER_H
#define TESTING_FONT_MANAGER_H

#include "font.h"

#include "resource_core.h"

class FontManager : public TypedResourceManager<Font>
{
	virtual Font* CreateEmpty( const char *filename );
	virtual Font* LoadResource( const char *filename );

public:
	FontManager();
};

FontManager *fontManager();

#endif
