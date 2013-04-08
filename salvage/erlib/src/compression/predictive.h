#ifndef ER_HEADER_PREDICTIVE_H
#define ER_HEADER_PREDICTIVE_H

#include "../erbase.h"

void IMGPRED_Compress( uint8_t *out, uint8_t *img, int w, int h );
void IMGPRED_Decompress( uint8_t *out, uint8_t *img, int w, int h );

#endif
