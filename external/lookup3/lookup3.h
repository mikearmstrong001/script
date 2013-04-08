#ifndef ER_HEADER_LOOKUP3_H
#define ER_HEADER_LOOKUP3_H

#include "erbase.h"

void hashlittle2( 
  const void *key,       /* the key to hash */
  size_t      length,    /* length of the key */
  uint32_t   *pc,        /* IN: primary initval, OUT: primary hash */
  uint32_t   *pb);        /* IN: secondary initval, OUT: secondary hash */


#endif