/* ////////////////////////////////////////////////////////////

File Name: edit_create_airfoil.cpp
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

#include "../ysgebl_gui_editor_base.h"
#include "../miscdlg.h"
#include <ysgl.h>

#include "edit_create_airfoil.h"
#include "../airfoil/ysairfoil.h"
#include "../textresource.h"
#include <ysshellextedit_orientationutil.h>

////////////////////////////////////////////////////////////


YsCreateAirfoilDialog::YsCreateAirfoilDialog()
{
}

YsCreateAirfoilDialog::~YsCreateAirfoilDialog()
{
}

/*static*/ YsCreateAirfoilDialog *YsCreateAirfoilDialog::Create(GeblGuiEditorBase *canvas)
{
	YsCreateAirfoilDialog *newDlg=new YsCreateAirfoilDialog;
	newDlg->canvas=canvas;
	return newDlg;
}

/*static*/void YsCreateAirfoilDialog::Delete(YsCreateAirfoilDialog *toDel)
{
	delete toDel;
}

void YsCreateAirfoilDialog::Make(void)
{
	FsGuiDialog::Initialize();

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);

	closeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSTRUE);
	createBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_AIRFOIL_CREATE,YSFALSE);
	helpBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_HELP,YSFALSE);

	airfoilTypeTab=AddTabControl(0,FSKEY_NULL,YSTRUE);

	nacaTabIdent=AddTab(airfoilTypeTab,L"NACA 4-digit");
	BeginAddTabItem(airfoilTypeTab,nacaTabIdent);
	{
		nacaNumberTxt=AddTextBox(0,FSKEY_NULL,L"NACA Number",L"2312",5,YSTRUE);
		nacaNumDivisionTxt=AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_DIVCOUNT,L"12",5,YSFALSE);
		nacaSharpTailBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_AIRFOIL_NACA4_MATCHTAIL,YSFALSE);
		nacaSharpTailBtn->SetCheck(YSTRUE);
	}
	EndAddTabItem();

	naca6TabIdent=AddTab(airfoilTypeTab,L"NACA 6-Series");
	BeginAddTabItem(airfoilTypeTab,naca6TabIdent);
	{
		YsArray <YsString> naca6Label;
		AirfoilNACA6::GetAllLabel(naca6Label);

		YsArray <const char *> naca6LabelPtr;
		for(auto &label : naca6Label)
		{
			naca6LabelPtr.Append(label.Txt());
		}

		naca6LabelDrp=AddDropList(0,FSKEY_NULL,"Type",naca6LabelPtr.GetN(),naca6LabelPtr,6,32,32,YSTRUE);
		naca6LabelDrp->Select(0);

		const char *aLabel[]=
		{
			"Symmetric",
			"a=0.0",
			"a=0.1",
			"a=0.2",
			"a=0.3",
			"a=0.4",
			"a=0.5",
			"a=0.6",
			"a=0.7",
			"a=0.8",
			"a=0.9",
			"a=1.0",
			"MeanLine12",
			"MeanLine13",
			"MeanLine14",
			"MeanLine15",
			"MeanLine16",
			"MeanLine17",
			"MeanLine22",
			"MeanLine23",
			"MeanLine24",
			"MeanLine25",
			"MeanLine26",
			"MeanLine27",
			"MeanLine32",
			"MeanLine33",
			"MeanLine34",
			"MeanLine35",
			"MeanLine36",
			"MeanLine37",
			"MeanLine42",
			"MeanLine43",
			"MeanLine44",
			"MeanLine45",
			"MeanLine46",
			"MeanLine47",
			"MeanLine52",
			"MeanLine53",
			"MeanLine54",
			"MeanLine55",
			"MeanLine56",
			"MeanLine57",
			"MeanLine62",
			"MeanLine63",
			"MeanLine64",
			"MeanLine65",
			"MeanLine66",
			"MeanLine67",
			"MeanLine72",
			"MeanLine73",
			"MeanLine74",
			"MeanLine75",
			"MeanLine76",
			"MeanLine77",
			"MeanLine82",
			"MeanLine83",
			"MeanLine84",
			"MeanLine85",
			"MeanLine86",
			"MeanLine87",
			"MeanLine92",
			"MeanLine93",
			"MeanLine94",
			"MeanLine95",
			"MeanLine96",
			"MeanLine97",
			"MeanLine210",
			"MeanLine220",
			"MeanLine230",
			"MeanLine240",
			"MeanLine250",
			NULL
		};
		int nChoice=0;
		for(nChoice=0; NULL!=aLabel[nChoice]; ++nChoice)
		{
		}
		naca6MeanLineDrp=AddDropList(0,FSKEY_NULL,"Mean Line",nChoice,aLabel,6,12,12,YSTRUE);
		naca6MeanLineDrp->Select(5);
		naca6ModifiedMeanLineBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_AIRFOIL_NACA6_MODIFIEDMEANLINE,YSTRUE);
		naca6NumDivisionTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_COMMON_DIVCOUNT,L"16",5,YSFALSE);
	}
	EndAddTabItem();

	supercriticalTabIdent=AddTab(airfoilTypeTab,L"Supercritical");
	BeginAddTabItem(airfoilTypeTab,supercriticalTabIdent);
	{
		supercriticalLabelDrp=AddEmptyDropList(0,FSKEY_NULL,"Type",10,30,30,YSTRUE);

		YsArray <const char *> catalogue;
		AirfoilSuperCritical::GetCatalogue(catalogue);
		for(int i=0; i<catalogue.GetN(); ++i)
		{
			supercriticalLabelDrp->AddString(catalogue[i],YSFALSE);
		}
		supercriticalLabelDrp->Select(0);

		supercriticalNumDivisionTxt=AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_DIVCOUNT,L"16",5,YSFALSE);
		superCriticalMatchTailBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_AIRFOIL_SUPERCRITICAL_MATCHTAIL,YSFALSE);
	}
	EndAddTabItem();

	JoukowskiTabIdent=AddTab(airfoilTypeTab,L"Joukowski");
	BeginAddTabItem(airfoilTypeTab,JoukowskiTabIdent);
	{
		JoukowskiATxt=AddTextBox(0,FSKEY_NULL,L"A",L"1.0",8,YSTRUE);
		JoukowskiDTxt=AddTextBox(0,FSKEY_NULL,L"B",L"0.055",8,YSFALSE);
		JoukowskiETxt=AddTextBox(0,FSKEY_NULL,L"C",L"0.1",8,YSFALSE);
		JoukowskiNumDivisionTxt=AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_DIVCOUNT,L"16",5,YSFALSE);
	}
	EndAddTabItem();

	ClarkYTabIdent=AddTab(airfoilTypeTab,L"Clark-Y");
	BeginAddTabItem(airfoilTypeTab,ClarkYTabIdent);
	{
		ClarkYNumDivisionTxt=AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_DIVCOUNT,L"12",5,YSTRUE);
	}
	EndAddTabItem();

	lensTabIdent=AddTab(airfoilTypeTab,L"Lens");
	BeginAddTabItem(airfoilTypeTab,lensTabIdent);
	{
		lensNumDivisionTxt=AddTextBox(0,FSKEY_NULL,FSGUI_DLG_COMMON_DIVCOUNT,L"12",5,YSTRUE);
		lensH[0]=AddTextBox(0,FSKEY_NULL,"H1(%)",L"6",5,YSFALSE);
		lensB[0]=AddTextBox(0,FSKEY_NULL,"B1(%)",L"0.0",5,YSFALSE);
		lensH[1]=AddTextBox(0,FSKEY_NULL,"H2(%)",L"6",5,YSFALSE);
		lensB[1]=AddTextBox(0,FSKEY_NULL,"B2(%)",L"0.0",5,YSFALSE);
	}
	EndAddTabItem();

	airfoilTypeTab->SelectCurrentTab(nacaTabIdent);


	AddStaticText(0,FSKEY_NULL,"Third point controls:",YSTRUE);
	thirdPointIsNormal=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_NORMAL,YSFALSE);
	thirdPointIsTangent=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_TANGENT,YSFALSE);
	{
		FsGuiButton *radioButtonGrp[2]=
		{
			thirdPointIsNormal,thirdPointIsTangent
		};
		SetRadioButtonGroup(2,radioButtonGrp);
	}
	thirdPointIsNormal->SetCheck(YSTRUE);

	RecalculateAirfoil();
	SetTransparency(YSTRUE);
	Fit();
}

void YsCreateAirfoilDialog::RecalculateAirfoil(void)
{
	YsArray <YsVec3> airfoil;
	GetAirfoil(airfoil);
	airfoilVtxBuf.CleanUp();
	for(int i=0; i<airfoil.GetN(); ++i)
	{
		airfoilVtxBuf.AddVertex(airfoil[i]);
	}
	canvas->SetNeedRedraw(YSTRUE);
}

void YsCreateAirfoilDialog::GetAirfoil(YsArray <YsVec3> &point) const
{
	const int curTabIdent=airfoilTypeTab->GetCurrentTab();

	if(curTabIdent==nacaTabIdent)
	{
		AirfoilNACA airfoil;

		YSBOOL sharpTail=nacaSharpTailBtn->GetCheck();
		int nacaNumber=nacaNumberTxt->GetInteger();
		int nDiv=nacaNumDivisionTxt->GetInteger();

		airfoil.GenerateRawAirfoil(nacaNumber,1.0,YsGreater(100*nDiv,65536),sharpTail);
		airfoil.Resample(nDiv);

		GetAirfoil(point,airfoil);
	}
	else if(curTabIdent==naca6TabIdent)
	{
		AirfoilNACA6 airfoil;

		YSBOOL modifiedMeanLine=naca6ModifiedMeanLineBtn->GetCheck();
		YsString typeLabel;
		naca6LabelDrp->GetSelectedString(typeLabel);
		int nDiv=naca6NumDivisionTxt->GetInteger();

		YsString meanLineLabel;
		naca6MeanLineDrp->GetSelectedString(meanLineLabel);

		airfoil.GenerateRawAirfoil(typeLabel,meanLineLabel,modifiedMeanLine,1.0,YsGreater(100*nDiv,65536));
		airfoil.Resample(nDiv);

		GetAirfoil(point,airfoil);
	}
	else if(curTabIdent==supercriticalTabIdent)
	{
		AirfoilSuperCritical airfoil;
		int nDiv=supercriticalNumDivisionTxt->GetInteger();
		YsString selStr;
		supercriticalLabelDrp->GetSelectedString(selStr);

		airfoil.Generate(nDiv,selStr);

		if(YSTRUE==superCriticalMatchTailBtn->GetCheck())
		{
			airfoil.MatchTail();
		}
		GetAirfoil(point,airfoil);
	}
	else if(curTabIdent==JoukowskiTabIdent)
	{
		AirfoilJoukowski airfoil;
		const int nDiv=JoukowskiNumDivisionTxt->GetInteger();
		const double a=JoukowskiATxt->GetRealNumber();
		const double d=JoukowskiDTxt->GetRealNumber();
		const double e=JoukowskiETxt->GetRealNumber();
		airfoil.Generate(nDiv,a,d,e);
		GetAirfoil(point,airfoil);
	}
	else if(curTabIdent==ClarkYTabIdent)
	{
		AirfoilClarkY airfoil;
		int nDiv=ClarkYNumDivisionTxt->GetInteger();
		airfoil.Generate(nDiv);
		GetAirfoil(point,airfoil);
	}
	else if(curTabIdent==lensTabIdent)
	{
		AirfoilLens airfoil;
		int nDiv=lensNumDivisionTxt->GetInteger();
		const double h1=lensH[0]->GetRealNumber()/100.0;
		const double b1=lensB[0]->GetRealNumber()/100.0;
		const double h2=lensH[1]->GetRealNumber()/100.0;
		const double b2=lensB[1]->GetRealNumber()/100.0;
		airfoil.Generate(nDiv,1.0,h1,b1,h2,b2);
		airfoil.Normalize();
		GetAirfoil(point,airfoil);
	}
}

void YsCreateAirfoilDialog::GetAirfoil(YsArray <YsVec3> &point,const Airfoil &airfoil) const
{
	YsVec3 U,V;
	GetUV(U,V);

	point.CleanUp();
	for(int i=0;  i<airfoil.point.GetN(); ++i)
	{
		double u,v;
		u=airfoil.point[i].x();
		v=airfoil.point[i].y();

		const YsVec3 pos=front+U*u+V*v;
		point.Append(pos);
	}
}

void YsCreateAirfoilDialog::GetUV(YsVec3 &u,YsVec3 &v) const
{
	u=back-front;

	YsVec3 third=canvas->threeDInterface.point_pos;

	if(YSTRUE==thirdPointIsNormal->GetCheck())
	{
		YsVec3 n=YsUnitVector(third-front);
		v=YsUnitVector(n^u)*u.GetLength();
	}
	else
	{
		YsVec3 t=YsUnitVector(third-front);
		YsVec3 n=YsUnitVector(t^u);
		v=u^n;
	}
}

void YsCreateAirfoilDialog::OnButtonClick(FsGuiButton *btn)
{
	if(closeBtn==btn)
	{
		canvas->RemoveDialog(this);
	}
	else if(createBtn==btn)
	{
		canvas->Edit_CreateAirfoil_Create();
	}
	else if(nacaSharpTailBtn==btn || thirdPointIsNormal==btn || thirdPointIsTangent==btn || superCriticalMatchTailBtn==btn)
	{
		RecalculateAirfoil();
	}
	else if(helpBtn==btn)
	{
		canvas->MessageDialog(L"Create Airfoil",
		    L"If you select two vertices and then select Create-Airfoil from the menu,\n"
		    L"the selected vertices defines the mean chord.\n"
		    L"If you select three vertices and then select Create-Airfoil from the menu,\n"
		    L"the third vertex will define initial normal or tangent, depending on which\n"
		    L"you choose in the dialog.");
	}
}

void YsCreateAirfoilDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(txt==nacaNumberTxt)
	{
		RecalculateAirfoil();
	}
	else if(txt==nacaNumDivisionTxt || txt==supercriticalNumDivisionTxt || txt==JoukowskiNumDivisionTxt ||
	        txt==ClarkYNumDivisionTxt || txt==lensNumDivisionTxt)
	{
		int nDiv=txt->GetInteger();
		if(8<nDiv || 10000<nDiv)
		{
			nDiv=YsBound(nDiv,8,10000);
			txt->SetInteger(nDiv);
		}
		RecalculateAirfoil();
	}
	else
	{
		RecalculateAirfoil();
	}
}

void YsCreateAirfoilDialog::OnDropListSelChange(FsGuiDropList *,int)
{
	RecalculateAirfoil();
}

void YsCreateAirfoilDialog::OnSelectTab(FsGuiTabControl *,int)
{
	RecalculateAirfoil();
}

////////////////////////////////////////////////////////////


void GeblGuiEditorBase::Edit_CreateAirfoil(FsGuiPopUpMenuItem *)
{
	if(NULL!=slHd)
	{
		Edit_ClearUIMode();

		if(NULL==createAirfoilDlg)
		{
			createAirfoilDlg=YsCreateAirfoilDialog::Create(this);
			createAirfoilDlg->Make();
		}
		AddDialog(createAirfoilDlg);
		UIDrawCallBack3D=Edit_CreateAirfoil_DrawCallBack3D;
		ArrangeDialog();


		YsArray <YsShellVertexHandle> selVtHd;
		slHd->GetSelectedVertex(selVtHd);

		if(0==selVtHd.GetN())
		{
			createAirfoilDlg->front=YsOrigin();
			createAirfoilDlg->back.Set(0.0,0.0,-1.0);
			createAirfoilDlg->thirdPoint.Set(1.0,0.0,0.0);

			createAirfoilDlg->frontVtKey=YSNULLHASHKEY;
			createAirfoilDlg->backVtKey=YSNULLHASHKEY;
		}
		else if(1==selVtHd.GetN())
		{
			slHd->GetVertexPosition(createAirfoilDlg->front,selVtHd[0]);
			createAirfoilDlg->back=createAirfoilDlg->front;
			createAirfoilDlg->back.AddZ(-1.0);
			createAirfoilDlg->thirdPoint=createAirfoilDlg->front;
			createAirfoilDlg->thirdPoint.AddX(1.0);

			createAirfoilDlg->frontVtKey=slHd->GetSearchKey(selVtHd[0]);
			createAirfoilDlg->backVtKey=YSNULLHASHKEY;
		}
		else if(2==selVtHd.GetN())
		{
			slHd->GetVertexPosition(createAirfoilDlg->front,selVtHd[0]);
			slHd->GetVertexPosition(createAirfoilDlg->back,selVtHd[1]);
			createAirfoilDlg->thirdPoint=createAirfoilDlg->back;
			createAirfoilDlg->thirdPoint.AddX(1.0);

			createAirfoilDlg->frontVtKey=slHd->GetSearchKey(selVtHd[0]);
			createAirfoilDlg->backVtKey=slHd->GetSearchKey(selVtHd[1]);
		}
		else if(3==selVtHd.GetN())
		{
			slHd->GetVertexPosition(createAirfoilDlg->front,selVtHd[0]);
			slHd->GetVertexPosition(createAirfoilDlg->back,selVtHd[1]);
			slHd->GetVertexPosition(createAirfoilDlg->thirdPoint,selVtHd[2]);

			createAirfoilDlg->frontVtKey=slHd->GetSearchKey(selVtHd[0]);
			createAirfoilDlg->backVtKey=slHd->GetSearchKey(selVtHd[1]);
		}

		threeDInterface.BeginInputPoint2(createAirfoilDlg->thirdPoint,0,NULL);
		threeDInterface.SetCallBack(Edit_CreateAirfoil_PivotMoved,this);

		createAirfoilDlg->RecalculateAirfoil();

		SetNeedRedraw(YSTRUE);
	}
}

/*static*/ void GeblGuiEditorBase::Edit_CreateAirfoil_DrawCallBack3D(GeblGuiEditorBase &canvas)
{
	canvas.Edit_CreateAirfoil_DrawCallBack3D();
}

void GeblGuiEditorBase::Edit_CreateAirfoil_DrawCallBack3D(void)
{
	if(NULL!=createAirfoilDlg)
	{
		struct YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
		YsGLSLUse3DRenderer(renderer);

		const GLfloat edgeCol[4]={0,0,0,1.0f};
		YsGLSLSet3DRendererUniformColorfv(renderer,edgeCol);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINE_LOOP,(int)createAirfoilDlg->airfoilVtxBuf.GetNumVertex(),createAirfoilDlg->airfoilVtxBuf);

#ifdef GL_PROGRAM_POINT_SIZE
        glEnable(GL_PROGRAM_POINT_SIZE);
#endif
        YsGLSLSet3DRendererUniformPointSize(renderer,3);
		YsGLSLDrawPrimitiveVtxfv(renderer,GL_POINTS,(int)createAirfoilDlg->airfoilVtxBuf.GetNumVertex(),createAirfoilDlg->airfoilVtxBuf);
        YsGLSLSet3DRendererUniformPointSize(renderer,1);
#ifdef GL_PROGRAM_POINT_SIZE
        glDisable(GL_PROGRAM_POINT_SIZE);
#endif

		YsGLSLEndUse3DRenderer(renderer);
	}
}

/*static*/void GeblGuiEditorBase::Edit_CreateAirfoil_PivotMoved(void *appPtr,FsGui3DInterface &)
{
	GeblGuiEditorBase *canvas=(GeblGuiEditorBase *)appPtr;
	canvas->createAirfoilDlg->RecalculateAirfoil();
	canvas->SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::Edit_CreateAirfoil_Create(void)
{
	if(NULL!=slHd)
	{
		YsArray <YsVec3> airfoil;
		createAirfoilDlg->GetAirfoil(airfoil);

		YsShellExtEdit::StopIncUndo incUndo(*slHd);

		YsArray <YsShellVertexHandle> loopVtHd;

		for(int idx=0; idx<airfoil.GetN(); ++idx)
		{
			YsShellVertexHandle vtHd=NULL;
			if(airfoil[idx]==createAirfoilDlg->front)
			{
				vtHd=slHd->FindVertex(createAirfoilDlg->frontVtKey);
			}
			if(airfoil[idx]==createAirfoilDlg->back)
			{
				vtHd=slHd->FindVertex(createAirfoilDlg->backVtKey);
			}
			if(NULL==vtHd)
			{
				vtHd=slHd->AddVertex(airfoil[idx]);
			}
			loopVtHd.Append(vtHd);
		}

		YsShellPolygonHandle plHd=slHd->AddPolygon(loopVtHd);

		slHd->SetPolygonColor(plHd,colorPaletteDlg->GetColor());

		YsShellExtEdit_OrientationUtil flipper;
		flipper.RecalculateNormal(*slHd,plHd);

		slHd->SelectVertex(0,NULL);
	}

	Edit_ClearUIMode();

	needRemakeDrawingBuffer=NEED_REMAKE_DRAWING_VERTEX|NEED_REMAKE_DRAWING_POLYGON;
	SetNeedRedraw(YSTRUE);
}
