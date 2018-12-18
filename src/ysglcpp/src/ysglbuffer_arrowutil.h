#ifndef YSGLBUFFER_ARROWUTIL_IS_INCLUDED
#define YSGLBUFFER_ARROWUTIL_IS_INCLUDED
/* { */

#include "ysglbuffer.h"

/*!
-----           +
                |\
outRad  +-------+ \
_____   |          \
inRad   |          /
-----   +-------+ /
                |/
                +

                |   |
               headLng
*/

YSRESULT YsGLCreateArrowForGL_TRIANGLES(
    YsGLVertexBuffer &vtxBuf,YsGLNormalBuffer &nomBuf,
    const YsVec3 &p0,const YsVec3 &p1,YSBOOL forwardArrow,YSBOOL backwardArrow,
    const double inRad,const double outRad,const double headLng,int nDiv=16);



/* } */
#endif
