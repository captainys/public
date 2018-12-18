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

#include <fsgui3d.h>
#include <ysgl.h>


// Comment out FSGUI3D_USE_SYSTEM_FONT to remove depencency on YsSystemFont library.
#define FSGUI3D_USE_SYSTEM_FONT

// Comment out FSGUI3D_USE_MODERN_UI to use primitive graphics, which may be faster.
#define FSGUI3D_USE_MODERN_UI


#ifdef FSGUI3D_USE_SYSTEM_FONT
#include <ysfontrenderer.h>
#endif



enum
{
	MENU_FILE_EXIT=0x7f000001
};

static void SetUpMainMenu(FsGuiPopUpMenu &mainMenu)
{
	mainMenu.Initialize();
	mainMenu.SetIsPullDownMenu(YSTRUE);

	FsGuiPopUpMenuItem *fileMenu=mainMenu.AddTextItem(0,FSKEY_F,L"File");
	FsGuiPopUpMenu *fileSubMenu=fileMenu->GetSubMenu();
	fileSubMenu->AddTextItem(MENU_FILE_EXIT,FSKEY_X,L"Exit");
}

static void Draw(const FsGuiCanvas &mainCanvas,const FsGui3DViewControl &drawEnv,FsGui3DInterface &threeDInterface)
{
	int viewport[4];
	drawEnv.GetOpenGlCompatibleViewport(viewport);

	const YsMatrix4x4 &projMat=drawEnv.GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=drawEnv.GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);

	threeDInterface.SetViewport(viewport);
	threeDInterface.SetProjViewModelMatrix(projViewMat);



	glUseProgram(0);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);


	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);

	double glMat[16];
	projMat.GetOpenGlCompatibleMatrix(glMat);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixd(glMat);

	viewMat.GetOpenGlCompatibleMatrix(glMat);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(glMat);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	threeDInterface.Draw();



	glUseProgram(0);
	glColor3ub(0,0,0);
	glBegin(GL_LINES);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(1.0,0.0,0.0);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(0.0,1.0,0.0);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(0.0,0.0,1.0);
	glEnd();


	int nTri=0;
	YsVec3 tri[9];
	if(YSOK==FsGui3DGetArrowHeadTriangle(
	    tri+nTri*3,YsOrigin(),YsXVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		++nTri;
	}
	if(YSOK==FsGui3DGetArrowHeadTriangle(
	    tri+nTri*3,YsOrigin(),YsYVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		++nTri;
	}
	if(YSOK==FsGui3DGetArrowHeadTriangle(
	    tri+nTri*3,YsOrigin(),YsZVec(),viewport,projMat,viewMat,0.1,10,8))
	{
		++nTri;
	}

	if(0<nTri)
	{
		glBegin(GL_TRIANGLES);
		for(int i=0; i<nTri*3; ++i)
		{
			glVertex3dv(tri[i]);
		}
		glEnd();
	}


	// 2D Drawing
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,(double)viewport[2],(double)viewport[3],0.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);
	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

	glDisable(GL_DEPTH_TEST);

	glUseProgram(0);
	mainCanvas.Show();

	FsSwapBuffers();
}

int main(int argc,char *argv[])
{
	FsOpenWindow(0,0,1024,640,1);

#ifdef FSGUI3D_USE_SYSTEM_FONT
	YsSystemFontRenderer *sysFont=new YsSystemFontRenderer;
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

	FsGuiObject::defUnicodeRenderer->RequestDefaultFontWithPixelHeight(16);
	FsGuiObject::defAsciiRenderer->RequestDefaultFontWithPixelHeight(16);

	YsGLSLCreateSharedRenderer();


	FsGuiPopUpMenu mainMenu;
	FsGuiCanvas mainCanvas;
	FsGui3DInterface threeDInterface(&mainCanvas);
	FsGui3DViewControl drawEnv;
	FsGui3DViewControlDialog *viewControlDlg=FsGui3DViewControlDialog::Create();

	drawEnv.SetVerticalOrientation(YSTRUE);
	drawEnv.SetTargetBoundingBox(-5.0*YsXYZ(),5.0*YsXYZ());
	drawEnv.SetViewTarget(YsOrigin());
	drawEnv.SetViewDistance(7.5);
	drawEnv.AttachCanvas(&mainCanvas);

	SetUpMainMenu(mainMenu);
	mainCanvas.SetMainMenu(&mainMenu);
	viewControlDlg->Make(&mainCanvas,&drawEnv);

	mainCanvas.AddDialog(viewControlDlg);

	YsCoordSysModel=YSOPENGL;

	YSBOOL terminate=YSFALSE;
	while(YSTRUE!=terminate)
	{
		FsPollDevice();

		{
			int key;
			while(FSKEY_NULL!=(key=FsInkey()))
			{
				if(YSOK!=drawEnv.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
				{
					if(YSOK!=threeDInterface.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
					{
						mainCanvas.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT));
					}
				}
			}
		}

		{
			int charCode;
			while(0!=(charCode=FsInkeyChar()))
			{
				mainCanvas.CharIn(charCode);
			}
		}

		{
			int lb,mb,rb,mx,my;
			while(FSMOUSEEVENT_NONE!=FsGetMouseEvent(lb,mb,rb,mx,my))
			{
				if(YSOK!=drawEnv.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
				{
					if(YSOK!=mainCanvas.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
					{
						if(YSOK!=threeDInterface.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
						{
						}
					}
				}
			}
		}



		const FsGuiPopUpMenuItem *selectedMenuItem=mainMenu.GetLastSelectedItem();
		if(NULL!=selectedMenuItem)
		{
			switch(selectedMenuItem->GetId())
			{
			case MENU_FILE_EXIT:
				terminate=YSTRUE;
				break;
			}
		}



		int winWid,winHei;
		FsGetWindowSize(winWid,winHei);
		mainCanvas.SetWindowSize(winWid,winHei,/*autoArrangeDialog=*/YSTRUE);

		drawEnv.SetWindowSize(winWid,winHei);
		drawEnv.SetViewportByTwoCorner(0,0,winWid,winHei);
		drawEnv.UpdateNearFar();

		if(YSTRUE==mainCanvas.NeedRedraw() || 
		   YSTRUE==drawEnv.NeedRedraw() ||
		   YSTRUE==threeDInterface.NeedRedraw() ||
		   0!=FsCheckWindowExposure())
		{
			// Do this before Draw function.  This will allow one of the elements set SetNeedRedraw(YSTRUE) 
			// within drawing function so that Draw function will be called again in the next iteragion. >>
			mainCanvas.SetNeedRedraw(YSFALSE);
			drawEnv.SetNeedRedraw(YSFALSE);
			threeDInterface.SetNeedRedraw(YSFALSE);
			// <<

			Draw(mainCanvas,drawEnv,threeDInterface);
		}

		FsSleep(10); // Not to burn a CPU.
	}

	FsGui3DViewControlDialog::Delete(viewControlDlg);

	YsGLSLDeleteSharedRenderer();

	FsCloseWindow();

#ifdef FSGUI3D_USE_SYSTEM_FONT
	delete sysFont;
#endif

	FsSleep(50); // Windows apparently has a problem terminating within certain time after last rendering.  Wait 50 ms.

	exit(0); // This typically is quicker, otherwise destructors occasinally takes long time to finish.
	return 0;
}
