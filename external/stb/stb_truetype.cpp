#include "dxut.h"
#include "stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#if 0

unsigned char ttf_buffer[1<<20];
unsigned char temp_bitmap[512*512];
unsigned char temp_bitmap2[512*512*4];

stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs

void my_stbtt_initfont(const char *font, float size, const char *out)
{
	fread(ttf_buffer, 1, 1<<20, fopen(font, "rb"));
	stbtt_BakeFontBitmap(ttf_buffer,0, size, temp_bitmap,512,512, 32,96, cdata); // no guarantee this fits!
	// can free ttf_buffer at this point
	for (int i=0; i<(512*512); i++)
	{
	   temp_bitmap2[i*4+0] = 255;
	   temp_bitmap2[i*4+1] = 255;
	   temp_bitmap2[i*4+2] = 255;
	   temp_bitmap2[i*4+3] = temp_bitmap[i];
	}
	char fileout[128];
	sprintf( fileout, "%s.tga", out );
	stbi_write_tga( fileout, 512, 512, 4, temp_bitmap2 );
	sprintf( fileout, "%s.font", out );
	FILE *f = fopen( fileout, "wb" );
	fprintf( f, "glyphStart: 32\nglyphCount: 96\n" );
	fprintf( f, "bakedImageSize: 512 512\n" );
	fprintf( f, "naturalHeight: %f\n", size );
	for (int i=0; i<96; i++)
	{
		fprintf( f, "%d %d %d %d %d %f %f %f\n", i, (int)cdata[i].x0, (int)cdata[i].y0, (int)cdata[i].x1, (int)cdata[i].y1, 
			cdata[i].xoff, cdata[i].yoff, cdata[i].xadvance );
	}
	fclose(f);
}

class testit
{
public:
	testit()
	{
		my_stbtt_initfont("c:/windows/fonts/times.ttf", 32, "media/fonts/times_32");
		my_stbtt_initfont("c:/windows/fonts/times.ttf", 16, "media/fonts/times_16");
	}
};

testit ti;

#endif