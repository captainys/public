/* ////////////////////////////////////////////////////////////

File Name: ysgebl_gui_foundation_io.cpp
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

#include <ysport.h>


#include "ysgebl_gui_foundation.h"


YSBOOL GeblGuiFoundation::IsSupportedFileType(const wchar_t fn[]) const
{
	YsString fnUtf8;
	fnUtf8.EncodeUTF8 <wchar_t> (fn);
	YSFTYPE fType=YsIdentifyFileType(fnUtf8);
	if(YSFTYPE_SRF==fType ||
	   YSFTYPE_STL==fType ||
	   YSFTYPE_DNM==fType ||
	   YSFTYPE_OBJ==fType ||
	   YSFTYPE_OFF==fType)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

GeblGuiFoundation::SlhdAndErrorCode GeblGuiFoundation::LoadGeneral(const wchar_t fn[],YsShellDnmContainer <YsShellExtEditGui>::Node *parent)
{
	SlhdAndErrorCode returnValue;
	returnValue.slHd=NULL;
	returnValue.errCode=GEBLERROR_NOERROR;

	YsWString wStr(fn);
	auto ext=wStr.GetExtension();

	if(0==ext.STRCMP(L".SRF"))
	{
		YsFileIO::File fp(fn,"r");
		if(nullptr!=fp)
		{
			YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd=shlGrp.CreateShell(parent);

			auto inStream=fp.InStream();
			if(YSOK==newSlHd->LoadSrf(inStream))
			{
				newSlHd->SetFileName(fn);
				returnValue.slHd=newSlHd;
			}
			else
			{
				shlGrp.DeleteShell(newSlHd);
				returnValue.errCode=GEBLERROR_FILE_CORRUPTED;
			}
		}
		else
		{
			returnValue.errCode=GEBLERROR_FILE_NOT_FOUND;
		}
	}
	else if(0==ext.STRCMP(L".STL"))
	{
		YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd=shlGrp.CreateShell(parent);
		auto fnUtf8=wStr.GetUTF8String();
		if(YSOK==newSlHd->LoadStl(fnUtf8))
		{
			newSlHd->SetFileName(fn);
			returnValue.slHd=newSlHd;
		}
		else
		{
			shlGrp.DeleteShell(newSlHd);
			returnValue.errCode=GEBLERROR_FILE_CORRUPTED;
		}
	}
	else if(0==ext.STRCMP(L".DNM"))
	{
		YsFileIO::File fp(fn,"r");
		if(NULL!=fp)
		{
			YsString utf8;
			utf8.EncodeUTF8 <wchar_t> (fn);

			shlGrp.CleanUp();
			slHd=NULL; // 2015/03/27 Was missing and added.  Otherwise, AfterOpenFile tries to delete a shell that has already been deleted.
			auto inStream=fp.InStream();
			if(YSOK==shlGrp.LoadDnm(inStream,parent,utf8))
			{
				returnValue.slHd=shlGrp.GetFirstRootNode();
			}
			else
			{
				returnValue.errCode=GEBLERROR_FILE_CORRUPTED;
			}
		}
	}
	else if(0==ext.STRCMP(L".OBJ"))
	{
		YsFileIO::File fp(fn,"r");
		if(NULL!=fp)
		{
			YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd=shlGrp.CreateShell(parent);

			auto inStream=fp.InStream();
			if(YSOK==newSlHd->LoadObj(inStream))
			{
				newSlHd->SetFileName(fn);
				returnValue.slHd=newSlHd;
			}
			else
			{
				shlGrp.DeleteShell(newSlHd);
				returnValue.errCode=GEBLERROR_FILE_CORRUPTED;
			}
		}
		else
		{
			returnValue.errCode=GEBLERROR_FILE_NOT_FOUND;
		}
	}
	else if(0==ext.STRCMP(L".OFF"))
	{
		YsFileIO::File fp(fn,"r");
		if(NULL!=fp)
		{
			YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd=shlGrp.CreateShell(parent);

			auto inStream=fp.InStream();
			if(YSOK==newSlHd->LoadOff(inStream))
			{
				newSlHd->SetFileName(fn);
				returnValue.slHd=newSlHd;
			}
			else
			{
				shlGrp.DeleteShell(newSlHd);
				returnValue.errCode=GEBLERROR_FILE_CORRUPTED;
			}
		}
		else
		{
			returnValue.errCode=GEBLERROR_FILE_NOT_FOUND;
		}
	}
	else if(0==ext.STRCMP(L".DXF"))
	{
		YsFileIO::File fp(fn,"r");
		if(NULL!=fp)
		{
			YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd=shlGrp.CreateShell(parent);

			auto inStream=fp.InStream();
			if(YSOK==newSlHd->LoadDxf(inStream))
			{
				newSlHd->SetFileName(fn);
				returnValue.slHd=newSlHd;
			}
			else
			{
				shlGrp.DeleteShell(newSlHd);
				returnValue.errCode=GEBLERROR_FILE_CORRUPTED;
			}
		}
		else
		{
			returnValue.errCode=GEBLERROR_FILE_NOT_FOUND;
		}
	}
	else
	{
		returnValue.errCode=GEBLERROR_FILE_NOT_SUPPORTED;
	}

	return returnValue;
}

