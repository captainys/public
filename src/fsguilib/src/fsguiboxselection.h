/* ////////////////////////////////////////////////////////////

File Name: fsguiboxselection.h
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

#ifndef FSGUIBOXSELECTION_IS_INCLUDED
#define FSGUIBOXSELECTION_IS_INCLUDED
/* { */


class FsGuiBoxSelection
{
private:
	YSBOOL enabled,dragging;
	int x0,y0,x1,y1;
public:
	FsGuiBoxSelection();

	void SetEnable(YSBOOL e);
	void Enable(void);
	void Disable(void);
	YSBOOL IsEnabled(void) const;

	YSBOOL IsDragging(void) const;
	void LButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	void MouseMove(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Returns YSTRUE if a box-selection has been made. */
	YSBOOL LButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	void GetRect(int &x0,int &y0,int &x1,int &y1) const;
};


/* } */
#endif
