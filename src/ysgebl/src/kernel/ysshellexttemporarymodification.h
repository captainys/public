/* ////////////////////////////////////////////////////////////

File Name: ysshellexttemporarymodification.h
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

#ifndef YSSHELLEXT_TEMPORARYMODIFICATION_IS_INCLUDED
#define YSSHELLEXT_TEMPORARYMODIFICATION_IS_INCLUDED
/* { */

#include "ysshellext.h"

class YsShellExt::TemporaryModification
{
private:
	const YsShellExt *shlPtr;
	YsShellPolygonAttribTable <YsColor> plgCol;

	YSBOOL polygonColorChanged;

public:
	TemporaryModification();
	void CleanUp(void);
	void SetShell(const YsShellExt &shl);

	void ClearChangeFlag(void);
	YSBOOL Changed(void) const;
	YSBOOL PolygonColorChanged(void) const;

	void SetColor(YsShellPolygonHandle plHd,const YsColor col);
	YsColor GetColor(YsShellPolygonHandle plHd) const;

	template <typename SHLCLASS>
	void MakePermanent(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo undoGuard(shl);
		for(auto colHd : plgCol.AllHandle())
		{
			auto plHd=shlPtr->FindPolygon(plgCol.GetKey(colHd));
			auto col=plgCol.GetAttrib(colHd);
			shl.SetPolygonColor(plHd,col);
		}
	}

};

/* } */
#endif
