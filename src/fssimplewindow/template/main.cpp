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

#include <fssimplewindow.h>

class Sample
{
public:
	int x[4],y[4];
	Sample();
	void Move(int &x,int &y) const;
	void RunOneStep(void);
	void Draw(void) const;
};

Sample::Sample()
{
	x[0]=0;
	y[0]=0;
	x[1]=100;
	y[1]=0;
	x[2]=100;
	y[2]=100;
	x[3]=0;
	y[3]=100;
}

void Sample::Move(int &x,int &y) const
{
	if(0==y)
	{
		++x;
		if(100<x)
		{
			y=x-100;
			x=100;
		}
	}
	else if(100==x)
	{
		++y;
		if(100<y)
		{
			x=200-y;
			y=100;
		}
	}
	else if(100==y)
	{
		--x;
		if(0>x)
		{
			y=100+x;
			x=0;
		}
	}
	else if(0==x)
	{
		--y;
		if(0>y)
		{
			x=-y;
			y=0;
		}
	}
}

void Sample::RunOneStep(void)
{
	for(int i=0; i<4; ++i)
	{
		Move(x[i],y[i]);
	}
}

void Sample::Draw(void) const
{
	int w,h;
	FsGetWindowSize(w,h);
	glBegin(GL_LINE_LOOP);
	for(int i=0; i<4; ++i)
	{
		auto sx=x[i]*w/100;
		auto sy=y[i]*h/100;
		glVertex2i(sx,sy);
	}
	glEnd();
}

int main(void)
{
	FsOpenWindowOption opt;
	opt.x0=0;
	opt.y0=0;
	opt.wid=1200;
	opt.hei=800;
	opt.useDoubleBuffer=true;

	Sample sample;

	FsOpenWindow(opt);
	for(;;)
	{
		FsPollDevice();
		auto key=FsInkey();
		if(FSKEY_ESC==key)
		{
			break;
		}

		sample.RunOneStep();

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Set up 2D drawing
		glViewport(0,0,wid,hei);
	    glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		sample.Draw();

		FsSwapBuffers();
		FsSleep(25);
	}

	FsCloseWindow();
	return 0;
}
