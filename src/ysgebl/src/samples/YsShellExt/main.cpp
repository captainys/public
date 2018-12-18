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

#include <stdio.h>

#include <ysshellext.h>
#include <ysshellextio.h>

#include <fssimplewindow.h>  // OpenGL headers are included from here.



int main(int ac,char *av[])
{
	YsShellExt shl;
	YsShellExtReader reader;

	FILE *fp=fopen(av[1],"r");
	if(NULL!=fp)
	{
		YsTextFileInputStream inStream(fp);
		reader.MergeSrf(shl,inStream);

		shl.EnableSearch();  // RemakePolygonBuffer requires EnableSearch

		YsShellExtDrawingBuffer &drawBuf=shl.GetDrawingBuffer();
		drawBuf.RemakePolygonBuffer(shl,0.8);

		FsOpenWindow(0,0,800,600,1);

		glShadeModel(GL_SMOOTH);
		glDisable(GL_CULL_FACE);

		GLfloat hdg=0.0;

		for(;;)
		{
			FsPollDevice();
			auto inkey=FsInkey();

			int wid,hei;
			FsGetWindowSize(wid,hei);

			if(FSKEY_ESC==inkey)
			{
				break;
			}

			glClearColor(0.8f,0.8f,0.8f,0.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);


			// It assumes that the model is about the size of 10m to 50m.
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(30.0f,(GLfloat)wid/(GLfloat)hei,1.0f,80.0f);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(0.0f,0.0f,-40.0f);

			glRotatef(hdg,0.0f,1.0f,0.0f);
			glScalef(1.0f,1.0f,-1.0f);  // Conversion Left-Hand Coord to Right-Hand Coord


			// Head light
			const GLfloat light0[]={0.0f,0.0f,1.0f,0.0f};
			glLightfv(GL_LIGHT0,GL_POSITION,light0);
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);


			// Dump the vertex buffer contents.
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);

			glBegin(GL_TRIANGLES);
			for(int i=0; i<drawBuf.solidShadedVtxBuffer.GetNumVertex(); ++i)
			{
				glNormal3fv(drawBuf.solidShadedNomBuffer[i]);
				glColor3fv(drawBuf.solidShadedColBuffer[i]);
				glVertex3fv(drawBuf.solidShadedVtxBuffer[i]);
			}
			glEnd();

			glDisable(GL_LIGHTING);

			glBegin(GL_TRIANGLES);
			for(int i=0; i<drawBuf.solidUnshadedVtxBuffer.GetNumVertex(); ++i)
			{
				glColor3fv(drawBuf.solidUnshadedColBuffer[i]);
				glVertex3fv(drawBuf.solidUnshadedVtxBuffer[i]);
			}
			for(int i=0; i<drawBuf.backFaceVtxBuffer.GetNumVertex(); ++i)
			{
				glColor3fv(drawBuf.backFaceColBuffer[i]);
				glVertex3fv(drawBuf.backFaceVtxBuffer[i]);
			}
			glEnd();

			glBegin(GL_POINTS);
			for(int i=0; i<drawBuf.lightVtxBuffer.GetNumVertex(); ++i)
			{
				glColor3fv(drawBuf.lightColBuffer[i]);
				glVertex3fv(drawBuf.lightVtxBuffer[i]);
			}
			glEnd();


			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_LIGHTING);

			glBegin(GL_TRIANGLES);
			for(int i=0; i<drawBuf.trspShadedVtxBuffer.GetNumVertex(); ++i)
			{
				glNormal3fv(drawBuf.trspShadedNomBuffer[i]);
				glColor4fv(drawBuf.trspShadedColBuffer[i]);
				glVertex3fv(drawBuf.trspShadedVtxBuffer[i]);
			}
			glEnd();

			glDisable(GL_LIGHTING);

			glBegin(GL_TRIANGLES);
			for(int i=0; i<drawBuf.trspUnshadedVtxBuffer.GetNumVertex(); ++i)
			{
				glColor4fv(drawBuf.trspUnshadedColBuffer[i]);
				glVertex3fv(drawBuf.trspUnshadedVtxBuffer[i]);
			}
			glEnd();

			FsSwapBuffers();

			FsSleep(20);

			hdg+=0.5;
		}

		FsCloseWindow();
	}

	return 0;
}
