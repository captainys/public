/* ////////////////////////////////////////////////////////////

File Name: ysglmath.h
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#ifndef YSGLMATH_IS_INCLUDED
#define YSGLMATH_IS_INCLUDED
/* { */

#include "ysglheader.h"

/*! \file */

/* Force Visual C++ to type-mismatching error. */
#pragma warning( error : 4028)
#pragma warning( error : 4047)

#define YSGLPI 3.14159265358979323
#define YSGLTOL 0.00001

#ifdef __cplusplus
extern "C" {
#endif

/*! This function makes a projection matrix.  The matrix created by this function is same as the matrix
    multiplied by gluPerspective function.
    fovy is in radian, NOT in degree.
*/
void YsGLMakePerspectivefv(float mat[16],const double fovy,const double aspect,const double nearz,const double farz);

/*! This function makes a projection matrix that is compatible with glFrustum. */
void YsGLMakeFrustum(GLfloat mat[16],GLfloat left,GLfloat right,GLfloat bottom,GLfloat top,GLfloat nearz,GLfloat farz);

/*! This function makes an orthographic projection matrix.  Ths matrix created by this function is same as the
    matrix multiplied by glOrtho function.
*/
void YsGLMakeOrthographicfv(
    GLfloat mat[16],const double left,const double right,const double bottom,const double top,const double nearz,const double farz);

/*! This function makes an identity matrix.
*/
void YsGLMakeIdentityfv(GLfloat mat[16]);

/*! This function makes a translation matrix.
*/
void YsGLMakeTranslationfv(GLfloat mat[16],const double tx,const double ty,const double tz);

/*! This function makes a scaling matrix.
*/
void YsGLMakeScalingfv(GLfloat mat[16],const double sx,const double sy,const double sz);

/*! This function makes a counter-clockwise rotational matrix in XY plane. (X to the right, Y to the up)
*/
void YsGLMakeXYRotationfv(GLfloat mat[16],const double ang);

/*! This function makes a counter-clockwise rotational matrix in YX plane. (Y to the right, X to the up)
*/
void YsGLMakeYXRotationfv(GLfloat mat[16],const double ang);

/*! This function makes a counter-clockwise rotational matrix in XZ plane. (X to the right, Z to the up)
*/
void YsGLMakeXZRotationfv(GLfloat mat[16],const double ang);

/*! This function makes a counter-clockwise rotational matrix in ZX plane. (Z to the right, X to the up)
*/
void YsGLMakeZXRotationfv(GLfloat mat[16],const double ang);

/*! This function makes a counter-clockwise rotational matrix in YZ plane. (Y to the right, Z to the up)
*/
void YsGLMakeYZRotationfv(GLfloat mat[16],const double ang);

/*! This function makes a counter-clockwise rotational matrix in ZY plane. (Z to the right, Y to the up)
*/
void YsGLMakeZYRotationfv(GLfloat mat[16],const double ang);

/*! This function multiplies two matrices a and b and stores the result in mat.  Mat can be same as a or b.
*/
void YsGLMultMatrixfv(GLfloat mat[16],const GLfloat a[16],const GLfloat b[16]);

/*! This function inverts matrix inMat and stores in outMat.
    It returns 1 if successful, or 0 if failed. */
int YsGLInvertMatrixfv(GLfloat outMat[16],const GLfloat inMat[16]);

/*! This function copies a matrix from src to dst. */
void YsGLCopyMatrixfv(GLfloat dst[16],const GLfloat src[16]);

/*! This function multiplies translation to the matrix. */
void YsGLMultMatrixTranslationfv(GLfloat mat[16],GLfloat tx,GLfloat ty,GLfloat tz);

/*! This function multiplies scaling to the matrix. */
void YsGLMultMatrixScalingfv(GLfloat mat[16],GLfloat sx,GLfloat sy,GLfloat sz);

/*! This function multiplies XY rotation to the matrix.  (Imagine 2D XY plane)  */
void YsGLMultMatrixRotationXYfv(GLfloat mat[16],GLfloat angle);

/*! This function multiplies reverse XY rotation to the matrix. */
void YsGLMultMatrixRotationYXfv(GLfloat mat[16],GLfloat angle);

/*! This function multiplies XZ rotation to the matrix.  */
void YsGLMultMatrixRotationXZfv(GLfloat mat[16],GLfloat angle);

/*! This function multiplies reverse of XZ rotation to the matrix.  */
void YsGLMultMatrixRotationZXfv(GLfloat mat[16],GLfloat angle);

/*! This function multiplies YZ rotation to the matrix.  */
void YsGLMultMatrixRotationYZfv(GLfloat mat[16],GLfloat angle);

/*! This function multiplies reverse of YZ rotation to the matrix.  */
void YsGLMultMatrixRotationZYfv(GLfloat mat[16],GLfloat angle);

/*! This function multiples a matrix and a vector. */
void YsGLMultMatrixVectorfv(GLfloat vecOut[4],const GLfloat mat[16],const GLfloat vecIn[4]);

/*! This function multiples a matrix and a vector. */
void YsGLMultMatrixVectordv(GLdouble vecOut[4],const GLdouble mat[16],const GLdouble vecIn[4]);

/*! This function inverts z.  Or, multiplies Scaleing-Matrix(1.0,1.0,-1.0) from the left.  
    It is useful for converting modelView matrix from the left-hand coordinate as in Direct 3D into the right-hand coordinate as in OpenGL. */
void YsGLInvertZfv(GLfloat mat[16]);

/*! This function inverts z.  Or, multiplies Scaleing-Matrix(1.0,1.0,-1.0) from the left.  
    It is useful for converting modelView matrix from the left-hand coordinate as in Direct 3D into the right-hand coordinate as in OpenGL. */
void YsGLInvertZdv(GLdouble mat[16]);

/*! This function makes a unit vector that is perpendicular to fromVec. */
void YsGLGetArbitraryPerpendicularVectorfv(GLfloat outVec[3],const GLfloat fromVec[3]);

/*! This function calculates a cross product of v1 and v2. */
void YsGLCrossProductfv(GLfloat outVec[3],const GLfloat v1[3],const GLfloat v2[3]);


#ifdef __cplusplus
}
#endif

/* } */
#endif
