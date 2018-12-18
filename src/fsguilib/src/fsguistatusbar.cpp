/* ////////////////////////////////////////////////////////////

File Name: fsguistatusbar.cpp
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

#include "fsguistatusbar.h"

void FsGuiStatusBar::Cell::CacheBitmap(void)
{
	YsBitmap bmp;
	if(YSOK==RenderUnicodeString(bmp,msg,defFgCol,defBgCol))
	{
		bmpCache.SetAndBreakUp(bmp,defBitmapWidthLimit,defBitmapHeightLimit);
	}
	else if(YSOK==RenderAsciiString(bmp,msg,defFgCol,defBgCol))
	{
		bmpCache.SetAndBreakUp(bmp,defBitmapWidthLimit,defBitmapHeightLimit);
	}
	else
	{
		bmp.PrepareBitmap(8,8);
		bmpCache.SetAndBreakUp(bmp,8,8); // At least crash must be prevented.
	}
}

////////////////////////////////////////////////////////////

FsGuiStatusBar::FsGuiStatusBar()
{
	CleanUp();
}

void FsGuiStatusBar::CleanUp(void)
{
	heightInPixel=0;
	columnArray.CleanUp();
	bgColor.SetIntRGBA(180,180,180,255);
	needRedraw=YSTRUE;
}

// FsGuiStatusBar::Show is written in fsguidraw.cpp

void FsGuiStatusBar::Make(int nColumn,int widthInPixel)
{
	int fontWid,fontHei;
	GetUnicodeCharacterBitmapSize(fontWid,fontHei);
	heightInPixel=fontHei+8;
	this->widthInPixel=widthInPixel;
	bgColor=defBgCol;
	columnArray.Set(nColumn,NULL);
	needRedraw=YSTRUE;
}

void FsGuiStatusBar::SetWidth(int widthInPixel)
{
	if(this->widthInPixel!=widthInPixel)
	{
		this->widthInPixel=widthInPixel;
		needRedraw=YSTRUE;
	}
}

const YsColor &FsGuiStatusBar::GetBackgroundColor(void) const
{
	return bgColor;
}

void FsGuiStatusBar::SetBackgroundColor(const YsColor &newBackgroundColor)
{
	if(bgColor!=newBackgroundColor)
	{
		bgColor=newBackgroundColor;
		needRedraw=YSTRUE;
	}
}


void FsGuiStatusBar::ClearStringAll(void)
{
	for(YSSIZE_T clmIdx=0; clmIdx<columnArray.GetN(); ++clmIdx)
	{
		SetString(clmIdx,L"");
	}
	needRedraw=YSTRUE;
}

YSRESULT FsGuiStatusBar::SetString(YSSIZE_T columnId,const wchar_t msg[])
{
	if(YSTRUE==columnArray.IsInRange(columnId))
	{
		if(NULL==msg)
		{
			return SetString(columnId,L"");
		}
		else
		{
			if(0!=YsWString::Strcmp(columnArray[columnId].msg,msg))
			{
				columnArray[columnId].msg=msg;
				columnArray[columnId].CacheBitmap();
				needRedraw=YSTRUE;
			}
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT FsGuiStatusBar::SetString(YSSIZE_T columnId,const char msg[])
{
	if(YSTRUE==columnArray.IsInRange(columnId))
	{
		if(NULL==msg)
		{
			return SetString(columnId,L"");
		}
		else
		{
			YsWString wStr;
			wStr.SetUTF8String(msg);
			return SetString(columnId,wStr);
		}
	}
	return YSERR;
}

const wchar_t *FsGuiStatusBar::GetString(YsWString &msg,YSSIZE_T columnIdx) const
{
	if(YSTRUE==columnArray.IsInRange(columnIdx))
	{
		msg=columnArray[columnIdx].msg;
	}
	else
	{
		msg.Set(L"");
	}
	return msg;
}

const char *FsGuiStatusBar::GetString(YsString &msg,YSSIZE_T columnIdx) const
{
	if(YSTRUE==columnArray.IsInRange(columnIdx))
	{
		msg.EncodeUTF8 <wchar_t> (columnArray[columnIdx].msg);
	}
	else
	{
		msg.Set("");
	}
	return msg;
}

int FsGuiStatusBar::GetHeight(void) const
{
	return heightInPixel;
}

void FsGuiStatusBar::SetNeedRedraw(YSBOOL needRedraw)
{
	this->needRedraw=needRedraw;
}

YSBOOL FsGuiStatusBar::NeedRedraw(void) const
{
	return needRedraw;
}

