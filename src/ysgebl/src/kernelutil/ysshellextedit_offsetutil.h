/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_offsetutil.h
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

#ifndef YSSHELLEXTEDIT_OFFSETUTIL_IS_INCLUDED
#define YSSHELLEXTEDIT_OFFSETUTIL_IS_INCLUDED
/* { */

#include "ysshellext_offsetutil.h"
#include <ysshellextedit.h>

class YsShellExtEdit_OffsetUtil2d : public YsShellExt_OffsetUtil2d
{
public:
	/*! YsShellExt_OffsetUtil2d::SetUpFor* must be called before this function. 
	    newVtxArray[?].toPos, newVtxArray[?].toVtHd will be populated.
	    Polygons in srcPlHdArray will be updated accordingly.  */
	YSRESULT ApplyByRatio(YsShellExtEdit &shl,const double ratioAgainstMaxDist);

	/*! YsShellExt_OffsetUtil2d::SetUpFor* must be called before this function. 
	    newVtxArray[?].toPos, newVtxArray[?].toVtHd will be populated.
	    Polygons in srcPlHdArray will be updated accordingly.  */
	static YSRESULT ApplyByRatio(YsShellExt_OffsetUtil2d &offsetUtil,YsShellExtEdit &shl,const double ratioAgainstMaxDist);
};

/* } */
#endif
