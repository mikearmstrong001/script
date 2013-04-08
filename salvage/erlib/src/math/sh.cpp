#include "../erbase.h"
#include "sh.h"
#include "PreethamSH.h"
#include "SunSH.h"

const float fConst1 = 4.f/17.f;
const float fConst2 = 8.f/17.f;
const float fConst3 = 15.f/17.f;
const float fConst4 = 5.f/68.f;
const float fConst5 = 15.f/68.f;


void SH_Zero( SH &sh ) {
	sh.param[0].Zero();
	sh.param[1].Zero();
	sh.param[2].Zero();
	sh.param[3].Zero();
	sh.param[4].Zero();
	sh.param[5].Zero();
	sh.param[6].Zero();
	sh.param[7].Zero();
	sh.param[8].Zero();
}

void SH_AddLight( SH &sh, const vec3 &c, const vec3 &d ) {
	sh.param[0] += c * fConst1;
	sh.param[1] += c * fConst2 * d.x;
	sh.param[2] += c * fConst2 * d.y;
	sh.param[3] += c * fConst2 * d.z;
	sh.param[4] += c * fConst3 * ( d.x * d.z );
	sh.param[5] += c * fConst3 * ( d.z * d.y );
	sh.param[6] += c * fConst3 * ( d.y * d.x );
	sh.param[7] += c * fConst4 * ( 3.f * d.z * d.z - 1.f );
	sh.param[8] += c * fConst5 * ( d.x * d.x - d.y * d.y );
}

void SH_Sample( vec3 &out, const SH &sh, const vec3 &d ) {
	out  = sh.param[0];
	out += sh.param[1] * d.x;
	out += sh.param[2] * d.y;
	out += sh.param[3] * d.z;
	out += sh.param[4] * ( d.x * d.z );
	out += sh.param[5] * ( d.z * d.y );
	out += sh.param[6] * ( d.y * d.x );
	out += sh.param[7] * ( 3.f * d.z * d.z - 1.f );
	out += sh.param[8] * ( d.x * d.x - d.y * d.y );
}



