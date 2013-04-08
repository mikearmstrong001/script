#ifndef ER_HEADER_WAVELET_H
#define ER_HEADER_WAVELET_H

#include "../erbase.h"

void WAVELET_ToHaar1d( int32_t *out, uint8_t *in, int32_t num, int32_t stride );
void WAVELET_ToHaar1d( int32_t *out, int32_t *in, int32_t num, int32_t stride );

void WAVELET_FromHaar1d( int32_t *out, int32_t *in, int32_t num, int32_t stride );
void WAVELET_FromHaar1d( uint8_t *out, int32_t *in, int32_t num, int32_t stride );


#endif


