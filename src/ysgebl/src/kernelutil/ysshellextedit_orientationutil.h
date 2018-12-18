/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_orientationutil.h
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

#ifndef YSSHELLEXTEDIT_ORIENTATIONUTIL_IS_INCLUDED
#define YSSHELLEXTEDIT_ORIENTATIONUTIL_IS_INCLUDED
/* { */

#include "ysshellext_orientationutil.h"
#include <ysshellextedit.h>

class YsShellExtEdit_OrientationUtil : private YsShellExt_OrientationUtil
{
public:
	YsShellExtEdit_OrientationUtil();
	~YsShellExtEdit_OrientationUtil();

	/*! This function re-calculates the normal of polygon plHd. 
	    The normal will be calculated based on the polygon orientation. 
	    This function does not change the orientation. */
	YSRESULT RecalculateNormal(YsShellExtEdit &shl,YsShellPolygonHandle plHd);

	/*! This function re-calculates the normal of polygon plHd. 
	    The normal will be calculated based on the polygon orientation. 
	    This function does not change the orientation. */
	template <const int N>
	YSRESULT RecalculateNormal(YsShellExtEdit &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray);

	/*! This function recalculates the normal vectors based on the polygon orientation.  
	    It only re-calculates normal vectors and does not change the orientation. */
	YSRESULT RecalculateNormal(YsShellExtEdit &shl);

	/*! This function fixes normal vectors of the polygons of the given shell 'shl' 
	    assuming that the shell is a closed manifold solid. */
	static void FixOrientationOfClosedSolid(YsShellExtEdit &shl);

	/*! This function assumes that the normal vectors assigned to polygons are reliable, and flips a polygon if
	    the orientation is inverted with respect to the assigned normal vector. */
	YSRESULT FixOrientationBasedOnAssignedNormal(YsShellExtEdit &shl);

	/*! This function assumes that the current orientation of the polygons are correct, and
	    re-calculates normal vectors of the polygons based on this assumption. */
	YSRESULT RecalculateNormalFromCurrentOrientation(YsShellExtEdit &shl);

};

template <const int N>
YSRESULT YsShellExtEdit_OrientationUtil::RecalculateNormal(YsShellExtEdit &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	for(YsShellPolygonHandle plHd : plHdArray)
	{
		RecalculateNormal(shl,plHd);
	}
	return YSOK;
}

/* } */
#endif
