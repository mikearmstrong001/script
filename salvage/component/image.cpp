#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "image.h"
#include "stb_dxt.h"
#include "vectormath/cpp/vectormath_aos.h"
#include "file_system.h"

namespace img
{

typedef unsigned int uint32_t;

static int Clamp( int v, int mn, int mx ) {
	if ( v < mn ) {
		return mn;
	} else
	if ( v > mx ) {
		return mx;
	}
	return v;
}


uint8_t * Compress( uint8_t *dst, uint8_t *in, int w, int h, int alpha ) {
	int bytesPerBlock = alpha ? (4 * 4) : ((4*4)/2);

	uint8_t block[4][4][4];
	uint8_t *curd = (unsigned char *)dst;
	uint8_t *src = (unsigned char *)in;
	for (int y=0; y<h; y+=4) {
		for (int x=0; x<w; x+=4) {
			memset( block, 0, sizeof(block) );
			for (int i=0; i<4; i++) {
				for (int j=0; j<4; j++) {
					int xp = x+j;
					int yp = y+i;
					if ( xp >= w ) {
						xp = w-1;
					}
					if ( yp >= h ) {
						yp = h-1;
					}
					block[i][j][0] = src[ (yp*w+xp) * 4 + 0 ];
					block[i][j][1] = src[ (yp*w+xp) * 4 + 1 ];
					block[i][j][2] = src[ (yp*w+xp) * 4 + 2 ];
					block[i][j][3] = src[ (yp*w+xp) * 4 + 3 ];
				}
			}

			stb_compress_dxt_block( curd, &block[0][0][0], alpha, 1 );
			curd += bytesPerBlock;
		}
	}
	return curd;
}


bool BoxFilter( uint8_t *out, int &nw, int &nh, uint8_t *in, int w, int h, float gamma ) {
	if ( w == 1 && h == 1 ) {
		return false;
	}
	nw = w > 1 ? w/2 : 1;
	nh = h > 1 ? h/2 : 1;
	if ( out == NULL ) {
		return true;
	}
	int sx = w > 1 ? 2 : 1;
	int sy = h > 1 ? 2 : 1;
	for (int y=0; y<nh; y++) {
		for (int x=0; x<nw; x++ ) {
			int x0 = x*sx;
			int y0 = y*sy;
			int x1 = x0+1;
			int y1 = y0+1;
			if ( x1 >= w ) {
				x1 = w-1;
			}
			if ( y1 >= h ) {
				y1 = h-1;
			}
			for (int c=0; c<4; c++) {
				int avg;
				
				if ( c != 3 ) {
					float i0 = powf( in[ (y0*w+x0)*4 + c ]/255.f, gamma );
					float i1 = powf( in[ (y0*w+x1)*4 + c ]/255.f, gamma );
					float i2 = powf( in[ (y1*w+x1)*4 + c ]/255.f, gamma );
					float i3 = powf( in[ (y1*w+x0)*4 + c ]/255.f, gamma );
					float avgi = (i0+i1+i2+i3) * 0.25f;

					avg = Clamp( (int)(powf( avgi, 1.f/gamma ) * 255.f), 0, 255 );
				} else {
					avg = ((uint32_t)in[ (y0*w+x0)*4 + c ] +
					(uint32_t)in[ (y0*w+x1)*4 + c ] +
					(uint32_t)in[ (y1*w+x1)*4 + c ] +
					(uint32_t)in[ (y1*w+x0)*4 + c ]) / 4;
				}
				out[ (y*nw+x)*4 + c ] = (uint8_t)avg;
			}
		}
	}
	return true;
}

int CalcTotalDXTBlocks( int w, int h ) {
	if ( w == 1 && h == 1 ) {
		return 1;
	}

	int nw = w > 1 ? w/2 : 1;
	int nh = h > 1 ? h/2 : 1;

	return ((w+3)/4) * ((h+3)/4) + CalcTotalDXTBlocks( nw, nh );
}

int CalcTotalMipLevels( int w, int h ) {
	int c = 1;
	while ( w != 1 || h != 1 ) {
		w = w > 1 ? w/2 : 1;
		h = h > 1 ? h/2 : 1;
		c++;
	}
	return c;
}

bool HasAlpha( uint8_t *in, int w, int h ) {
	int tot = w * h * 4;
	for (int i=0; i<tot; i+=4) {
		if ( in[i+3] != 0xff ) {
			return true;
		}
	}
	return false;
}

#define IMG_CUBEMAP_FACE_POSX 0
#define IMG_CUBEMAP_FACE_NEGX 1
#define IMG_CUBEMAP_FACE_POSY 2
#define IMG_CUBEMAP_FACE_NEGY 3
#define IMG_CUBEMAP_FACE_POSZ 4
#define IMG_CUBEMAP_FACE_NEGZ 5

void GetCubeVector( float* vector, int face, int cubesize, int x, int y) {
	float s, t, sc, tc, mag;

	s = ( (float)x + 0.5f ) / (float)cubesize;
	t = ( (float)y + 0.5f ) / (float)cubesize;
	sc = s * 2.0f - 1.0f;
	tc = t * 2.0f - 1.0f;

	switch ( face ) {
	case IMG_CUBEMAP_FACE_POSX:
		vector[0] = 1.0;
		vector[1] = -tc;
		vector[2] = -sc;
		break;
	case IMG_CUBEMAP_FACE_NEGX:
		vector[0] = -1.0;
		vector[1] = -tc;
		vector[2] = sc;
		break;
	case IMG_CUBEMAP_FACE_POSY:
		vector[0] = sc;
		vector[1] = 1.0;
		vector[2] = tc;
		break;
	case IMG_CUBEMAP_FACE_NEGY:
		vector[0] = sc;
		vector[1] = -1.0;
		vector[2] = -tc;
		break;
	case IMG_CUBEMAP_FACE_POSZ:
		vector[0] = sc;
		vector[1] = -tc;
		vector[2] = 1.0;
		break;
	case IMG_CUBEMAP_FACE_NEGZ:
		vector[0] = -sc;
		vector[1] = -tc;
		vector[2] = -1.0f;
		break;
	}

	mag = 1.f / sqrtf( vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2] );
	vector[0] *= mag;
	vector[1] *= mag;
	vector[2] *= mag;
}


void		MakeAlpha( uint8_t *in, int w, int h ) {
	int tot = w * h * 4;
	for (int i=0; i<tot; i+=4) {
		int a = 0;
		if ( in[i+0] || in[i+1] || in[i+2] ) {
			a = 0xff;
		}
		in[i+3] = (uint8_t)a;
	}
}

void MakeNormalMap( uint8_t *in, int w, int h ) 
{
	//hx = h(x+1) - h(x-1)
	// ...
	// (-hx, -hy, 1)
	uint8_t *heightmap = (uint8_t*)malloc( w * h * 4 );
	memcpy( heightmap, in, w*h*4 );
	int stride = w * 4;
	for (int y=0; y<h; y++) 
	{
		int prevy = (y+h) % h;
		int nexty = (y+1) % h;
		for (int x=0; x<w; x++)
		{
			int prevx = (x+w) % w;
			int nextx = (x+1) % w;

			float hx = (heightmap[y*stride+nextx*4] - heightmap[y*stride+prevx*4])/512.f;
			float hy = (heightmap[nexty*stride+x*4] - heightmap[prevy*stride+x*4])/512.f;
			
			Vectormath::Aos::Vector3 nmap = Vectormath::Aos::normalize( Vectormath::Aos::Vector3( -hx, -hy, 1.f ) );
			int outidx = y*stride+x*4;
			in[outidx+0] = nmap.getX() * 128.f + 127.f;
			in[outidx+1] = nmap.getY() * 128.f + 127.f;
			in[outidx+2] = nmap.getZ() * 128.f + 127.f;
			in[outidx+3] = 255;
		}
	}
	free( heightmap );
}

void	Resize( uint8_t *in, int w, int h, uint8_t *out, int nw, int nh, float gamma ) {

	float *work = (float*)malloc( nw * h * 4 * sizeof(float) );
	for (int y=0; y<h; y++) {
		for (int x=0; x<nw; x++) {
			if ( nw < w ) {
				int ox = (int)floorf(((x-0.5f)*(w-1))/(nw-1));
				int nox = (int)ceilf(((x+0.5f)*(w-1))/(nw-1));

				float avg[4] = { 0, 0, 0, 0 };
				int c = 0;
				for (int i=ox; i<nox; i++) {
					if ( i < 0 || i >= w ) {
						continue;
					}
					c++;
					avg[0] += powf( in[(i+y*w)*4+0]/255.f, gamma );
					avg[1] += powf( in[(i+y*w)*4+1]/255.f, gamma );
					avg[2] += powf( in[(i+y*w)*4+2]/255.f, gamma );
					avg[3] += powf( in[(i+y*w)*4+3]/255.f, gamma );
				}
				if ( c ) {
					avg[0] /= c;
					avg[1] /= c;
					avg[2] /= c;
					avg[3] /= c;
				}

				work[(x+y*nw)*4+0] = avg[0];
				work[(x+y*nw)*4+1] = avg[1];
				work[(x+y*nw)*4+2] = avg[2];
				work[(x+y*nw)*4+3] = avg[3];
			} else {
				int ox = (int)((x)*(w-1))/(nw-1);
				float f = ((((float)x)*(w-1))/(nw-1)) - ox;
				int nox = (ox+1) == w ? ox : ox+1;
				work[(x+y*nw)*4+0] = (uint8_t)((1-f)*powf( in[(ox+y*w)*4+0]/255.f, gamma ) + f*powf( in[((nox)+y*w)*4+0]/255.f, gamma ));
				work[(x+y*nw)*4+1] = (uint8_t)((1-f)*powf( in[(ox+y*w)*4+1]/255.f, gamma ) + f*powf( in[((nox)+y*w)*4+1]/255.f, gamma ));
				work[(x+y*nw)*4+2] = (uint8_t)((1-f)*powf( in[(ox+y*w)*4+2]/255.f, gamma ) + f*powf( in[((nox)+y*w)*4+2]/255.f, gamma ));
				work[(x+y*nw)*4+3] = (uint8_t)((1-f)*powf( in[(ox+y*w)*4+3]/255.f, gamma ) + f*powf( in[((nox)+y*w)*4+3]/255.f, gamma ));
			}
		}
	}

	for (int y=0; y<nh; y++) {
		for (int x=0; x<nw; x++) {
			if ( nh < h ) {
				int oy = (int)floorf(((y-0.5f)*(h-1))/(nh-1));
				int noy = (int)ceilf(((y+0.5f)*(h-1))/(nh-1));

				float avg[4] = { 0, 0, 0, 0 };
				int c = 0;
				for (int i=oy; i<noy; i++) {
					if ( i < 0 || i >= h ) {
						continue;
					}
					c++;
					avg[0] += work[(x+i*nw)*4+0];
					avg[1] += work[(x+i*nw)*4+1];
					avg[2] += work[(x+i*nw)*4+2];
					avg[3] += work[(x+i*nw)*4+3];
				}
				if ( c ) {
					avg[0] /= c;
					avg[1] /= c;
					avg[2] /= c;
					avg[3] /= c;
				}

				out[(x+y*nw)*4+0] = (uint8_t)powf( avg[0], 1.f/gamma );
				out[(x+y*nw)*4+1] = (uint8_t)powf( avg[1], 1.f/gamma );
				out[(x+y*nw)*4+2] = (uint8_t)powf( avg[2], 1.f/gamma );
				out[(x+y*nw)*4+3] = (uint8_t)powf( avg[3], 1.f/gamma );
			} else {
				int oy = (int)((y)*(h-1))/(nh-1);
				float f = ((((float)y)*(h-1))/(nh-1)) - oy;
				int noy = (oy+1) == h ? oy : oy+1;
				out[(x+y*nw)*4+0] = (uint8_t)powf( ((1-f)*work[(x+oy*nw)*4+0] + f*work[(x+(noy)*nw)*4+0]), 1.f/gamma );
				out[(x+y*nw)*4+1] = (uint8_t)powf( ((1-f)*work[(x+oy*nw)*4+1] + f*work[(x+(noy)*nw)*4+1]), 1.f/gamma );
				out[(x+y*nw)*4+2] = (uint8_t)powf( ((1-f)*work[(x+oy*nw)*4+2] + f*work[(x+(noy)*nw)*4+2]), 1.f/gamma );
				out[(x+y*nw)*4+3] = (uint8_t)powf( ((1-f)*work[(x+oy*nw)*4+3] + f*work[(x+(noy)*nw)*4+3]), 1.f/gamma );
			}
		}
	}
	free( work );
}

static inline int Max(int a,int b) { return a > b ? a : b; }

void MakeVirtualTexture( const char *outfile, uint8_t *in, int w, int h, float gamma )
{
	PHYSFS_File *out = PHYSFS_openWrite( outfile );
	if ( out == NULL )
	{
		return;
	}

	//1 2 4 8 16 32 64 128 256 512 1024 2048 4096
	int size = 4096;//Max( Max( w, h ), 4096 );
	int numMips = 6; // down to 128
	uint8_t *work = (uint8_t*)malloc( size * size * 4 );
	Resize( in, w, h, work, size, size, gamma );
	int tile = size / 128;

	uint8_t *tileWork = (uint8_t *)malloc( (128+4+4) * (128+4+4) * 4 );
	uint8_t *tileComp = (uint8_t *)malloc( (128+4+4) * (128+4+4) * 1 );
	while ( numMips-- )
	{
		for ( int ty=0; ty<tile; ty++ )
		{
			for ( int tx=0; tx<tile; tx++ )
			{
				for ( int y=-4; y<(128+4); y++)
				{
					for ( int x=-4; x<(128+4); x++)
					{
						int outofs = ((y+4)*(128+4+4)+(x+4))*4;
						int inx = (tx*128+x+size) % size;
						int iny = (ty*128+y+size) % size;
						int inofs = (iny*size+inx)*4;
						tileWork[ outofs+0 ] = work[ inofs+0 ];
						tileWork[ outofs+1 ] = work[ inofs+1 ];
						tileWork[ outofs+2 ] = work[ inofs+2 ];
						tileWork[ outofs+3 ] = work[ inofs+3 ];
					}
				}
				Compress( tileComp, tileWork, 128+8, 128+8, 1 );
				PHYSFS_write( out, tileComp, (128+8)*(128+8), 1 );
			}
		}
		int nsize = size / 2;
		uint8_t* mipwork = (uint8_t*)malloc( nsize * nsize * 4 );
		BoxFilter( mipwork, nsize, nsize, work, size, size, gamma );
		free( work );
		work = mipwork;
		size = nsize;
		tile = size / 128;
	}
	free( work );
	free( tileComp );
	free( tileWork );
	PHYSFS_close( out );
}

int AlignUp( int v, int a )
{
	return ((v+(a-1))&~(a-1));
}

int RGBAToDXT( uint8_t *out, uint8_t *in, int w, int h, bool hasalpha, float gamma ) {
	
	int divide = hasalpha ? 1 : 2;

	uint32_t numlevels = CalcTotalMipLevels( w, h );

	uint8_t *work = (uint8_t*)malloc( (AlignUp(w,4) * AlignUp(h,4)) / divide );

	uint8_t *input = in;
	uint8_t *output = out;
	for (uint32_t level=0; level<numlevels; level++) {
		Compress( work, input, w, h, hasalpha );

		if ( output != NULL ) {
			memcpy( out, work, (AlignUp(w,4) * AlignUp(h,4)) / divide );
		}
		out += (AlignUp(w,4) * AlignUp(h,4)) / divide;

		int nw, nh;
		if ( BoxFilter( NULL, nw, nh, input, w, h, gamma ) ) {
			uint8_t *mipimg = (uint8_t *)malloc( nw * nh * 4 );
			BoxFilter( mipimg, w, h, input, w, h, gamma );
			if ( input != in ) {
				free( input );
			}
			input = mipimg;
		} else {
			if ( input != in ) {
				free( input );
			}
			break;
		}
	}
	free( work );

	return out - output;
}

};