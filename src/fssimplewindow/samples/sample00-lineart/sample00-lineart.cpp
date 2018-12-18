/* ////////////////////////////////////////////////////////////

File Name: sample00-lineart.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#endif

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <fssimplewindow.h>

int main(void)
{
	FsOpenWindow(32,128,800,600,0); // 800x600 pixels, useDoubleBuffer=0

	// Verify window position and size.  Just for testing purpose. >>
	int x0,y0,wid,hei;
	FsGetWindowSize(wid,hei);
	FsGetWindowPosition(x0,y0);
	printf("%d %d %d %d\n",x0,y0,wid,hei);
	// Verify window position and size.  Just for testing purpose. <<

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_ALWAYS);

	glClear(GL_COLOR_BUFFER_BIT);

	srand(time(NULL));

	while(FsInkey()==0)
	{
		FsPollDevice();

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

		int x1,y1,x2,y2;
		int r,g,b;

		x1=rand()%wid;
		y1=rand()%hei;
		x2=rand()%wid;
		y2=rand()%hei;
		r=rand()%256;
		g=rand()%256;
		b=rand()%256;

		glColor3ub(r,g,b);
		glBegin(GL_LINES);
		glVertex2i(x1,y1);
		glVertex2i(x2,y2);
		glEnd();
		glFlush();

		FsSleep(10);
	}

	return 0;
}


