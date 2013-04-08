#ifndef MLIB_IMAGE_H
#define MLIB_IMAGE_H


namespace img
{

typedef unsigned char uint8_t;

uint8_t *	Compress( uint8_t *dst, uint8_t *in, int w, int h, int alpha );

bool		BoxFilter( uint8_t *out, int &nw, int &nh, uint8_t *in, int w, int h, float gamma );

int			CalcTotalDXTBlocks( int w, int h );
int			CalcTotalMipLevels( int w, int h );

bool		HasAlpha( uint8_t *in, int w, int h );
void		MakeAlpha( uint8_t *in, int w, int h );

void		Resize( uint8_t *in, int w, int h, uint8_t *out, int nw, int nh, float gamma=2.2f );

int			RGBAToDXT( uint8_t *out, uint8_t *in, int w, int h, bool hasalpha, float gamma );

void		MakeNormalMap( uint8_t *in, int w, int h );
void		MakeVirtualTexture( const char *outfile, uint8_t *in, int w, int h, float gamma );
};

#endif
