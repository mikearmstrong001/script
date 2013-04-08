#ifndef ER_HEADER_CLIP_H
#define ER_HEADER_CLIP_H

int clipNearPlane0( void* out, int outStride, void* in, int inStride, int numPoints );
int clipPlaneW( void* out, int outStride, void* in, int inStride, int numPoints, int idx );
int clipPlaneNegW( void* out, int outStride, void* in, int inStride, int numPoints, int idx );

int clipPlanesD3D( void* out, int outStride, void* in, int inStride, int numPoints );
int clipPlanes( void* out, int outStride, void* in, int inStride, int numPoints );
int clipPlanesNoNear( void* out, int outStride, void* in, int inStride, int numPoints );

bool clipRequiredNearPlane0( void* in, int inStride, int numPoints );
bool clipRequiredPlaneW( void* in, int inStride, int numPoints, int idx );
bool clipRequiredPlaneNegW( void* in, int inStride, int numPoints, int idx );

#endif
