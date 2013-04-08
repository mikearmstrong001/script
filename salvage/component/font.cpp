#include "dxut.h"
#include "font.h"
#include "material_manager.h"
#include "stb_truetype.h"
#include "file_system.h"
#include "model_structs.h"

Font::Font() : fontMaterial(NULL), fontGlyphs(NULL)
{
}

Font::~Font()
{
	Purge();
	delete fontGlyphs;
}

void Font::Purge()
{
	delete fontGlyphs;
	fontGlyphs = NULL;
	fontMaterial = NULL;
}

void Font::Load( const char *filename )
{
	Purge();

	char buffer[512];
	PHYSFS_File *f = PHYSFS_openRead( fs::MakeCanonicalForm( buffer, filename ) );
	if ( f )
	{
		char line[512];
		while ( PHYSFS_gets( line, sizeof(line), f ) )
		{
			stbtt_bakedchar bc;
			int i0, i1;
			char str[128];
			if ( sscanf( line, "material: %s", str ) == 1 )
			{
				fontMaterial = materialManager()->Load( str );
			}
			else if ( sscanf( line, "glyphStart: %d", &i0 ) == 1 )
			{
				baseGlyph = i0;
			}
			else if ( sscanf( line, "glyphCount: %d", &i0 ) == 1 )
			{
				numGlyphs = i0;
				fontGlyphs = new stbtt_bakedchar[ numGlyphs ];
			}
			else if ( sscanf( line, "bakedImageSize: %d %d", &i0, &i1 ) == 2 )
			{
				bakedSize[0] = i0;
				bakedSize[1] = i1;
			}
			else if ( sscanf( line, "%d %d %d %d %d %f %f %f", &i0, &bc.x0, &bc.y0, &bc.x1, &bc.y1,
				&bc.xoff, &bc.yoff, &bc.xadvance ) == 8 )
			{
				fontGlyphs[i0] = bc;
			}
		}
		PHYSFS_close(f);
	}
}

int Font::Print(ModelVert *verts, unsigned short *indices, int baseVert, float *x, float *y, const char *text, float z) const
{
	int num = 0;
	// assume orthographic projection with units = screen pixels, origin at top left
	while (*text) {
	  if (*text >= baseGlyph && *text < (baseGlyph+numGlyphs)) {
		 stbtt_aligned_quad q;
		 stbtt_GetBakedQuad(fontGlyphs, bakedSize[0],bakedSize[1], *text-baseGlyph, x, y, &q, 0);//1=opengl,0=old d3d
		 
		 ModelVert *v0 = verts++;
		 ModelVert *v1 = verts++;
		 ModelVert *v2 = verts++;
		 ModelVert *v3 = verts++;

		 v0->p[0] = q.x0;
		 v0->p[1] = q.y0;
		 v0->p[2] = z;
		 v0->t[0] = q.s0;
		 v0->t[1] = q.t0;

		 v1->p[0] = q.x1;
		 v1->p[1] = q.y0;
		 v1->p[2] = z;
		 v1->t[0] = q.s1;
		 v1->t[1] = q.t0;

 		 v2->p[0] = q.x1;
		 v2->p[1] = q.y1;
		 v2->p[2] = z;
		 v2->t[0] = q.s1;
		 v2->t[1] = q.t1;

 		 v3->p[0] = q.x0;
		 v3->p[1] = q.y1;
		 v3->p[2] = z;
		 v3->t[0] = q.s0;
		 v3->t[1] = q.t1;

		 *indices++ = (unsigned short)(baseVert + 0);
		 *indices++ = (unsigned short)(baseVert + 1);
		 *indices++ = (unsigned short)(baseVert + 2);

		 *indices++ = (unsigned short)(baseVert + 0);
		 *indices++ = (unsigned short)(baseVert + 2);
		 *indices++ = (unsigned short)(baseVert + 3);
		 
		 num++;
		 baseVert += 4;
	  }
	  ++text;
	}
	return num;
}
