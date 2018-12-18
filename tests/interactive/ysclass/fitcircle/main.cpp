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

#include <ysclass.h>
#include <ysfitcircle.h>
#include <ysglcpp.h>
#include <ysglslcpp.h>

#include <yssystemfont.h>
#include <ystexturemanager.h>

#include <fslazywindow.h>


class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

	YsVec2 cen;
	double rad;
	YsArray <YsVec2> pnt;

	YsArray <float> pntVtxBuf;
	YsArray <float> circleVtxBuf;

	YsSystemFontCache sysFont;
	YsTextureManager texMan;
	YsTextureManager::TexHandle msgTexHd;

	YsArray <YsWString> msg;

public:
	void RemakeVertexArray(void);

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

void FsLazyWindowApplication::RemakeVertexArray(void)
{
	pntVtxBuf.CleanUp();
	circleVtxBuf.CleanUp();

	for(auto p : pnt)
	{
		pntVtxBuf.Add(p.x());
		pntVtxBuf.Add(p.y());
	}

	for(int angle=0; angle<360; angle+=12)
	{
		const double radian=(double)angle*YsPi/180.0;
		const double c=cos(radian);
		const double s=sin(radian);

		circleVtxBuf.Add(cen.x()+rad*c);
		circleVtxBuf.Add(cen.y()+rad*s);
	}
}

FsLazyWindowApplication::FsLazyWindowApplication()
{
	needRedraw=false;
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
	YsGLSLRenderer::CreateSharedRenderer();
	cen=YsVec2::Origin();
	rad=0.0;


	msg.Add(L"Left-click to add a point.");
	msg.Add(L"Right-click to clear points.");
	msg.Add(L"See a circle fit to the points.");

	sysFont.RequestDefaultFontWithHeight(24);

	auto genOpt=texMan.GetTextureGenerationOption();
	genOpt.needUpScaling=YSTRUE;
	texMan.SetTextureGenerationOption(genOpt);

	msgTexHd=texMan.AddTexture("");

	YsGLSLCreateSharedRenderer();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	int lb,mb,rb,mx,my;
	auto evt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(FSMOUSEEVENT_LBUTTONDOWN==evt)
	{
		YsVec2 p(mx,my);
		pnt.Add(p);

		YsFindLeastSquareFittingCircle(cen,rad,pnt);

		RemakeVertexArray();
	}
	if(FSMOUSEEVENT_RBUTTONDOWN==evt)
	{
		cen=YsVec2::Origin();
		rad=0.0;
		pnt.CleanUp();
		RemakeVertexArray();
	}

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);
	glViewport(0,0,wid,hei);

	needRedraw=false;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	{
		YsGLSL2DRenderer renderer;
		renderer.UseWindowCoordinateTopLeftAsOrigin();

		GLfloat red[]={1,0,0,1};
		GLfloat blue[]={0,0,1,1};

		glEnable(GL_PROGRAM_POINT_SIZE);

		renderer.SetUniformColor(blue);
		renderer.SetUniformPointSize(10);
		renderer.DrawVtx(GL_POINTS,pntVtxBuf.GetN()/2,pntVtxBuf);

		renderer.SetUniformColor(red);
		renderer.DrawVtx(GL_POINTS,circleVtxBuf.GetN()/2,circleVtxBuf);
	}

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
