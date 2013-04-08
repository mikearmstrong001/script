#include "image.h"
#include "../stb/stb_dxt.h"
#include "../math/maths.h"



uint8_t * IMG_Compress( uint8_t *dst, uint8_t *in, int w, int h, int alpha ) {
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


bool IMG_BoxFilter( uint8_t *out, int &nw, int &nh, uint8_t *in, int w, int h, float gamma ) {
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

					avg = Maths::Clamp( (int)(powf( avgi, 1.f/gamma ) * 255.f), 0, 255 );
				} else {
					avg = ((uint32_t)in[ (y0*w+x0)*4 + c ] +
					(uint32_t)in[ (y0*w+x1)*4 + c ] +
					(uint32_t)in[ (y1*w+x1)*4 + c ] +
					(uint32_t)in[ (y1*w+x0)*4 + c ]) / 4;
				}
				out[ (y*nw+x)*4 + c ] = avg;
			}
		}
	}
	return true;
}

int IMG_CalcTotalDXTBlocks( int w, int h ) {
	if ( w == 1 && h == 1 ) {
		return 1;
	}

	int nw = w > 1 ? w/2 : 1;
	int nh = h > 1 ? h/2 : 1;

	return ((w+3)/4) * ((h+3)/4) + IMG_CalcTotalDXTBlocks( nw, nh );
}

int IMG_CalcTotalMipLevels( int w, int h ) {
	int c = 1;
	while ( w != 1 || h != 1 ) {
		w = w > 1 ? w/2 : 1;
		h = h > 1 ? h/2 : 1;
		c++;
	}
	return c;
}

bool IMG_HasAlpha( uint8_t *in, int w, int h ) {
	int tot = w * h;
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

void IMG_GetCubeVector( float* vector, int face, int cubesize, int x, int y) {
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

void IMG_MakeCubeFromHalfSphere( uint8_t *dst[6], int cw, uint8_t *in, int w, int h ) {
	for ( int i = 0; i < 6 ; i++ ) {
		for ( int x = 0; x < cw; x++ ) {
			for ( int y = 0; y < cw; y++ ) {

				float vec[3];
                IMG_GetCubeVector( vec, i, cw, x, y );

				vec[1] = fabsf( vec[1] );

				float theta = atan2f( vec[2], vec[0] ) / PI;
				float phi = acosf( vec[1] ) / PI;

				theta = ( theta * 0.5f + 0.5f ) * ( w - 1 );
				phi = ( phi * 2 ) * ( h - 1 );

				int u = static_cast< int >( floorf( theta ) ) % w;
				int v = static_cast< int >( floorf( phi ) ) % h;

				int indexSphere = ( u + v * w ) * 4;
				int indexCube = ( x + y * cw ) * 4;
                dst[i][indexCube + 0] = in[indexSphere + 0];
				dst[i][indexCube + 1] = in[indexSphere + 1];
				dst[i][indexCube + 2] = in[indexSphere + 2];
				dst[i][indexCube + 3] = in[indexSphere + 3];
			}
		}
	}
}
