/* ////////////////////////////////////////////////////////////

File Name: fsgui3dmain.cpp
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

#include <fslazywindow.h>
#include <fssimplewindow.h>
#include <ysdebug.h>
#include <ysport.h>

// Comment out FSGUI3D_USE_SYSTEM_FONT to remove depencency on YsSystemFont library.
#define FSGUI3D_USE_SYSTEM_FONT

// Comment out FSGUI3D_USE_MODERN_UI to use primitive graphics, which may be faster.
#define FSGUI3D_USE_MODERN_UI



#ifdef FSGUI3D_USE_SYSTEM_FONT
#include <ysfontrenderer.h>
#endif

#include "ysgebl_gui_editor_base.h"
#include "filename.h"
#include "config/polycreconfig.h"
#include "ysgebl_gui_extension_base.h"

// Modify this program if the initial window location needs to be changed.



class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;
	YsSystemFontRenderer *sysFont;
	class GeblGuiEditorBase *canvasPtr;

public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &opt) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
	virtual bool StayIdleUntilEvent(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=96;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=640;

	auto *cfg=PolyCreConfig::Create();
	auto *fileAndDirName=PolyCreFileName::Create();

	cfg->Load(fileAndDirName->GetOptionFileName());
	if(YSTRUE==cfg->saveWindowPositionAndSize)
	{
		auto wposFn=fileAndDirName->GetLastWindowPositionFileName();

		FILE *fp=YsFileIO::Fopen(wposFn,"r");
		if(NULL!=fp)
		{
			YsString str;
			while(NULL!=str.Fgets(fp))
			{
				YsArray <YsString> args;
				str.Arguments(args);
				if(0==strcmp(args[0],"POS") && 5<=args.GetN())
				{
					opt.x0=atoi(args[1]);
					opt.y0=atoi(args[2]);
					opt.wid=atoi(args[3]);
					opt.hei=atoi(args[4]);
					if(0>opt.x0)
					{
						opt.x0=0;
					}
					if(0>opt.y0)
					{
						opt.y0=0;
					}
					if(120>opt.wid)
					{
						opt.wid=120;
					}
					if(120>opt.hei)
					{
						opt.hei=120;
					}
				}
			}
			fclose(fp);
		}
	}

	PolyCreConfig::Delete(cfg);
	PolyCreFileName::Delete(fileAndDirName);
}

/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
#ifdef FSGUI3D_USE_SYSTEM_FONT
	sysFont=new YsSystemFontRenderer;
	FsGuiObject::defUnicodeRenderer=sysFont;
#endif

#ifdef FSGUI3D_USE_MODERN_UI
	FsGuiObject::scheme=FsGuiObject::MODERN;
	FsGuiObject::defRoundRadius=8.0;
	FsGuiObject::defHScrollBar=20;
	FsGuiObject::defHAnnotation=14;
	FsGuiObject::defVSpaceUnit=12;

	FsGuiObject::defDialogBgCol.SetDoubleRGB(0.75,0.75,0.75);

	FsGuiObject::defTabBgCol.SetDoubleRGB(0.82,0.82,0.82);
	FsGuiObject::defTabClosedFgCol.SetDoubleRGB(0.8,0.8,0.8);
	FsGuiObject::defTabClosedBgCol.SetDoubleRGB(0.2,0.2,0.2);

	FsGuiObject::defBgCol.SetDoubleRGB(0.85,0.85,0.85);
	FsGuiObject::defFgCol.SetDoubleRGB(0.0,0.0,0.0);
	FsGuiObject::defActiveBgCol.SetDoubleRGB(0.3,0.3,0.7);
	FsGuiObject::defActiveFgCol.SetDoubleRGB(1.0,1.0,1.0);
	FsGuiObject::defFrameCol.SetDoubleRGB(0.0,0.0,0.0);

	FsGuiObject::defListFgCol.SetDoubleRGB(0.0,0.0,0.0);
	FsGuiObject::defListBgCol.SetDoubleRGB(0.8,0.8,0.8);
	FsGuiObject::defListActiveFgCol.SetDoubleRGB(1.0,1.0,1.0);
	FsGuiObject::defListActiveBgCol.SetDoubleRGB(0.3,0.3,0.7);
#endif

	int customFontHeight=PolyCreGetCustomFontHeight();
	if(0==customFontHeight)
	{
		customFontHeight=16;
	}

	FsGuiObject::defUnicodeRenderer->RequestDefaultFontWithPixelHeight(customFontHeight);
	FsGuiObject::defAsciiRenderer->RequestDefaultFontWithPixelHeight(customFontHeight);

	YsCoordSysModel=YSOPENGL;

	canvasPtr=FsGui3DAppInitialize(argc,argv);
}

/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	canvasPtr->Interval();
}

/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
	auto wposFn=canvasPtr->fileAndDirName->GetLastWindowPositionFileName();

	int x0,y0,wid,hei;
	FsGetWindowPosition(x0,y0);
	FsGetWindowSize(wid,hei);

	PolyCreFileName::MakeDirectoryForFile(wposFn);

	FILE *fp=YsFileIO::Fopen(wposFn,"w");
	if(NULL!=fp)
	{
		fprintf(fp,"POS %d %d %d %d\n",x0,y0,wid,hei);
		fclose(fp);
	}

	YsGLSLDeleteSharedRenderer();
	delete canvasPtr;


#ifdef FSGUI3D_USE_SYSTEM_FONT
	delete sysFont;
#endif

	FsSleep(50); // Windows apparently has a problem terminating within certain time after last rendering.  Wait 50 ms.
	exit(0); // This typically is quicker, otherwise destructors occasinally takes long time to finish.
}

/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	canvasPtr->Draw();
	needRedraw=false;
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	if(YSTRUE==canvasPtr->mustTerminate)
	{
		return true;
	}
	return FsLazyWindowApplicationBase::MustTerminate();
}

/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	canvasPtr->File_Exit(nullptr);
	return false;
}

/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 10;
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	if(YSTRUE==canvasPtr->NeedRedraw())
	{
		return true;
	}
	return needRedraw;
}

/* virtual */ bool FsLazyWindowApplication::StayIdleUntilEvent(void) const
{
	return true;
}



static FsLazyWindowApplication *appPtr=nullptr;

/* static */ FsLazyWindowApplicationBase *FsLazyWindowApplicationBase::GetApplication(void)
{
	if(nullptr==appPtr)
	{
		appPtr=new FsLazyWindowApplication;
	}
	return appPtr;
}
