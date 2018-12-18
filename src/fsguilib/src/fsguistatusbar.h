/* ////////////////////////////////////////////////////////////

File Name: fsguistatusbar.h
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

#ifndef FSGUISTATUSBAR_IS_INCLUDED
#define FSGUISTATUSBAR_IS_INCLUDED
/* { */

#include "fsguibase.h"

/*! This class is a status bar, which can be placed at the top and the bottom of the GUI canvas. 
    */
class FsGuiStatusBar : public FsGuiObject
{
private:
	class Cell : public FsGuiObject
	{
	public:
		YsWString msg;
		mutable FsGuiBitmapType bmpCache;
		void CacheBitmap(void);
	};

	int widthInPixel;
	int heightInPixel;
	YsSegmentedArray <Cell,3> columnArray;
	YsColor bgColor;
	mutable YSBOOL needRedraw;
public:
	/*! Default constructor. */
	FsGuiStatusBar();

	/*! This function clears the object.  
	    The status bar must be re-constructed when this object is recycled. */
	void CleanUp(void);

	/*! This function draws the status bar.  y0 is the y coordinate of the top-left corner of the status bar. */
	void Show(int y0) const;

	/*! This function creates empty columns.  */
	void Make(int nColumn,int widthInPixel);

	/*! This function sets the width (in pixels) of the status bar. */
	void SetWidth(int widthInPixel);

	/*! This function returns the background color. */
	const YsColor &GetBackgroundColor(void) const;

	/*! This function changes the background color. */
	void SetBackgroundColor(const YsColor &newBackgroundColor);

	/*! This function clears all messages in the status bar. */
	void ClearStringAll(void);

	/*! This function sets the message for the specified column.  Column index (columnId) is zero based. */
	YSRESULT SetString(YSSIZE_T columnId,const wchar_t msg[]);

	/*! This function sets the message for the specified column.  Column index (columnId) is zero based. */
	YSRESULT SetString(YSSIZE_T columnId,const char msg[]);

	/*! This function copies the string of the specified column (columnIdx) in msg and returns the pointer to msg. */
	const wchar_t *GetString(YsWString &msg,YSSIZE_T columnIdx) const;

	/*! This function copies the string of the specified column (columnIdx) in msg in UTF8 format and returns the pointer to msg. */
	const char *GetString(YsString &msg,YSSIZE_T columnIdx) const;

	/*! This function returns the height in the number of pixels. */
	int GetHeight(void) const;

	/*! This function sets or clears the needRedraw flag. */
	virtual void SetNeedRedraw(YSBOOL needRedraw);

	/*! This function returns the needRedraw flag. */
	virtual YSBOOL NeedRedraw(void) const;
};

/* } */
#endif
