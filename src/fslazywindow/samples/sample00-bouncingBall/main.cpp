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

#include <fslazywindow.h>
#include <ysglcpp.h>
#include <ysglbuffer.h>
#include <ysglslcpp.h>

class FsLazyWindowApplication : public FsLazyWindowApplicationBase
{
protected:
	bool needRedraw;

public:
	FsLazyWindowApplication();
	virtual void BeforeEverything(int argc,char *argv[]);
	virtual void GetOpenWindowOption(FsOpenWindowOption &OPT) const;
	virtual void Initialize(int argc,char *argv[]);
	virtual void Interval(void);
	virtual void BeforeTerminate(void);
	virtual void Draw(void);
	virtual bool MustTerminate(void) const;
	virtual long long int GetMinimumSleepPerInterval(void) const;
	virtual bool NeedRedraw(void) const;
	virtual void ContextLostAndRecreated(void);

	enum
	{
		nBall=100
	};
	double ballX[nBall],ballY[nBall];
	double ballVx[nBall],ballVy[nBall];

	int nPrevTouch;

	void Reset(void);
	void AddCircle(
	   YsGLVertexBuffer2D &vtxBuf,
	   YsGLColorBuffer &colBuf,
	   float cx,float cy,float r,const float col[4]) const;
	float GetScale(void) const;
	void ConvertToScreen(float &sx,float &sy,double x,double y) const;
	void MoveBall(double &x,double &y,double &vx,double &vy,double m,double dt) const;
	void BounceBall(double &x,double &y,double &vx,double &vy) const;
	void Collision(double x1,double y1,double &vx1,double &vy1,double r1,
	               double x2,double y2,double &vx2,double &vy2,double r2) const;
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
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
}

/* virtual */ void FsLazyWindowApplication::Initialize(int argc,char *argv[])
{
	YsGLSLRenderer::CreateSharedRenderer();
	nPrevTouch=0;
	Reset();
}
/* virtual */ void FsLazyWindowApplication::Interval(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		SetMustTerminate(true);
	}

	auto nTouch=FsGetNumCurrentTouch();
	if(0==nPrevTouch && 0<nTouch)
	{
		Reset();
	}
	nPrevTouch=nTouch;

	const int passed=FsPassedTime();
	const double dt=(double)passed/1000.0;

	printf("%d %d\n",passed,FsSubSecondTimer());

	for(int i=0; i<nBall; ++i)
	{
		MoveBall(ballX[i],ballY[i],ballVx[i],ballVy[i],1.0,dt);
		BounceBall(ballX[i],ballY[i],ballVx[i],ballVy[i]);
	}

	for(int i=0; i<nBall; ++i)
	{
		for(int j=i+1; j<nBall; ++j)
		{
			Collision(ballX[i],ballY[i],ballVx[i],ballVy[i],1.0,
			          ballX[j],ballY[j],ballVx[j],ballVy[j],1.0);
		}
	}

	needRedraw=true;
}
/* virtual */ void FsLazyWindowApplication::BeforeTerminate(void)
{
}
/* virtual */ void FsLazyWindowApplication::Draw(void)
{
	int w,h;
	FsGetWindowSize(w,h);
	glViewport(0,0,w,h);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	YsGLSL2DRenderer renderer;

	renderer.UseWindowCoordinateTopLeftAsOrigin();


	const float black[4]={0,0,0,1};
	YsGLVertexBuffer2D vtxBuf;
	YsGLColorBuffer colBuf;

	for(int i=0; i<nBall; ++i)
	{
		float sx,sy;
		ConvertToScreen(sx,sy,ballX[i],ballY[i]);
		float rad=1.0f*GetScale();
		AddCircle(vtxBuf,colBuf,sx,sy,rad,black);
	}
	renderer.DrawVtxCol(YsGL::TRIANGLES,vtxBuf.GetN(),vtxBuf,colBuf);

	FsSwapBuffers();

	needRedraw=false;
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

/* virtual */ void FsLazyWindowApplication::ContextLostAndRecreated(void)
{
	YsGLSLRenderer::SharedRendererLost();
	YsGLSLRenderer::CreateSharedRenderer();
}

void FsLazyWindowApplication::AddCircle(
   YsGLVertexBuffer2D &vtxBuf,
   YsGLColorBuffer &colBuf,
   float cx,float cy,float r,const float col[4]) const
{
	float prevX=cx+r;
	float prevY=cy;
	const double rd=(double)r;

	for(int a=1; a<=64; ++a)
	{
		vtxBuf.Add(cx,cy);
		colBuf.Add(col[0],col[1],col[2],col[3]);

		vtxBuf.Add(prevX,prevY);
		colBuf.Add(col[0],col[1],col[2],col[3]);

		const double radian=YsPi*2.0*(double)a/64.0;
		const float x=cx+(float)(rd*cos(radian));
		const float y=cy+(float)(rd*sin(radian));

		vtxBuf.Add(x,y);
		colBuf.Add(col[0],col[1],col[2],col[3]);

		prevX=x;
		prevY=y;
	}
}

float FsLazyWindowApplication::GetScale(void) const
{
	int w,h;
	FsGetWindowSize(w,h);
	return (float)w/80.0;
}

void FsLazyWindowApplication::ConvertToScreen(float &sx,float &sy,double x,double y) const
{
	int w,h;
	FsGetWindowSize(w,h);
	auto scale=GetScale();

	sx=(float)(w/2)+(float)x*scale;
	sy=(float)(h)  -(float)y*scale;
}
void FsLazyWindowApplication::MoveBall(double &x,double &y,double &vx,double &vy,double m,double dt) const
{
	x+=vx*dt;
	y+=vy*dt;

	double Fx=0.0;
	double Fy=-9.8*m;

	double ax=Fx/m;
	double ay=Fy/m;

	vx+=ax*dt;
	vy+=ay*dt;
}
void FsLazyWindowApplication::BounceBall(double &x,double &y,double &vx,double &vy) const
{
	if(y<1.0 && vy<0.0)
	{
		vy=-vy;
	}
	if((x<-39.0 && vx<0.0) || (39.0<x && 0.0<vx))
	{
		vx=-vx;
	}
}

void FsLazyWindowApplication::Collision(double x1,double y1,double &vx1,double &vy1,double r1,
	               double x2,double y2,double &vx2,double &vy2,double r2) const
{
	double dx=x2-x1;
	double dy=y2-y1;
	double distSq=dx*dx+dy*dy;
	double dist=sqrt(distSq);
	if(dist<r1+r2)
	{
		if(0.0<distSq)
		{
			double nx=(x2-x1)/dist;
			double ny=(y2-y1)/dist;
			double k1=vx1*nx+vy1*ny;
			double k2=vx2*nx+vy2*ny;

			if((k2>0.0 && k2<k1) ||
			   (k1<0.0 && k2<k1) ||
			   (k1>0.0 && k2<0.0))
			{
				vx1=vx1+nx*(k2-k1);
				vy1=vy1+ny*(k2-k1);

				vx2=vx2+nx*(k1-k2);
				vy2=vy2+ny*(k1-k2);
			}
		}
	}
}

void FsLazyWindowApplication::Reset(void)
{
	for(int i=0; i<nBall; ++i)
	{
		int x=rand()%20-10;
		int y=rand()%20+20;
		int vx=rand()%20-10;
		int vy=rand()%20-10;

		ballX[i]=(double)x;
		ballY[i]=(double)y;
		ballVx[i]=(double)vx;
		ballVy[i]=(double)vy;
	}

	FsPassedTime();
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
