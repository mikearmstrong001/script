#ifndef TESTING_LOADER_ATGI_H
#define TESTING_LOADER_ATGI_H

#include <vector>
#include <string>
#include "model_structs.h"

bool LoadAtgi( std::vector< ModelGeometry * > &geom, std::vector< std::string > &materials, std::vector< std::string > &surfaceNames, const char *filename );

#endif

