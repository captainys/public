/* ////////////////////////////////////////////////////////////

File Name: main.cpp
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

#include <ysgl.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>
#include <ysclass.h>
#include <ysport.h>

#include <yssystemfont.h>
#include <ystexturemanager.h>

#include <fslazywindow.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	YsSystemFontCache sysFont;
	YsTextureManager texMan;
	YsTextureManager::TexHandle msgTexHd;

	YsArray <YsWString> msg;

	bool first;

public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool UserWantToCloseProgram(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
};

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;
	first=true;
}

/* virtual */ void FsLazyWindowApplication::BeforeEverything(int argc,char *argv[])
{
}
/* virtual */ void FsLazyWindowApplication::GetOpenWindowOption(FsOpenWindowOption &opt) const
{
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}
/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	sysFont.RequestDefaultFontWithHeight(24);

	auto genOpt=texMan.GetTextureGenerationOption();
	genOpt.needUpScaling=YSTRUE;
	texMan.SetTextureGenerationOption(genOpt);

	msgTexHd=texMan.AddTexture("");

	YsGLSLCreateSharedRenderer();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);



	msg.Add(YsFileIO::Getcwd());



	auto baseDir=YsSpecialPath::GetProgramBaseDirW();
	YsFileList fInfo;
	fInfo.FindFileList(baseDir,L"test",L"txt");

	msg.Add(L"From:");
	msg.Last().Append(baseDir);
	for(int i=0; i<fInfo.GetN(); ++i)
	{
		msg.Add(fInfo.GetFileName(i));
	}



	FsChangeToProgramDir();

	fInfo.FindFileList(L".",L"test",L"txt");

	msg.Add(L"From:.");
	for(int i=0; i<fInfo.GetN(); ++i)
	{
		msg.Add(fInfo.GetFileName(i));
	}
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}
	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	glViewport(0,0,winWid,winHei);

	int y=0;
	for(auto &str : msg)
	{
		const unsigned char fgCol[3]={0,0,0};
		const unsigned char bgCol[3]={0,0,0};
		auto bmp=sysFont.MakeRGBABitmap(str,fgCol,bgCol,YSFALSE);
		texMan.SetTextureFileData(msgTexHd,texMan.FOM_RAW_RGBA,bmp.Width(),bmp.Height(),4*bmp.Width()*bmp.Height(),bmp.Bitmap());

		YsGLVertexBuffer2D vtxBuf;
		YsGLVertexBuffer2D texCoordBuf;

		auto unitPtr=texMan.GetTexture(msgTexHd);
		if(nullptr!=unitPtr)
		{
			unitPtr->MakeActualTexture();

			auto w=unitPtr->GetWidth();
			auto h=unitPtr->GetHeight();

			vtxBuf.Add<float>(0,y);
			texCoordBuf.Add(unitPtr->ScaleTexCoord(0,1));
			vtxBuf.Add<float>(w,y);
			texCoordBuf.Add(unitPtr->ScaleTexCoord(1,1));
			vtxBuf.Add<float>(w,y+h);
			texCoordBuf.Add(unitPtr->ScaleTexCoord(1,0));
			vtxBuf.Add<float>(0,y+h);
			texCoordBuf.Add(unitPtr->ScaleTexCoord(0,0));

			unitPtr->Bind(0);

			auto renderer=YsGLSLSharedPlain2DRenderer();
			YsGLSLUsePlain2DRenderer(renderer);

			YsGLSLUseWindowCoordinateInPlain2DDrawing(renderer,1);

			const GLfloat col[4]={1,0,1,1};
			YsGLSLSetPlain2DRendererUniformColor(renderer,col);
			YsGLSLDrawPlain2DPrimitiveVtxTexCoordfv(renderer,GL_TRIANGLE_FAN,vtxBuf.GetN(),vtxBuf,texCoordBuf);

			YsGLSLEndUsePlain2DRenderer(renderer);
		}

		y+=bmp.Height();
	}

	FsSwapBuffers();

	needRedraw=false;
}
/* virtual */ bool FsLazyWindowApplication::UserWantToCloseProgram(void)
{
	return true; // Returning true will just close the program.
}
/* virtual */ bool FsLazyWindowApplication::MustTerminate(void) const
{
	return FsLazyWindowApplicationBase::MustTerminate();
}
/* virtual */ long long int FsLazyWindowApplication::GetMinimumSleepPerInterval(void) const
{
	return 10;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ bool FsLazyWindowApplication::NeedRedraw(void) const
{
	return needRedraw;
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
