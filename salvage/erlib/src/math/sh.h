#ifndef ER_HEADER_SH_H
#define ER_HEADER_SH_H

#include "vec.h"

struct SH {
	vec3 param[9];
};

void SH_Zero( SH &sh );
void SH_AddLight( SH &sh, const vec3 &c, const vec3 &d );
void SH_Sample( vec3 &out, const SH &sh, const vec3 &d );

#endif
