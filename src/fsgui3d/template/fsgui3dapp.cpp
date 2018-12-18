/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp.cpp
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
#include <yscompilerwarning.h>
#include <ysgl.h>

#include "fsgui3dapp.h"



// If you want to use fixed-function pipeline of OpenGL 1.x,
//   (1) comment out the following line, and
//   (2) change linking libraries in CMakeLists.txt from:
//          fsguilib_gl2 fsgui3d_gl2
//       to
//          fsguilib_gl1 fsgui3d_gl1
#define DONT_USE_FIXED_FUNCTION_PIPELINE



static FsGui3DMainCanvas *appMainCanvas;

////////////////////////////////////////////////////////////

FsGui3DMainCanvas::FsGui3DMainCanvas() : threeDInterface(this),viewControlDlg(NULL)
{
	mainMenu=nullptr;
	appMustTerminate=YSFALSE;
	drawEnv.AttachCanvas(this);
}

FsGui3DMainCanvas::~FsGui3DMainCanvas()
{
	// The following two lines ensure that all self-destructive dialogs are cleaned. 2015/03/18
	RemoveDialogAll();
	PerformScheduledDeletion();

	DeleteMainMenu();
	if(NULL!=viewControlDlg)
	{
		FsGui3DViewControlDialog::Delete(viewControlDlg);
	}
}

void FsGui3DMainCanvas::Initialize(int argc,char *argv[])
{
	MakeMainMenu();
	AddViewControlDialog();

	YsDisregardVariable(argc);
	YsDisregardVariable(argv);

	YsGLSLCreateSharedRenderer();

	drawEnv.SetTargetBoundingBox(-5.0*YsXYZ(),5.0*YsXYZ());
	drawEnv.SetViewTarget(YsOrigin());
	drawEnv.SetViewDistance(7.5);
}

void FsGui3DMainCanvas::AddViewControlDialog(void)
{
	viewControlDlg=FsGui3DViewControlDialog::Create();
	viewControlDlg->Make(this,&drawEnv);
	this->AddDialog(viewControlDlg);
}

void FsGui3DMainCanvas::MakeMainMenu(void)
{
	mainMenu=new FsGuiPopUpMenu;
	mainMenu->Initialize();
	mainMenu->SetIsPullDownMenu(YSTRUE);

	FsGuiPopUpMenuItem *fileMenu=mainMenu->AddTextItem(0,FSKEY_F,L"File");
	FsGuiPopUpMenu *fileSubMenu=fileMenu->GetSubMenu();

	fileSubMenu->AddTextItem(0,FSKEY_X,L"Exit")->BindCallBack(&THISCLASS::File_Exit,this);

	SetMainMenu(mainMenu);
}

void FsGui3DMainCanvas::DeleteMainMenu(void)
{
	delete mainMenu;
	mainMenu=nullptr;
}

void FsGui3DMainCanvas::OnInterval(void)
{
	FsGuiCanvas::Interval();

	{
		int key;
		while(FSKEY_NULL!=(key=FsInkey()))
		{
			if(YSOK!=drawEnv.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
			{
				if(YSOK!=threeDInterface.KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT)))
				{
					this->KeyIn(key,(YSBOOL)FsGetKeyState(FSKEY_SHIFT),(YSBOOL)FsGetKeyState(FSKEY_CTRL),(YSBOOL)FsGetKeyState(FSKEY_ALT));
				}
			}
		}
	}

	{
		int charCode;
		while(0!=(charCode=FsInkeyChar()))
		{
			this->CharIn(charCode);
		}
	}

	{
		int lb,mb,rb,mx,my;
		while(FSMOUSEEVENT_NONE!=FsGetMouseEvent(lb,mb,rb,mx,my))
		{
			if(YSOK!=drawEnv.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
			{
				if(YSOK!=this->SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
				{
					if(YSOK!=threeDInterface.SetMouseState((YSBOOL)lb,(YSBOOL)mb,(YSBOOL)rb,mx,my))
					{
					}
				}
			}
		}
	}

	{
		auto nTouch=FsGetNumCurrentTouch();
		auto touch=FsGetCurrentTouch();
		if(YSOK!=drawEnv.SetTouchState(nTouch,touch))
		{
			if(YSOK!=this->SetTouchState(nTouch,touch))
			{
				if(YSOK!=threeDInterface.SetTouchState(nTouch,touch))
				{
				}
			}
		}
	}

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	this->SetWindowSize(winWid,winHei,/*autoArrangeDialog=*/YSTRUE);

	drawEnv.SetWindowSize(winWid,winHei);
	drawEnv.SetViewportByTwoCorner(0,0,winWid,winHei);

	if(YSTRUE==drawEnv.NeedRedraw() ||
	   YSTRUE==threeDInterface.NeedRedraw() ||
	   0!=FsCheckWindowExposure())
	{
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::Draw(void)
{
	// Do this at the beginning of Draw funtion.  This will allow one of the elements set SetNeedRedraw(YSTRUE) 
	// within drawing function so that Draw function will be called again in the next iteragion. >>
	SetNeedRedraw(YSFALSE);
	drawEnv.SetNeedRedraw(YSFALSE);
	threeDInterface.SetNeedRedraw(YSFALSE);
	// <<

	int viewport[4];
	drawEnv.GetOpenGlCompatibleViewport(viewport);
	drawEnv.SetVerticalOrientation(YSTRUE);
	drawEnv.UpdateNearFar();


	glUseProgram(0);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);


	const YsMatrix4x4 &projMat=drawEnv.GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=drawEnv.GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);

	threeDInterface.SetViewport(viewport);
	threeDInterface.SetProjViewModelMatrix(projViewMat);


	glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);

	GLfloat glProjMat[16];
	projMat.GetOpenGlCompatibleMatrix(glProjMat);

	GLfloat glModelviewMat[16];
	viewMat.GetOpenGlCompatibleMatrix(glModelviewMat);

#ifndef DONT_USE_FIXED_FUNCTION_PIPELINE
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(glProjMat);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(glModelviewMat);
#endif


	YsGLSLSetShared3DRendererProjection(glProjMat);
	YsGLSLSetShared3DRendererModelView(glModelviewMat);



	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	threeDInterface.Draw();

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

#ifdef DONT_USE_FIXED_FUNCTION_PIPELINE
	{
        auto renderer=YsGLSLSharedVariColor3DRenderer();
        YsGLSLUse3DRenderer(renderer);

		const float col[4]={0,0,0,1};
        YsGLSLSet3DRendererUniformColorfv(renderer,col);

        const float vtxPos[6*3]=
		{
			0,0,0, 1,0,0,
			0,0,0, 0,1,0,
			0,0,0, 0,0,1
		};
        YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINES,6,vtxPos);

		if(0<nTri)
		{
	        float triVtxPos[9*3];
			for(int i=0; i<nTri*3; ++i)
			{
				triVtxPos[i*3  ]=tri[i].xf();
				triVtxPos[i*3+1]=tri[i].yf();
				triVtxPos[i*3+2]=tri[i].zf();
			}
	        YsGLSLDrawPrimitiveVtxfv(renderer,GL_TRIANGLES,nTri*3,triVtxPos);
		}
        YsGLSLEndUse3DRenderer(renderer);
	}
#else
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

	if(0<nTri)
	{
		glBegin(GL_TRIANGLES);
		for(int i=0; i<nTri*3; ++i)
		{
			glVertex3dv(tri[i]);
		}
		glEnd();
	}
#endif


	// 2D Drawing
#ifndef DONT_USE_FIXED_FUNCTION_PIPELINE
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,(double)viewport[2],(double)viewport[3],0.0,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#endif

	YsGLSLUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());
	YsGLSLUseWindowCoordinateInPlain2DDrawing(YsGLSLSharedPlain2DRenderer(),YSTRUE);

	YsGLSLDrawPlain2DLined(YsGLSLSharedPlain2DRenderer(),0,0,1000,1000,1,0,0,1);

	YsGLSLEndUsePlain2DRenderer(YsGLSLSharedPlain2DRenderer());

	glDisable(GL_DEPTH_TEST);

	glUseProgram(0);
	FsGuiCanvas::Show();

	FsSwapBuffers();
}



////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::File_Exit(FsGuiPopUpMenuItem *)
{
	auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
	msgDlg->Make(L"Confirm Exit?",L"Confirm Exit?",L"Yes",L"No");
	msgDlg->BindCloseModalCallBack(&THISCLASS::File_Exit_ConfirmExitCallBack,this);
	AttachModalDialog(msgDlg);
}

void FsGui3DMainCanvas::File_Exit_ConfirmExitCallBack(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		File_Exit_ReallyExit();
	}
}

void FsGui3DMainCanvas::File_Exit_ReallyExit(void)
{
	this->appMustTerminate=YSTRUE;
}

////////////////////////////////////////////////////////////

