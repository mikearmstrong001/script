#ifndef FLOAT_MATH_LIB_H

#define FLOAT_MATH_LIB_H


/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

// a set of routines that let you do common 3d math
// operations without any vector, matrix, or quaternion
// classes or templates.
//
// a vector (or point) is a 'float *' to 3 floating point numbers.
// a matrix is a 'float *' to an array of 16 floating point numbers representing a 4x4 transformation matrix compatible with D3D or OGL
// a quaternion is a 'float *' to 4 floats representing a quaternion x,y,z,w
//
//
//
// Please email bug fixes or improvements to John W. Ratcliff at mailto:jratcliff@infiniplex.net
//
// If you find this source code useful donate a couple of bucks to my kid's fund raising website at
//  www.amillionpixels.us
//
// More snippets at: www.codesuppository.com
//

#include <float.h>

enum FM_ClipState
{
  FMCS_XMIN       = (1<<0),
  FMCS_XMAX       = (1<<1),
  FMCS_YMIN       = (1<<2),
  FMCS_YMAX       = (1<<3),
  FMCS_ZMIN       = (1<<4),
  FMCS_ZMAX       = (1<<5),
};

enum FM_Axis
{
  FM_XAXIS   = (1<<0),
  FM_YAXIS   = (1<<1),
  FM_ZAXIS   = (1<<2)
};

enum LineSegmentType
{
  LS_START,
  LS_MIDDLE,
  LS_END
};


const float FM_PI = 3.1415926535897932384626433832795028841971693993751f;
const float FM_DEG_TO_RAD = ((2.0f * FM_PI) / 360.0f);
const float FM_RAD_TO_DEG = (360.0f / (2.0f * FM_PI));

//***************** Float versions
//***
//*** vectors are assumed to be 3 floats or 3 doubles representing X, Y, Z
//*** quaternions are assumed to be 4 floats or 4 doubles representing X,Y,Z,W
//*** matrices are assumed to be 16 floats or 16 doubles representing a standard D3D or OpenGL style 4x4 matrix
//*** bounding volumes are expressed as two sets of 3 floats/double representing bmin(x,y,z) and bmax(x,y,z)
//*** Plane equations are assumed to be 4 floats or 4 doubles representing Ax,By,Cz,D

FM_Axis fm_getDominantAxis(const float normal[3]);
FM_Axis fm_getDominantAxis(const double normal[3]);

void  fm_identity(float matrix[16]); // set 4x4 matrix to identity.
void  fm_identity(double matrix[16]); // set 4x4 matrix to identity.

void  fm_inverseRT(const float matrix[16], const float pos[3], float t[3]); // inverse rotate translate the point.
void  fm_inverseRT(const double matrix[16],const double pos[3],double t[3]); // inverse rotate translate the point.

void  fm_transform(const float matrix[16], const float pos[3], float t[3]); // rotate and translate this point.
void  fm_transform(const double matrix[16],const double pos[3],double t[3]); // rotate and translate this point.

void  fm_rotate(const float matrix[16],const float pos[3],float t[3]); // only rotate the point by a 4x4 matrix, don't translate.
void  fm_rotate(const double matri[16],const double pos[3],double t[3]); // only rotate the point by a 4x4 matrix, don't translate.

void  fm_eulerToMatrix(float ax,float ay,float az,float matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)
void  fm_eulerToMatrix(double ax,double ay,double az,double matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)

void  fm_getAABB(size_t vcount,const float *points,size_t pstride,float bmin[3],float bmax[3]);
void  fm_getAABB(size_t vcount,const double *points,size_t pstride,double bmin[3],double bmax[3]);

void  fm_getAABBCenter(const float bmin[3],const float bmax[3],float center[3]);
void  fm_getAABBCenter(const double bmin[3],const double bmax[3],double center[3]);

void  fm_eulerToQuat(float x,float y,float z,float quat[4]); // convert euler angles to quaternion.
void  fm_eulerToQuat(double x,double y,double z,double quat[4]); // convert euler angles to quaternion.

void  fm_eulerToQuat(const float euler[3],float quat[4]); // convert euler angles to quaternion. Angles must be radians not degrees!
void  fm_eulerToQuat(const double euler[3],double quat[4]); // convert euler angles to quaternion.

void  fm_scale(float x,float y,float z,float matrix[16]); // apply scale to the matrix.
void  fm_scale(double x,double y,double z,double matrix[16]); // apply scale to the matrix.

void  fm_eulerToQuatDX(float x,float y,float z,float quat[4]); // convert euler angles to quaternion using the fucked up DirectX method
void  fm_eulerToQuatDX(double x,double y,double z,double quat[4]); // convert euler angles to quaternion using the fucked up DirectX method

void  fm_eulerToMatrixDX(float x,float y,float z,float matrix[16]); // convert euler angles to quaternion using the fucked up DirectX method.
void  fm_eulerToMatrixDX(double x,double y,double z,double matrix[16]); // convert euler angles to quaternion using the fucked up DirectX method.

void  fm_quatToMatrix(const float quat[4],float matrix[16]); // convert quaterinion rotation to matrix, translation set to zero.
void  fm_quatToMatrix(const double quat[4],double matrix[16]); // convert quaterinion rotation to matrix, translation set to zero.

void  fm_quatRotate(const float quat[4],const float v[3],float r[3]); // rotate a vector directly by a quaternion.
void  fm_quatRotate(const double quat[4],const double v[3],double r[3]); // rotate a vector directly by a quaternion.

void  fm_getTranslation(const float matrix[16],float t[3]);
void  fm_getTranslation(const double matrix[16],double t[3]);

void  fm_setTranslation(const float *translation,float matrix[16]);
void  fm_setTranslation(const double *translation,double matrix[16]);

void  fm_matrixToQuat(const float matrix[16],float quat[4]); // convert the 3x3 portion of a 4x4 matrix into a quaterion as x,y,z,w
void  fm_matrixToQuat(const double matrix[16],double quat[4]); // convert the 3x3 portion of a 4x4 matrix into a quaterion as x,y,z,w

float fm_sphereVolume(float radius); // return's the volume of a sphere of this radius (4/3 PI * R cubed )
double fm_sphereVolume(double radius); // return's the volume of a sphere of this radius (4/3 PI * R cubed )

float fm_cylinderVolume(float radius,float h);
double fm_cylinderVolume(double radius,double h);

float fm_capsuleVolume(float radius,float h);
double fm_capsuleVolume(double radius,double h);

float fm_distance(const float p1[3],const float p2[3]);
double fm_distance(const double p1[3],const double p2[3]);

float fm_distanceSquared(const float p1[3],const float p2[3]);
double fm_distanceSquared(const double p1[3],const double p2[3]);

float fm_distanceSquaredXZ(const float p1[3],const float p2[3]);
double fm_distanceSquaredXZ(const double p1[3],const double p2[3]);

float fm_computePlane(const float p1[3],const float p2[3],const float p3[3],float *n); // return D
double fm_computePlane(const double p1[3],const double p2[3],const double p3[3],double *n); // return D

float fm_distToPlane(const float plane[4],const float pos[3]); // computes the distance of this point from the plane.
double fm_distToPlane(const double plane[4],const double pos[3]); // computes the distance of this point from the plane.

float fm_dot(const float p1[3],const float p2[3]);
double fm_dot(const double p1[3],const double p2[3]);

void  fm_cross(float cross[3],const float a[3],const float b[3]);
void  fm_cross(double cross[3],const double a[3],const double b[3]);

void  fm_computeNormalVector(float n[3],const float p1[3],const float p2[3]); // as P2-P1 normalized.
void  fm_computeNormalVector(double n[3],const double p1[3],const double p2[3]); // as P2-P1 normalized.

bool  fm_computeWindingOrder(const float p1[3],const float p2[3],const float p3[3]); // returns true if the triangle is clockwise.
bool  fm_computeWindingOrder(const double p1[3],const double p2[3],const double p3[3]); // returns true if the triangle is clockwise.

float  fm_normalize(float n[3]); // normalize this vector and return the distance
double  fm_normalize(double n[3]); // normalize this vector and return the distance

void  fm_matrixMultiply(const float A[16],const float B[16],float dest[16]);
void  fm_matrixMultiply(const double A[16],const double B[16],double dest[16]);

void  fm_composeTransform(const float position[3],const float quat[4],const float scale[3],float matrix[16]);
void  fm_composeTransform(const double position[3],const double quat[4],const double scale[3],double matrix[16]);

float fm_computeArea(const float p1[3],const float p2[3],const float p3[3]);
double fm_computeArea(const double p1[3],const double p2[3],const double p3[3]);

void  fm_lerp(const float p1[3],const float p2[3],float dest[3],float lerpValue);
void  fm_lerp(const double p1[3],const double p2[3],double dest[3],double lerpValue);

bool  fm_insideTriangleXZ(const float test[3],const float p1[3],const float p2[3],const float p3[3]);
bool  fm_insideTriangleXZ(const double test[3],const double p1[3],const double p2[3],const double p3[3]);

bool  fm_insideAABB(const float pos[3],const float bmin[3],const float bmax[3]);
bool  fm_insideAABB(const double pos[3],const double bmin[3],const double bmax[3]);

bool  fm_insideAABB(const float obmin[3],const float obmax[3],const float tbmin[3],const float tbmax[3]); // test if bounding box tbmin/tmbax is fully inside obmin/obmax
bool  fm_insideAABB(const double obmin[3],const double obmax[3],const double tbmin[3],const double tbmax[3]); // test if bounding box tbmin/tmbax is fully inside obmin/obmax

size_t fm_clipTestPoint(const float bmin[3],const float bmax[3],const float pos[3]);
size_t fm_clipTestPoint(const double bmin[3],const double bmax[3],const double pos[3]);

size_t fm_clipTestPointXZ(const float bmin[3],const float bmax[3],const float pos[3]); // only tests X and Z, not Y
size_t fm_clipTestPointXZ(const double bmin[3],const double bmax[3],const double pos[3]); // only tests X and Z, not Y


size_t fm_clipTestAABB(const float bmin[3],const float bmax[3],const float p1[3],const float p2[3],const float p3[3],size_t &andCode);
size_t fm_clipTestAABB(const double bmin[3],const double bmax[3],const double p1[3],const double p2[3],const double p3[3],size_t &andCode);


bool     fm_lineTestAABBXZ(const float p1[3],const float p2[3],const float bmin[3],const float bmax[3],float &time);
bool     fm_lineTestAABBXZ(const double p1[3],const double p2[3],const double bmin[3],const double bmax[3],double &time);

bool     fm_lineTestAABB(const float p1[3],const float p2[3],const float bmin[3],const float bmax[3],float &time);
bool     fm_lineTestAABB(const double p1[3],const double p2[3],const double bmin[3],const double bmax[3],double &time);


void  fm_initMinMax(const float p[3],float bmin[3],float bmax[3]);
void  fm_initMinMax(const double p[3],double bmin[3],double bmax[3]);

void  fm_initMinMax(float bmin[3],float bmax[3]);
void  fm_initMinMax(double bmin[3],double bmax[3]);

void  fm_minmax(const float p[3],float bmin[3],float bmax[3]); // accmulate to a min-max value
void  fm_minmax(const double p[3],double bmin[3],double bmax[3]); // accmulate to a min-max value


float fm_solveX(const float plane[4],float y,float z); // solve for X given this plane equation and the other two components.
double fm_solveX(const double plane[4],double y,double z); // solve for X given this plane equation and the other two components.

float fm_solveY(const float plane[4],float x,float z); // solve for Y given this plane equation and the other two components.
double fm_solveY(const double plane[4],double x,double z); // solve for Y given this plane equation and the other two components.

float fm_solveZ(const float plane[4],float x,float y); // solve for Z given this plane equation and the other two components.
double fm_solveZ(const double plane[4],double x,double y); // solve for Z given this plane equation and the other two components.

bool  fm_computeBestFitPlane(size_t vcount,     // number of input data points
                     const float *points,     // starting address of points array.
                     size_t vstride,    // stride between input points.
                     const float *weights,    // *optional point weighting values.
                     size_t wstride,    // weight stride for each vertex.
                     float plane[4]);

bool  fm_computeBestFitPlane(size_t vcount,     // number of input data points
                     const double *points,     // starting address of points array.
                     size_t vstride,    // stride between input points.
                     const double *weights,    // *optional point weighting values.
                     size_t wstride,    // weight stride for each vertex.
                     double plane[4]);


float  fm_computeBestFitAABB(size_t vcount,const float *points,size_t pstride,float bmin[3],float bmax[3]); // returns the diagonal distance
double fm_computeBestFitAABB(size_t vcount,const double *points,size_t pstride,double bmin[3],double bmax[3]); // returns the diagonal distance

float  fm_computeBestFitSphere(size_t vcount,const float *points,size_t pstride,float center[3]);
double  fm_computeBestFitSphere(size_t vcount,const double *points,size_t pstride,double center[3]);

bool fm_lineSphereIntersect(const float center[3],float radius,const float p1[3],const float p2[3],float intersect[3]);
bool fm_lineSphereIntersect(const double center[3],double radius,const double p1[3],const double p2[3],double intersect[3]);

bool fm_intersectRayAABB(const float bmin[3],const float bmax[3],const float pos[3],const float dir[3],float intersect[3]);
bool fm_intersectLineSegmentAABB(const float bmin[3],const float bmax[3],const float p1[3],const float p2[3],float intersect[3]);

bool fm_lineIntersectsTriangle(const float rayStart[3],const float rayEnd[3],const float p1[3],const float p2[3],const float p3[3],float sect[3]);
bool fm_lineIntersectsTriangle(const double rayStart[3],const double rayEnd[3],const double p1[3],const double p2[3],const double p3[3],double sect[3]);

bool fm_rayIntersectsTriangle(const float origin[3],const float dir[3],const float v0[3],const float v1[3],const float v2[3],float &t);
bool fm_rayIntersectsTriangle(const double origin[3],const double dir[3],const double v0[3],const double v1[3],const double v2[3],double &t);

bool fm_raySphereIntersect(const float center[3],float radius,const float pos[3],const float dir[3],float distance,float intersect[3]);
bool fm_raySphereIntersect(const double center[3],double radius,const double pos[3],const double dir[3],double distance,double intersect[3]);

void fm_catmullRom(float out_vector[3],const float p1[3],const float p2[3],const float p3[3],const float *p4, const float s);
void fm_catmullRom(double out_vector[3],const double p1[3],const double p2[3],const double p3[3],const double *p4, const double s);

bool fm_intersectAABB(const float bmin1[3],const float bmax1[3],const float bmin2[3],const float bmax2[3]);
bool fm_intersectAABB(const double bmin1[3],const double bmax1[3],const double bmin2[3],const double bmax2[3]);


// computes the rotation quaternion to go from unit-vector v0 to unit-vector v1
void fm_rotationArc(const float v0[3],const float v1[3],float quat[4]);
void fm_rotationArc(const double v0[3],const double v1[3],double quat[4]);

float  fm_distancePointLineSegment(const float Point[3],const float LineStart[3],const float LineEnd[3],float intersection[3],LineSegmentType &type);
double fm_distancePointLineSegment(const double Point[3],const double LineStart[3],const double LineEnd[3],double intersection[3],LineSegmentType &type);


bool fm_colinear(const double p1[3],const double p2[3],const double p3[3],double epsilon=0.999);               // true if these three points in a row are co-linear
bool fm_colinear(const float  p1[3],const float  p2[3],const float p3[3],float epsilon=0.999f);

bool fm_colinear(const float a1[3],const float a2[3],const float b1[3],const float b2[3]);  // true if these two line segments are co-linear.
bool fm_colinear(const double a1[3],const double a2[3],const double b1[3],const double b2[3]);  // true if these two line segments are co-linear.

enum IntersectResult
{
  IR_DONT_INTERSECT,
  IR_DO_INTERSECT,
  IR_COINCIDENT,
  IR_PARALLEL,
};

IntersectResult fm_intersectLineSegments2d(const float a1[3], const float a2[3], const float b1[3], const float b2[3], float intersectionPoint[3]);
IntersectResult fm_intersectLineSegments2d(const double a1[3],const double a2[3],const double b1[3],const double b2[3],double intersectionPoint[3]);

IntersectResult fm_intersectLineSegments2dTime(const float a1[3], const float a2[3], const float b1[3], const float b2[3],float &t1,float &t2);
IntersectResult fm_intersectLineSegments2dTime(const double a1[3],const double a2[3],const double b1[3],const double b2[3],double &t1,double &t2);

// Plane-Triangle splitting

enum PlaneTriResult
{
  PTR_ON_PLANE,
  PTR_FRONT,
  PTR_BACK,
  PTR_SPLIT,
};

PlaneTriResult fm_planeTriIntersection(const float plane[4],    // the plane equation in Ax+By+Cz+D format
                                    const float *triangle, // the source triangle.
                                    size_t tstride,  // stride in bytes of the input and output *vertices*
                                    float        epsilon,  // the co-planer epsilon value.
                                    float       *front,    // the triangle in front of the
                                    size_t &fcount,  // number of vertices in the 'front' triangle
                                    float       *back,     // the triangle in back of the plane
                                    size_t &bcount); // the number of vertices in the 'back' triangle.


PlaneTriResult fm_planeTriIntersection(const double plane[4],    // the plane equation in Ax+By+Cz+D format
                                    const double *triangle, // the source triangle.
                                    size_t tstride,  // stride in bytes of the input and output *vertices*
                                    double        epsilon,  // the co-planer epsilon value.
                                    double       *front,    // the triangle in front of the
                                    size_t &fcount,  // number of vertices in the 'front' triangle
                                    double       *back,     // the triangle in back of the plane
                                    size_t &bcount); // the number of vertices in the 'back' triangle.


void fm_intersectPointPlane(const float p1[3],const float p2[3],float *split,const float plane[4]);
void fm_intersectPointPlane(const double p1[3],const double p2[3],double *split,const double plane[4]);

PlaneTriResult fm_getSidePlane(const float p[3],const float plane[4],float epsilon);
PlaneTriResult fm_getSidePlane(const double p[3],const double plane[4],double epsilon);


void fm_computeBestFitOBB(size_t vcount,const float *points,size_t pstride,float *sides,float matrix[16],bool bruteForce=true);
void fm_computeBestFitOBB(size_t vcount,const double *points,size_t pstride,double *sides,double matrix[16],bool bruteForce=true);

void fm_computeBestFitOBB(size_t vcount,const float *points,size_t pstride,float *sides,float pos[3],float quat[4],bool bruteForce=true);
void fm_computeBestFitOBB(size_t vcount,const double *points,size_t pstride,double *sides,double pos[3],double quat[4],bool bruteForce=true);

void fm_computeBestFitABB(size_t vcount,const float *points,size_t pstride,float *sides,float pos[3]);
void fm_computeBestFitABB(size_t vcount,const double *points,size_t pstride,double *sides,double pos[3]);


void fm_planeToMatrix(const float plane[4],float matrix[16]); // convert a plane equation to a 4x4 rotation matrix.  Reference vector is 0,1,0
void fm_planeToQuat(const float plane[4],float quat[4],float pos[3]); // convert a plane equation to a quaternion and translation

void fm_planeToMatrix(const double plane[4],double matrix[16]); // convert a plane equation to a 4x4 rotation matrix
void fm_planeToQuat(const double plane[4],double quat[4],double pos[3]); // convert a plane equation to a quaternion and translation

inline void fm_doubleToFloat3(const double p[3],float t[3]) { t[0] = (float) p[0]; t[1] = (float)p[1]; t[2] = (float)p[2]; };
inline void fm_floatToDouble3(const float p[3],double t[3]) { t[0] = (double)p[0]; t[1] = (double)p[1]; t[2] = (double)p[2]; };


void  fm_eulerMatrix(float ax,float ay,float az,float matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)
void  fm_eulerMatrix(double ax,double ay,double az,double matrix[16]); // convert euler (in radians) to a dest 4x4 matrix (translation set to zero)


float  fm_computeMeshVolume(const float *vertices,size_t tcount,const unsigned int *indices);
double fm_computeMeshVolume(const double *vertices,size_t tcount,const unsigned int *indices);


#define FM_DEFAULT_GRANULARITY 0.001f  // 1 millimeter is the default granularity

class fm_VertexIndex
{
public:
  virtual size_t          getIndex(const float pos[3],bool &newPos) = 0;  // get welded index for this float vector[3]
  virtual size_t          getIndex(const double pos[3],bool &newPos) = 0;  // get welded index for this double vector[3]
  virtual const float *   getVerticesFloat(void) const = 0;
  virtual const double *  getVerticesDouble(void) const = 0;
  virtual const float *   getVertexFloat(size_t index) const = 0;
  virtual const double *  getVertexDouble(size_t index) const = 0;
  virtual size_t          getVcount(void) const = 0;
  virtual bool            isDouble(void) const = 0;
  virtual bool            saveAsObj(const char *fname,unsigned int tcount,unsigned int *indices) = 0;
};

fm_VertexIndex * fm_createVertexIndex(double granularity,bool snapToGrid); // create an indexed vertex system for doubles
fm_VertexIndex * fm_createVertexIndex(float granularity,bool snapToGrid);  // create an indexed vertext system for floats
void             fm_releaseVertexIndex(fm_VertexIndex *vindex);




class fm_LineSegment
{
public:
  fm_LineSegment(void)
  {
    mE1 = mE2 = 0;
  }

  fm_LineSegment(size_t e1,size_t e2)
  {
    mE1 = e1;
    mE2 = e2;
  }

  size_t mE1;
  size_t mE2;
};

// LineSweep *only* supports doublees.  As a geometric operation it needs as much precision as possible.
class fm_LineSweep
{
public:

 virtual fm_LineSegment * performLineSweep(const fm_LineSegment *segments,
                                   size_t icount,
                                   const double *planeEquation,
                                   fm_VertexIndex *pool,
                                   size_t &scount) = 0;


};

fm_LineSweep * fm_createLineSweep(void);
void           fm_releaseLineSweep(fm_LineSweep *sweep);


class fm_Triangulate
{
public:
  virtual const double *       triangulate3d(size_t pcount,const double *points,size_t vstride,size_t &tcount) = 0;
  virtual const float  *       triangulate3d(size_t pcount,const float  *points,size_t vstride,size_t &tcount) = 0;
};

fm_Triangulate * fm_createTriangulate(void);
void             fm_releaseTriangulate(fm_Triangulate *t);


const float * fm_getPoint(const float *points,size_t pstride,size_t index);
const double * fm_getPoint(const double *points,size_t pstride,size_t index);

bool   fm_insideTriangle(float Ax, float Ay,float Bx, float By,float Cx, float Cy,float Px, float Py);
bool   fm_insideTriangle(double Ax, double Ay,double Bx, double By,double Cx, double Cy,double Px, double Py);
float  fm_areaPolygon2d(size_t pcount,const float *points,size_t pstride);
double fm_areaPolygon2d(size_t pcount,const double *points,size_t pstride);

bool  fm_pointInsidePolygon2d(size_t pcount,const float *points,size_t pstride,const float *point,size_t xindex=0,size_t yindex=1);
bool  fm_pointInsidePolygon2d(size_t pcount,const double *points,size_t pstride,const double *point,size_t xindex=0,size_t yindex=1);

size_t fm_consolidatePolygon(size_t pcount,const float *points,size_t pstride,float *dest,float epsilon=0.999f); // collapses co-linear edges.
size_t fm_consolidatePolygon(size_t pcount,const double *points,size_t pstride,double *dest,double epsilon=0.999); // collapses co-linear edges.


bool fm_computeSplitPlane(unsigned int vcount,const double *vertices,unsigned int tcount,const unsigned int *indices,double *plane);
bool fm_computeSplitPlane(unsigned int vcount,const float *vertices,unsigned int tcount,const unsigned int *indices,float *plane);

void fm_nearestPointInTriangle(const float *pos,const float *p1,const float *p2,const float *p3,float *nearest);
void fm_nearestPointInTriangle(const double *pos,const double *p1,const double *p2,const double *p3,double *nearest);

float  fm_areaTriangle(const float *p1,const float *p2,const float *p3);
double fm_areaTriangle(const double *p1,const double *p2,const double *p3);

void fm_subtract(const float *A,const float *B,float *diff); // compute A-B and store the result in 'diff'
void fm_subtract(const double *A,const double *B,double *diff); // compute A-B and store the result in 'diff'

void fm_multiply(float *A,float scaler);
void fm_multiply(double *A,double scaler);

void fm_add(const float *A,const float *B,float *sum);
void fm_add(const double *A,const double *B,double *sum);

void fm_copy3(const float *source,float *dest);
void fm_copy3(const double *source,double *dest);

// re-indexes an indexed triangle mesh but drops unused vertices.  The output_indices can be the same pointer as the input indices.
// the output_vertices can point to the input vertices if you desire.  The output_vertices buffer should be at least the same size
// is the input buffer.  The routine returns the new vertex count after re-indexing.
size_t  fm_copyUniqueVertices(size_t vcount,const float *input_vertices,float *output_vertices,size_t tcount,const size_t *input_indices,size_t *output_indices);
size_t  fm_copyUniqueVertices(size_t vcount,const double *input_vertices,double *output_vertices,size_t tcount,const size_t *input_indices,size_t *output_indices);

bool    fm_isMeshCoplanar(size_t tcount,const size_t *indices,const float *vertices,bool doubleSided); // returns true if this collection of indexed triangles are co-planar!
bool    fm_isMeshCoplanar(size_t tcount,const size_t *indices,const double *vertices,bool doubleSided); // returns true if this collection of indexed triangles are co-planar!

bool    fm_samePlane(const float p1[4],const float p2[4],float normalEpsilon=0.01f,float dEpsilon=0.001f,bool doubleSided=false); // returns true if these two plane equations are identical within an epsilon
bool    fm_samePlane(const double p1[4],const double p2[4],double normalEpsilon=0.01,double dEpsilon=0.001,bool doubleSided=false);

// a utility class that will tesseleate a mesh.
class fm_Tesselate
{
public:
  virtual const size_t * tesselate(fm_VertexIndex *vindex,size_t tcount,const size_t *indices,float longEdge,size_t maxDepth,size_t &outcount) = 0;
};

fm_Tesselate * fm_createTesselate(void);
void           fm_releaseTesselate(fm_Tesselate *t);

#endif
