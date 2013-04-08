#ifndef ER_HEADER_IMAGE_H
#define ER_HEADER_IMAGE_H

#include "../erbase.h"

uint8_t *	IMG_Compress( uint8_t *dst, uint8_t *in, int w, int h, int alpha );
bool		IMG_BoxFilter( uint8_t *out, int &nw, int &nh, uint8_t *in, int w, int h, float gamma );
int			IMG_CalcTotalDXTBlocks( int w, int h );
int			IMG_CalcTotalMipLevels( int w, int h );
bool		IMG_HasAlpha( uint8_t *in, int w, int h );

void		IMG_MakeCubeFromHalfSphere( uint8_t *dst[6], int cw, uint8_t *in, int w, int h );

#endif
