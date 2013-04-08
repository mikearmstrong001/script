#ifndef ER_HEADER_FINDFILES_H
#define ER_HEADER_FINDFILES_H

#include "vector.h"
#include "str.h"

void Findfiles( Vector<Str<char>> &filelist, const char *basedir, const char *wc, bool recursive );

#endif