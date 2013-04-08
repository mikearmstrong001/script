#include "sphere.h"

Sphere SPHERE_MinSphere( const vec3 &rkP0, const vec3 &rkP1, const vec3 &rkP2, const vec3 &rkP3 ) {
    // Compute the sphere containing p0, p1, p2, and p3.  The Center in
    // barycentric coordinates is K = u0*p0+u1*p1+u2*p2+u3*p3 where
    // u0+u1+u2+u3=1.  The Center is equidistant from the three points, so
    // |K-p0| = |K-p1| = |K-p2| = |K-p3| = R where R is the radius of the
    // sphere.
    //
    // From these conditions,
    //   K-p0 = u0*A + u1*B + u2*C - A
    //   K-p1 = u0*A + u1*B + u2*C - B
    //   K-p2 = u0*A + u1*B + u2*C - C
    //   K-p3 = u0*A + u1*B + u2*C
    // where A = p0-p3, B = p1-p3, and C = p2-p3 which leads to
    //   r^2 = |u0*A+u1*B+u2*C|^2 - 2*Dot(A,u0*A+u1*B+u2*C) + |A|^2
    //   r^2 = |u0*A+u1*B+u2*C|^2 - 2*Dot(B,u0*A+u1*B+u2*C) + |B|^2
    //   r^2 = |u0*A+u1*B+u2*C|^2 - 2*Dot(C,u0*A+u1*B+u2*C) + |C|^2
    //   r^2 = |u0*A+u1*B+u2*C|^2
    // Subtracting the last equation from the first three and writing
    // the equations as a linear system,
    //
    // +-                          -++   -+       +-        -+
    // | Dot(A,A) Dot(A,B) Dot(A,C) || u0 | = 0.5 | Dot(A,A) |
    // | Dot(B,A) Dot(B,B) Dot(B,C) || u1 |       | Dot(B,B) |
    // | Dot(C,A) Dot(C,B) Dot(C,C) || u2 |       | Dot(C,C) |
    // +-                          -++   -+       +-        -+
    //
    // The following code solves this system for u0, u1, and u2, then
    // evaluates the fourth equation in r^2 to obtain r.

    vec3 kE10 = rkP0 - rkP3;
    vec3 kE20 = rkP1 - rkP3;
    vec3 kE30 = rkP2 - rkP3;

    float aafA[3][3];
    aafA[0][0] = kE10.Dot(kE10);
    aafA[0][1] = kE10.Dot(kE20);
    aafA[0][2] = kE10.Dot(kE30);
    aafA[1][0] = aafA[0][1];
    aafA[1][1] = kE20.Dot(kE20);
    aafA[1][2] = kE20.Dot(kE30);
    aafA[2][0] = aafA[0][2];
    aafA[2][1] = aafA[1][2];
    aafA[2][2] = kE30.Dot(kE30);

    float afB[3];
    afB[0] = ((float)0.5)*aafA[0][0];
    afB[1] = ((float)0.5)*aafA[1][1];
    afB[2] = ((float)0.5)*aafA[2][2];

    float aafAInv[3][3];
    aafAInv[0][0] = aafA[1][1]*aafA[2][2]-aafA[1][2]*aafA[2][1];
    aafAInv[0][1] = aafA[0][2]*aafA[2][1]-aafA[0][1]*aafA[2][2];
    aafAInv[0][2] = aafA[0][1]*aafA[1][2]-aafA[0][2]*aafA[1][1];
    aafAInv[1][0] = aafA[1][2]*aafA[2][0]-aafA[1][0]*aafA[2][2];
    aafAInv[1][1] = aafA[0][0]*aafA[2][2]-aafA[0][2]*aafA[2][0];
    aafAInv[1][2] = aafA[0][2]*aafA[1][0]-aafA[0][0]*aafA[1][2];
    aafAInv[2][0] = aafA[1][0]*aafA[2][1]-aafA[1][1]*aafA[2][0];
    aafAInv[2][1] = aafA[0][1]*aafA[2][0]-aafA[0][0]*aafA[2][1];
    aafAInv[2][2] = aafA[0][0]*aafA[1][1]-aafA[0][1]*aafA[1][0];
    float fDet = aafA[0][0]*aafAInv[0][0] + aafA[0][1]*aafAInv[1][0] +
        aafA[0][2]*aafAInv[2][0];

    Sphere kMinimal;

    if (fabsf(fDet) > 0.0001f)
    {
        float fInvDet = ((float)1.0)/fDet;
        int iRow, iCol;
        for (iRow = 0; iRow < 3; iRow++)
        {
            for (iCol = 0; iCol < 3; iCol++)
            {
                aafAInv[iRow][iCol] *= fInvDet;
            }
        }
        
        float afU[4];
        for (iRow = 0; iRow < 3; iRow++)
        {
            afU[iRow] = 0.0f;
            for (iCol = 0; iCol < 3; iCol++)
            {
                afU[iRow] += aafAInv[iRow][iCol]*afB[iCol];
            }
        }
        afU[3] = (float)1.0 - afU[0] - afU[1] - afU[2];
        
        kMinimal.centre = afU[0]*rkP0 + afU[1]*rkP1 + afU[2]*rkP2 +
            afU[3]*rkP3;
        vec3 kTmp = afU[0]*kE10 + afU[1]*kE20 + afU[2]*kE30;
		kMinimal.radius = kTmp.Length();
    }
    else
    {
        kMinimal.centre.Zero();
        kMinimal.radius = 0.f;
    }

    return kMinimal;
}
