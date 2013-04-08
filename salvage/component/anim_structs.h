#ifndef TESTING_ANIM_STRUCTS_H
#define TESTING_ANIM_STRUCTS_H

#include <vector>
#include "vectormath/cpp/vectormath_aos.h"

struct animsample
{
	Vectormath::Aos::Quat q;
	Vectormath::Aos::Point3 t;
};

inline animsample operator*( const animsample &a, const animsample &b )
{
	animsample out;
	out.t = a.t + Vectormath::Aos::rotate( b.q, Vectormath::Aos::Vector3( a.t ) );
	out.q = a.q * b.q;
	return out;
}

struct animjoint
{
	std::string name;
	int parent;
	animsample base;
	std::vector< animsample > frames;
};

struct anim
{
	float frameRate;
	std::vector<animjoint> joints;

	Vectormath::Aos::Vector3 locomotion;
};

#endif
