/* ////////////////////////////////////////////////////////////

File Name: sample04-cube.cpp
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
	double cubeMatrix[16]=
	{
		1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,0.0,1.0
	};

	FsOpenWindow(32,32,800,600,1); // 800x600 pixels, useDoubleBuffer=1

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_LESS);

	FsPassedTime();  // Reset the timer

	while(FsInkey()==0)
	{
		int mx,my,lb,mb,rb,passed;
		double spinX,spinY;

		passed=FsPassedTime();

		FsPollDevice();
		FsGetMouseState(lb,mb,rb,mx,my);

		int wid,hei,cx,cy;
		FsGetWindowSize(wid,hei);
		cx=wid/2;
		cy=hei/2;

		spinX=(double)((mx-cx)/10)*(double)passed/1000.0;  // 1 pixel = degrees/sec
		spinY=(double)((my-cy)/10)*(double)passed/1000.0;  // 1 pixel = degrees/sec

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0,(double)wid/(double)hei,1.0,20.0);
		glTranslated(0.0,0.0,-10.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(spinX,0.0,1.0,0.0);
		glRotated(spinY,1.0,0.0,0.0);
		glMultMatrixd(cubeMatrix);
		glGetDoublev(GL_MODELVIEW_MATRIX,cubeMatrix);

		glBegin(GL_QUADS);

		glColor3ub(0,0,255);
		glVertex3d(-2.5,-2.5,-2.5);
		glVertex3d(-2.5, 2.5,-2.5);
		glVertex3d( 2.5, 2.5,-2.5);
		glVertex3d( 2.5,-2.5,-2.5);

		glColor3ub(255,0,0);
		glVertex3d(-2.5,-2.5, 2.5);
		glVertex3d(-2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5, 2.5);
		glVertex3d( 2.5,-2.5, 2.5);

		glColor3ub(255,0,255);
		glVertex3d(-2.5,-2.5,-2.5);
		glVertex3d(-2.5,-2.5, 2.5);
		glVertex3d(-2.5, 2.5, 2.5);
		glVertex3d(-2.5, 2.5,-2.5);

		glColor3ub(0,255,0);
		glVertex3d( 2.5,-2.5,-2.5);
		glVertex3d( 2.5,-2.5, 2.5);
		glVertex3d( 2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5,-2.5);

		glColor3ub(0,255,255);
		glVertex3d(-2.5,-2.5,-2.5);
		glVertex3d(-2.5,-2.5, 2.5);
		glVertex3d( 2.5,-2.5, 2.5);
		glVertex3d( 2.5,-2.5,-2.5);

		glColor3ub(255,255,0);
		glVertex3d(-2.5, 2.5,-2.5);
		glVertex3d(-2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5,-2.5);

		glEnd();

		FsSwapBuffers();

		FsSleep(20-passed);
	}

	return 0;
}


