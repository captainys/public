/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_copyutil.cpp
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

#include "ysshellextedit_copyutil.h"

YSRESULT YsShellExt_CopyPolygonAttribColorNormal(YsShellExtEdit &outShl,YsShellPolygonHandle outPlHd,const YsShellExtEdit &inShl,YsShellPolygonHandle inPlHd)
{
	YsColor inColor;
	YsVec3 inNormal;
	const YsShellExt::PolygonAttrib *inAttrib=inShl.GetPolygonAttrib(inPlHd);
	inShl.GetColorOfPolygon(inColor,inPlHd);
	inShl.GetNormalOfPolygon(inNormal,inPlHd);

	YsShellExtEdit::StopIncUndo incUndo(outShl);
	outShl.SetPolygonColor(outPlHd,inColor);
	outShl.SetPolygonNormal(outPlHd,inNormal);
	outShl.SetPolygonAttrib(outPlHd,*inAttrib);

	return YSOK;
}

YSRESULT YsShellExt_CopyPolygonAttribColorNormalFaceGroup(YsShellExtEdit &outShl,YsShellPolygonHandle outPlHd,YsShellPolygonHandle inPlHd)
{
	YsShellExtEdit::StopIncUndo incUndo(outShl);

	YsShellExt::FaceGroupHandle fgHd=outShl.FindFaceGroupFromPolygon(inPlHd);
	outShl.DeleteFaceGroupPolygon(outPlHd);
	if(NULL!=fgHd)
	{
		const YsShellPolygonHandle fgPlHd[1]={outPlHd};
		outShl.AddFaceGroupPolygon(fgHd,1,fgPlHd);
	}

	return YsShellExt_CopyPolygonAttribColorNormal(outShl,outPlHd,outShl,inPlHd);
}
