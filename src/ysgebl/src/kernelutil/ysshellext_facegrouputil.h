/* ////////////////////////////////////////////////////////////

File Name: ysshellext_facegrouputil.h
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

#ifndef YSSHELLEXT_FACEGROUPUTIL_IS_INCLUDED
#define YSSHELLEXT_FACEGROUPUTIL_IS_INCLUDED
/* { */


#include <ysshellext.h>

#include "ysshellext_trackingutil.h"

class YsShellExt_FaceGroupCandidate
{
public:
	class GroupCandidate
	{
	public:
		YsArray <YsShellPolygonHandle> plHdArray;
	};

private:
	YsArray <GroupCandidate> grpArray;

	class ForcePolygonToBeInFaceGroupTrackingCondition;
public:
	/*! Cleans up. */
	void CleanUp(void);

	/*! Returns an array of face-group candidates. */
	const YsArray <GroupCandidate> &FaceGroupCandidate(void) const;

	/*! Make face group candidates of the polygons that are not yet in a face group.  This function is const-edge sensitive. */
	void ForcePolygonToBeInFaceGroup(const YsShellExt &shl);

};



/*! This function returns YSTRUE if at least one pair of normal vectors of the polygons included in the face group fgHd exceeds the angle threshold, YSFALSE otherwise.
*/
YSBOOL YsShellExt_CheckFaceGroupBending(const YsShellExt &shl,const YsShellExt::FaceGroupHandle fgHd,const double angleThr);



/*! This function returns YSTRUE if removing plHd from the face group causes the face group to be disconnected. 
    YSFALSE otherwise. */
YSBOOL YsShellExt_CheckFaceGroupDisconnectedByRemovingPolygon(const YsShellExt &shl,YsShell::PolygonHandle plHd);


/* } */
#endif
