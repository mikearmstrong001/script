#ifndef TESTING_FONT_H
#define TESTING_FONT_H

#include "resource_core.h"

class Material;
struct stbtt_bakedchar;
struct ModelVert;

class Font : public Resource
{
	const Material *fontMaterial;
	stbtt_bakedchar *fontGlyphs;
	int   baseGlyph;
	int   numGlyphs;
	int   bakedSize[2];

public:

	Font();
	~Font();

	virtual void Purge();

	void Load( const char *filename );

	virtual void Reload()
	{
		Purge();
		Load( Name() );
	}

	int Print(ModelVert *verts, unsigned short *indices, int baseVert, float *x, float *y, const char *text, float z) const;

	const Material *GetMaterial() const
	{
		return fontMaterial;
	}

};


#endif
