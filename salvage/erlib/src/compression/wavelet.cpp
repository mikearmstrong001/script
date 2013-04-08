#include "wavelet.h"

void WAVELET_ToHaar1d( int32_t *out, uint8_t *in, int32_t num, int32_t stride ) {
	int32_t *avg = out;
	int32_t *delta = out + (num/2) * stride;

	for (int i=0; i<num; i+=2) {
		uint8_t v0 = in[i*stride];
		uint8_t v1 = in[(i+1)*stride];

		int32_t a = (v0+v1);
		int32_t d = a - (v0<<1);

		avg[(i/2)*stride] = a;
		delta[(i/2)*stride] = d;
	}
}

void WAVELET_ToHaar1d( int32_t *out, int32_t *in, int32_t num, int32_t stride ) {

	int32_t *avg = out;
	int32_t *delta = out + (num/2) * stride;

	for (int i=0; i<num; i+=2) {
		int32_t v0 = in[i*stride];
		int32_t v1 = in[(i+1)*stride];

		int32_t a = (v0+v1);
		int32_t d = a - (v0<<1);

		avg[(i/2)*stride] = a;
		delta[(i/2)*stride] = d;
	}
}


void WAVELET_FromHaar1d( uint8_t *out, int32_t *in, int32_t num, int32_t stride ) {
	int32_t *avg = in;
	int32_t *delta = in + (num/2) * stride;

	for (int i=0; i<num; i+=2) {
		int32_t a = avg[(i/2)*stride];
		int32_t d = delta[(i/2)*stride];

		out[i*stride] = (a-d)>>1;
		out[(i+1)*stride] = (a+d)>>1;
	}
}

void WAVELET_FromHaar1d( int32_t *out, int32_t *in, int32_t num, int32_t stride ) {
	int32_t *avg = in;
	int32_t *delta = in + (num/2) * stride;

	for (int i=0; i<num; i+=2) {
		int32_t a = avg[(i/2)*stride];
		int32_t d = delta[(i/2)*stride];

		out[i*stride] = (a-d)>>1;
		out[(i+1)*stride] = (a+d)>>1;
	}
}

