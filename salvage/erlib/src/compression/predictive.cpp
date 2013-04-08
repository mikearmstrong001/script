#include "predictive.h"

static bool InImage( int x, int y ) {
	if ( x < 0 || y < 0 ) {
		return false;
	}
	return true;
}

void IMGPRED_Compress( uint8_t *out, uint8_t *img, int w, int h ) {
	uint8_t zero[] = {0,0,0,0};
	uint8_t *o = out;
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			
			uint8_t *NW = InImage(x-1,y-1) ? &img[((y-1)*w+(x-1))*4] : zero;
			uint8_t *W = InImage(x-1,y) ? &img[((y)*w+(x-1))*4] : zero;
			uint8_t *N = InImage(x,y-1) ? &img[((y-1)*w+(x))*4] : zero;
			
			uint8_t *C = InImage(x-1,y-1) ? &img[((y-1)*w+(x-1))*4] : zero;
			uint8_t *A = InImage(x-1,y) ? &img[((y)*w+(x-1))*4] : zero;
			uint8_t *B = InImage(x,y-1) ? &img[((y-1)*w+(x))*4] : zero;
			uint8_t *V = &img[((y)*w+(x))*4];

			for (int c=0; c<4; c++) {
				int32_t grad = (int32_t)N[c] + (int32_t)W[c] - (int32_t)NW[c];
				int32_t avg  = ((int32_t)N[c] + (int32_t)W[c]) / 2;
				int32_t pred = (avg + grad) >> 1;

				if ( C[c] > Max(A[c], B[c]) ) {
					pred = Min(A[c],B[c]);
				} else
				if ( C[c] < Min(A[c], B[c]) ) {
					pred = Max(A[c],B[c]);
				} else {
					pred = (int32_t)A[c] + (int32_t)B[c] - (int32_t)C[c];
				}

				int32_t diff = (int32_t)V[c] - pred;
				int32_t idiff = abs( diff ) & (~1);
				if ( diff < 0 ) {
					idiff |= 1;
				}

				*o++ = idiff;
			}
		}
	}
}


void IMGPRED_Decompress( uint8_t *out, uint8_t *img, int w, int h ) {
	uint8_t zero[] = {0,0,0,0};
	uint8_t *o = out;
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			
			uint8_t *NW = InImage(x-1,y-1) ? &out[((y-1)*w+(x-1))*4] : zero;
			uint8_t *W = InImage(x-1,y) ? &out[((y)*w+(x-1))*4] : zero;
			uint8_t *N = InImage(x,y-1) ? &out[((y-1)*w+(x))*4] : zero;

			uint8_t *C = InImage(x-1,y-1) ? &img[((y-1)*w+(x-1))*4] : zero;
			uint8_t *A = InImage(x-1,y) ? &img[((y)*w+(x-1))*4] : zero;
			uint8_t *B = InImage(x,y-1) ? &img[((y-1)*w+(x))*4] : zero;
			uint8_t *V = &img[((y)*w+(x))*4];

			for (int c=0; c<4; c++) {
				int32_t grad = (int32_t)N[c] + (int32_t)W[c] - (int32_t)NW[c];
				int32_t avg  = ((int32_t)N[c] + (int32_t)W[c]) / 2;
				int32_t pred = (avg + grad) >> 1;

				if ( C[c] > Max(A[c], B[c]) ) {
					pred = Min(A[c],B[c]);
				} else
				if ( C[c] < Min(A[c], B[c]) ) {
					pred = Max(A[c],B[c]);
				} else {
					pred = (int32_t)A[c] + (int32_t)B[c] - (int32_t)C[c];
				}

				int32_t diff = (int32_t)V[c];// - pred;

				if ( diff & 1 ) {
					diff &= (~1);
					diff = -diff;
				}

				*o++ = pred + diff;
			}
		}
	}
}
