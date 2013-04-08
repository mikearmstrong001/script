#ifndef MLIB_MESHMD5_H
#define MLIB_MESHMD5_H

#include <string>
#include <vector>
#include "faff.h"

struct modeljoint
{
	std::string name;
	int parent;
	Vectormath::Aos::Quat relQ;
	Vectormath::Aos::Point3 relT;
	Vectormath::Aos::Transform3 m;
	Vectormath::Aos::Transform3 im;
	std::string mirror;
	unsigned int mirrorFunc;
};

struct ModelGeometry;

void LoadMD5Mesh( std::vector< ModelGeometry * > &geom, 
				 std::vector< std::string > &materials, 
				 std::vector< std::string > &surfaceNames, 
				 std::vector< modeljoint > &joints, const char *filename );

#endif
