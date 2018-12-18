/* ////////////////////////////////////////////////////////////

File Name: fsgui3dutil.h
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

#ifndef FSGUI_DUTIL_IS_INCLUDED
#define FSGUI_DUTIL_IS_INCLUDED
/* { */

#include <ysclass.h>

/*! This function calculates a triangle that can be drawn as an arrowhead of a line 
    segment from p1 to p2 that appears to be the size of lengthInPixel and widthInPixel.  */
YSRESULT FsGui3DGetArrowHeadTriangle(
    YsVec3 plg[3],YsVec3 p1,YsVec3 p2,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double lengthInPixel,const double widthInPixel);

/*! This function calculates a vector that will be one pixel from the projected position of point p. */
YSRESULT FsGui3DGetPixelVector(
    YsVec3 &xVec,YsVec3 &yVec,const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz);

/*! This function returns five points that will draw a fixed-size star on the screen. */
YSRESULT FsGui3DGetStarPoint(
    YsVec3 plg[5],const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double iconSize);

/*! This function returns five points that will draw a fixed-size star on the screen. */
YSRESULT FsGui3DGetRectPoint(
    YsVec3 plg[4],const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double iconSize);

/*! This function returns five points that will draw a fixed-size star on the screen.  The length of the array plg must 
    be at least nDiv.  */
YSRESULT FsGui3DGetCirclePoint(
    YsVec3 plg[],int nDiv,const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double iconSize);

/* } */
#endif
