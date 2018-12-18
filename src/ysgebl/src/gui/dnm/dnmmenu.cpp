/* ////////////////////////////////////////////////////////////

File Name: dnmmenu.cpp
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

#include <ysgl.h>
#include <ysglbuffer.h>

#include "../ysgebl_gui_editor_base.h"
#include "../textresource.h"
#include "dnmtreedialog.h"


void GeblGuiEditorBase::Dnm_DnmEditMode(FsGuiPopUpMenuItem *)
{
	this->dnmEditMode=YSTRUE;
	ArrangeDialog();
}

void GeblGuiEditorBase::Dnm_LoadStaticMode(void)
{
	Edit_ClearUIModeExceptDNMEditMode();

	this->dnmEditMode=YSTRUE;
	this->dnmCurrentState=-1;
	this->dnmState=this->shlGrp.EmptyDnmState();
	this->shlGrp.CacheTransformation(this->dnmState);
	this->SetNeedRedraw(YSTRUE);

	this->dnmTreeDlg->Make(this);
	this->AddDialog(this->dnmTreeDlg);
	ArrangeDialog();
}

void GeblGuiEditorBase::Dnm_LoadState0(void)
{
	Edit_ClearUIModeExceptDNMEditMode();

	this->dnmEditMode=YSTRUE;
	this->dnmCurrentState=0;
	this->shlGrp.SetState(this->dnmState,this->dnmCurrentState);
	this->shlGrp.CacheTransformation(this->dnmState);
	this->SetNeedRedraw(YSTRUE);

	this->dnmTreeDlg->Make(this);
	this->AddDialog(this->dnmTreeDlg);
	ArrangeDialog();
}

void GeblGuiEditorBase::Dnm_LoadState1(void)
{
	Edit_ClearUIModeExceptDNMEditMode();

	this->dnmEditMode=YSTRUE;
	this->dnmCurrentState=1;
	this->shlGrp.SetState(this->dnmState,this->dnmCurrentState);
	this->shlGrp.CacheTransformation(this->dnmState);
	this->SetNeedRedraw(YSTRUE);

	this->dnmTreeDlg->Make(this);
	this->AddDialog(this->dnmTreeDlg);
	ArrangeDialog();
}

void GeblGuiEditorBase::Dnm_LoadState2(void)
{
	Edit_ClearUIModeExceptDNMEditMode();

	this->dnmEditMode=YSTRUE;
	this->dnmCurrentState=2;
	this->shlGrp.SetState(this->dnmState,this->dnmCurrentState);
	this->shlGrp.CacheTransformation(this->dnmState);
	this->SetNeedRedraw(YSTRUE);

	this->dnmTreeDlg->Make(this);
	this->AddDialog(this->dnmTreeDlg);
	ArrangeDialog();
}

void GeblGuiEditorBase::Dnm_LoadState3(void)
{
	Edit_ClearUIModeExceptDNMEditMode();

	this->dnmEditMode=YSTRUE;
	this->dnmCurrentState=3;
	this->shlGrp.SetState(this->dnmState,this->dnmCurrentState);
	this->shlGrp.CacheTransformation(this->dnmState);
	this->SetNeedRedraw(YSTRUE);

	this->dnmTreeDlg->Make(this);
	this->AddDialog(this->dnmTreeDlg);
	ArrangeDialog();
}

void GeblGuiEditorBase::Dnm_EditTreeStructure(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIModeExceptDNMEditMode();

	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <PolyCreDnmTreeDialog>();
	dlg->Make(this);
	dlg->runningModal=YSTRUE;
	dlg->EnableTreeEdit();
	AttachModalDialog(dlg);
	ArrangeDialog();
	SetNeedRedraw(YSTRUE);
}

////////////////////////////////////////////////////////////

class GeblGuiEditorBase::Dnm_SetDnmNameDialog : public FsGuiDialog
{
private:
	GeblGuiEditorBase *canvasPtr;
	FsGuiButton *classAsAirBtn,*classAsGndBtn;
	YSBOOL nameChanged,classChanged;

	FsGuiButton *okBtn,*cancelBtn;

public:
	FsGuiTextBox *dnmNameTxt;
	FsGuiDropList *dnmClassDrp;

public:
	void Make(GeblGuiEditorBase *canvasPtr);
	void SetClassDropListAsAircraft(void);
	void SetClassDropListAsGround(void);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
};

void GeblGuiEditorBase::Dnm_SetDnmNameDialog::Make(GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	dnmNameTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"DNM Name",32,YSTRUE);

	dnmClassDrp=AddEmptyDropList(0,FSKEY_NULL,"CLASS",24,32,32,YSTRUE);
	classAsAirBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,L"As Aircraft",YSFALSE);
	classAsGndBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,L"As Ground",YSFALSE);
	classAsAirBtn->SetCheck(YSTRUE);
	{
		FsGuiButton *radioBtnGrp[2]={classAsAirBtn,classAsGndBtn};
		SetRadioButtonGroup(2,radioBtnGrp);
	}
	SetClassDropListAsAircraft();

	if(nullptr!=canvasPtr->slHd)
	{
		auto &shl=*canvasPtr->slHd;
		dnmNameTxt->SetText(shl.nodeName);
		dnmClassDrp->Select(shl.dnmClassType);
	}


	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	nameChanged=YSFALSE;
	classChanged=YSFALSE;

	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
}

void GeblGuiEditorBase::Dnm_SetDnmNameDialog::SetClassDropListAsAircraft(void)
{
	auto sel=dnmClassDrp->GetSelection();
	dnmClassDrp->ClearChoice();

	const char *const label[]=
	{
		" 0: Static & Landing Device",
		" 1: Variable Geometry Wing",
		" 2: Afterburner",
		" 3: Rotor (Heading Rotation)",
		" 4: Air Brake/Spoiler",
		" 5: Flap",
		" 6: Elevator",
		" 7: Aileron",
		" 8: Rudder",
		" 9: Bomb-bay Door",
		"10: Vertical Take-Off Nozzle",
		"11: Thrust Reverser",
		"12: Droop Nose",
		"13: Droop Nose Visor",
		"14: Landing-Gear Door",
		"15: Landing-Gear Compartment Wall",
		"16: Wheel Brake/Arresting Hook",
		"17: Landing-Gear Door (Fast Open/Close)",
		"18: Propeller Slow",
		"19:",
		"20: Propeller Fast",
		"21: Gunner Turret",
		"22: Tire",
		"23: Steering",
		"24: Rotor (Custom Rotation Axis) {After 20170415 ver}",
		"25:",
		"26:",
		"27:",
		"28:",
		"29:",
		"30: NAV Light",
		"31: Beacon",
		"32: Strobe",
		"33: Landing Light",
		"34: Gear-Mounted Landing Light",
		"35:",
		"36:",
		"37:",
		"38:",
		"39:",
		"40: Left Door",
		"41: Right Door",
		"42: Rear Door",
		"43: Interior",
		"44:",
		"45:",
		"46:",
		"47:",
		"48:",
		"49:",
		"50:",
	};
	for(auto l : label)
	{
		dnmClassDrp->AddString(l,YSFALSE);
	}

	dnmClassDrp->Select(sel);
}
void GeblGuiEditorBase::Dnm_SetDnmNameDialog::SetClassDropListAsGround(void)
{
	auto sel=dnmClassDrp->GetSelection();
	dnmClassDrp->ClearChoice();

	const char *const label[]=
	{
		" 0: Static",
		" 1: Gun Turret",
		" 2: Gun",
		" 3: SAM Turret",
		" 4: SAM",
		" 5: Cannon Turret",
		" 6: Cannon",
		" 7:",
		" 8:",
		" 9:",
		"10: Radar (Constant Heading Rotation)",
		"11: Radar Fast (Constant Heading Rotation)",
		"12:",
		"13:",
		"14:",
		"15:",
		"16: Brake",
		"17:",
		"18:",
		"20:",
		"21:",
		"22: Tire",
		"23: Steering",
		"24:",
		"25:",
		"26:",
		"27:",
		"28:",
		"29:",
		"30:",
		"31:",
		"32:",
		"33: Head Light",
		"34:",
		"35:",
		"36:",
		"37:",
		"38:",
		"39:",
		"40: Left Door",
		"41: Right Door",
		"42: Rear Door",
		"43: Interior",
		"44:",
		"45:",
		"46:",
		"47:",
		"48:",
		"49:",
		"50:",
	};
	for(auto l : label)
	{
		dnmClassDrp->AddString(l,YSFALSE);
	}

	dnmClassDrp->Select(sel);
}

/* virtual */ void GeblGuiEditorBase::Dnm_SetDnmNameDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		CloseModalDialog(YSOK);
	}
	else if(btn==cancelBtn)
	{
		CloseModalDialog(YSERR);
	}
	else if(btn==classAsAirBtn)
	{
		SetClassDropListAsAircraft();
	}
	else if(btn==classAsGndBtn)
	{
		SetClassDropListAsGround();
	}
}

/* virtual */ void GeblGuiEditorBase::Dnm_SetDnmNameDialog::OnDropListSelChange(FsGuiDropList *drp,int prevSel)
{
	if(drp==dnmClassDrp)
	{
		classChanged=YSTRUE;
	}
}
/* virtual */ void GeblGuiEditorBase::Dnm_SetDnmNameDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(txt==dnmNameTxt)
	{
		nameChanged=YSTRUE;
	}
}

void GeblGuiEditorBase::Dnm_SetDnmName(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<Dnm_SetDnmNameDialog>();
		dlg->Make(this);
		dlg->BindCloseModalCallBack(&THISCLASS::Dnm_SetDnmName_CloseModalCallBack,this);
		AttachModalDialog(dlg);
		ArrangeDialog();
	}
}

void GeblGuiEditorBase::Dnm_SetDnmName_CloseModalCallBack(FsGuiDialog *closedModalDialog,int returnCode)
{
	auto dnmNameDlg=dynamic_cast<Dnm_SetDnmNameDialog*>(closedModalDialog);
	if(nullptr!=dnmNameDlg && nullptr!=slHd && YSOK==(YSRESULT)returnCode)
	{
		auto &dnmNode=*slHd;
		dnmNode.nodeName=dnmNameDlg->dnmNameTxt->GetString();
		dnmNode.dnmClassType=dnmNameDlg->dnmClassDrp->GetSelection();
	}
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::Select_Shell_ByDialog(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIModeExceptDNMEditMode();

	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <PolyCreDnmTreeDialog>();
	dlg->Make(this);
	AddDialog(dlg);
	ArrangeDialog();
	SetNeedRedraw(YSTRUE);
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::Dnm_SetPivot(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		if(YSTRUE!=dnmEditMode)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_MUSTBE_DNMEDIT_MODE);
			return;
		}

		Edit_ClearUIModeExceptDNMEditMode();

		auto defPos=drawEnv.GetViewTarget();

		auto &shl=*slHd;
		auto selVtHd=shl.GetSelectedVertex();
		if(1<=selVtHd.GetN())
		{
			defPos=YsOrigin();
			for(auto vtHd : selVtHd)
			{
				defPos+=shl.GetVertexPosition(selVtHd[0]);
			}
			defPos/=(double)selVtHd.GetN();
		}

		threeDInterface.BeginInputPoint1(defPos);
		spaceKeyCallBack=std::bind(&THISCLASS::Dnm_SetPivot_SpaceKeyCallBack,this);
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Dnm_SetPivot_SpaceKeyCallBack(void)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
}

void GeblGuiEditorBase::Dnm_SetRotationAxis(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		if(YSTRUE!=dnmEditMode)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_MUSTBE_DNMEDIT_MODE);
			return;
		}

		// Selection: 2 vertices -> Pivot is the mid point.  Axis is the second-first.
		// Selection: 3 vertices -> Pivot is the center, Axis is the 3rd-mid(1st,2nd)
		// Selection: 4 vertices -> Pivot is the center, Axis is the mid(3rd,4th)-mid(1st,2nd)
		// Selection: 1 const edge -> Pivot is the mid(first vertex,last vertex), Axis is the last vertex - first vertex

		auto &shl=*slHd;
		YsArray <YsShellExt::VertexHandle> vtHd;
		{
			auto selVtHd=shl.GetSelectedVertex();
			auto selCeHd=shl.GetSelectedConstEdge();
			if(0==selVtHd.size() && 1==selCeHd.size())
			{
				auto ceVtHd=shl.GetConstEdgeVertex(selCeHd[0]);
				if(2<=ceVtHd.size())
				{
					vtHd.push_back(ceVtHd.front());
					vtHd.push_back(ceVtHd.back());
				}
			}
			else if(2==selVtHd.size() || 3==selVtHd.size() || 4==selVtHd.size())
			{
				vtHd=selVtHd;
			}
		}

		YsVec3 cen,axs;
		if(2==vtHd.size())
		{
			cen=shl.GetCenter(vtHd.size(),vtHd.data());
			axs=shl.GetEdgeVector(vtHd[0],vtHd[1]);
		}
		else if(3==vtHd.size())
		{
			cen=shl.GetCenter(vtHd.size(),vtHd.data());
			axs=(shl.GetVertexPosition(vtHd[2])-shl.GetCenter(vtHd[0],vtHd[1]));
		}
		else if(4==vtHd.size())
		{
			cen=shl.GetCenter(vtHd.size(),vtHd.data());
			axs=(shl.GetCenter(vtHd[2],vtHd[3])-shl.GetCenter(vtHd[0],vtHd[1]));
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_AXIS_VERTICESORCONSTEDGEREQUIRED);
			return;
		}

		if(YSOK==axs.Normalize())
		{
			// Now set it.
			auto parent=shlGrp.GetParent(slHd);
			auto thisTfm=dnmState.GetNodeToRootTransformation(slHd);
			auto parentTfm=dnmState.GetNodeToRootTransformation(parent);

			YsVec3 pivotInThis,pivotInParent,axisInThis;
			thisTfm.MulInverse(pivotInThis,cen,1.0);
			parentTfm.MulInverse(pivotInParent,cen,1.0);

			thisTfm.MulInverse(axisInThis,axs,0.0);

			slHd->posInParent=pivotInParent;
			slHd->localRotationCenter=pivotInThis;
			slHd->localRotationAxis=axisInThis;

			dnmState.GetState(slHd);

			dnmState.CacheTransformation(slHd);
			SetNeedRedraw(YSTRUE);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOTCALCULATEVECTOR);
		}
	}
}



class GeblGuiEditorBase::Dnm_TransformationDialog : public FsGuiDialog
{
public:
	GeblGuiEditorBase *canvasPtr;
	YsMatrix4x4 refTfm;
	YsVec3 refPos;
	YsAtt3 refAtt;
	bool captured;  // If captured by OK/Space key, pos&att shouldn't be reset on exit.  If not captured by Cancel/ESC, pos&att should be reset.
	FsGuiButton *okBtn,*cancelBtn;

	void Make(GeblGuiEditorBase *canvasPtr);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void GeblGuiEditorBase::Dnm_TransformationDialog::Make(GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();

	YsString str;
	if(0<=canvasPtr->dnmCurrentState)
	{
		str.Printf("Current DNM State=%d",canvasPtr->dnmCurrentState);
	}
	else
	{
		str="Current State=STATIC";
	}
	AddStaticText(0,FSKEY_NULL,str,YSTRUE);

	okBtn=    AddTextButton(MakeIdent("ok"),FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(MakeIdent("cancel"),FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
	captured=false;
	Fit();
}
/* virtual */ void GeblGuiEditorBase::Dnm_TransformationDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvasPtr->Dnm_Transformation_OK();
	}
	else if(btn==cancelBtn)
	{
		canvasPtr->Dnm_Transformation_Cancel();
	}
}

void GeblGuiEditorBase::Dnm_Rotate(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		if(YSTRUE!=dnmEditMode)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_MUSTBE_DNMEDIT_MODE);
			return;
		}

		Edit_ClearUIModeExceptDNMEditMode();

		auto state=dnmState.GetState(slHd); // This dummy call creates a state for this node if not yet exists.
		auto thisTfm=dnmState.GetNodeToRootTransformation(slHd);
		YsVec3 cen,axs;
		thisTfm.Mul(cen,slHd->localRotationCenter,1.0);
		thisTfm.Mul(axs,slHd->localRotationAxis,0.0);

		YsVec3 bbx[2];
		slHd->GetBoundingBox(bbx);
		auto dgn=(bbx[1]-bbx[0]).GetLength();

		const YSBOOL allowChangeAxis=YSTRUE;
		const YSBOOL allowMoveCenter=YSTRUE;
		const YSBOOL allowMirror=YSFALSE;
		threeDInterface.BeginInputRotation(axs,0.0,cen,dgn/2.0,allowChangeAxis,allowMoveCenter,allowMirror);
		threeDInterface.BindCallBack(&THISCLASS::Dnm_Rotate_RotationChanged,this);

		spaceKeyCallBack=std::bind(&THISCLASS::Dnm_Transformation_OK,this);
		modeCleanUpCallBack=std::bind(&THISCLASS::Dnm_Transformation_Cancel,this);

		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<Dnm_TransformationDialog>();
		dlg->Make(this);
		AddDialog(dlg);
		ArrangeDialog();

		dlg->refAtt=state.relAtt;
		dlg->refPos=state.relPos;
		dlg->refTfm=thisTfm;

		nextESCOnlyCancelDnmEditSubMode=YSTRUE;
	}
}

void GeblGuiEditorBase::Dnm_Rotate_RotationChanged(FsGui3DInterface *itfc)
{
	if(nullptr!=slHd)
	{
		auto dlgPtr=FindTypedModelessDialog<Dnm_TransformationDialog>();
		if(nullptr==dlgPtr)
		{
			printf("Lost dialog.\n");
		}

		YsMatrix4x4 rotTfm;
		itfc->GetRotationMatrix(rotTfm);

		auto &state=dnmState.GetState(slHd);
		state.relAtt=YsZeroAtt();
		dnmState.CacheTransformation(slHd);
		auto zeroTfm=dnmState.GetNodeToRootTransformation(slHd);

		state.relAtt=(nullptr!=dlgPtr ? dlgPtr->refAtt : YsZeroAtt());
		dnmState.CacheTransformation(slHd);
		auto refTfm=dnmState.GetNodeToRootTransformation(slHd);

		// Gets a bit confusing.
		// The rotation in the interface is from the state at the time when the user chose 'Rotate' menu.
		// The rotation that needs to be set to the state must be the state relative to the zero rotation.
		YsMatrix4x4 newTfm=refTfm*rotTfm;
		YsVec3 ev=YsZeroAtt().GetForwardVector();
		YsVec3 uv=YsZeroAtt().GetUpVector();

		newTfm.Mul(ev,ev,0.0);
		newTfm.Mul(uv,uv,0.0);

		zeroTfm.MulInverse(ev,ev,0.0);
		zeroTfm.MulInverse(uv,uv,0.0);

		state.relAtt.SetTwoVector(ev,uv);

		dnmState.CacheTransformation(slHd);
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Dnm_Translate(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		if(YSTRUE!=dnmEditMode)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_MUSTBE_DNMEDIT_MODE);
			return;
		}

		Edit_ClearUIModeExceptDNMEditMode();

		auto state=dnmState.GetState(slHd); // This dummy call creates a state for this node if not yet exists.
		auto thisTfm=dnmState.GetNodeToRootTransformation(slHd);
		YsVec3 cen,axs;
		thisTfm.Mul(cen,slHd->localRotationCenter,1.0);
		thisTfm.Mul(axs,slHd->localRotationAxis,0.0);

		const YSBOOL dirSwitch[]={YSTRUE,YSTRUE,YSTRUE};
		const YsVec3 dir[]={YsXVec(),YsYVec(),YsZVec()};
		threeDInterface.BeginInputPoint2(cen,dirSwitch,dir);
		threeDInterface.BindCallBack(&THISCLASS::Dnm_Translate_TranslationChanged,this);

		spaceKeyCallBack=std::bind(&THISCLASS::Dnm_Transformation_OK,this);
		modeCleanUpCallBack=std::bind(&THISCLASS::Dnm_Transformation_Cancel,this);

		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<Dnm_TransformationDialog>();
		dlg->Make(this);
		AddDialog(dlg);
		ArrangeDialog();

		dlg->refAtt=state.relAtt;
		dlg->refPos=state.relPos;
		dlg->refTfm=thisTfm;

		nextESCOnlyCancelDnmEditSubMode=YSTRUE;
	}
}

void GeblGuiEditorBase::Dnm_Translate_TranslationChanged(FsGui3DInterface *itfc)
{
	if(nullptr!=slHd)
	{
		auto dlgPtr=FindTypedModelessDialog<Dnm_TransformationDialog>();
		if(nullptr==dlgPtr)
		{
			printf("Lost dialog.\n");
		}

		auto &state=dnmState.GetState(slHd);

		// All TFM=ParentTfm*PosInParent*AttInParent*relPos*relAtt*(-localCenter)
		// Unknown relPos
		// Known: itfc->point_pos=TFM*localCenter
		// Therefore:
		//  inverse(ParentTfm*PosInParent*AttInParent)*itfc->point_pos*inv(relAtt)=relPos

		YsMatrix4x4 leftTfm;
		if(nullptr!=shlGrp.GetParent(slHd))
		{
			auto parentHd=shlGrp.GetParent(slHd);
			dnmState.CacheTransformation(parentHd);
			leftTfm=dnmState.GetNodeToRootTransformation(parentHd);
		}
		leftTfm.Translate(slHd->posInParent);
		leftTfm*=slHd->attInParent;
		leftTfm.Invert();

		leftTfm.Translate(itfc->point_pos);

		YsMatrix4x4 rightTfm;
		rightTfm*=state.relAtt;
		rightTfm.Invert();

		leftTfm*=rightTfm;

		state.relPos=leftTfm*YsVec3::Origin();

		dnmState.CacheTransformation(slHd);
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::Dnm_Transformation_OK(void)
{
	auto dlgPtr=FindTypedModelessDialog<Dnm_TransformationDialog>();
	if(nullptr==dlgPtr)
	{
		printf("Lost dialog.\n");
	}
	else
	{
		dlgPtr->captured=true; // Then Dnm_Rotate_Cancel will be called from Dnm_Capture through Edit_ClearUIModeExceptDNMEditMode.
	}
	Dnm_Transformation_Capture(slHd,dnmCurrentState);
}

void GeblGuiEditorBase::Dnm_Transformation_Cancel(void)
{
	if(nullptr!=slHd)
	{
		auto &state=dnmState.GetState(slHd);
		auto dlgPtr=FindTypedModelessDialog<Dnm_TransformationDialog>();
		if(nullptr==dlgPtr)
		{
			printf("Lost dialog.\n");
		}
		else if(true!=dlgPtr->captured)
		{
			state.relPos=dlgPtr->refPos;
			state.relAtt=dlgPtr->refAtt;

			dnmState.CacheTransformation(slHd);
			SetNeedRedraw(YSTRUE);
		}
	}
}

void GeblGuiEditorBase::Dnm_Transformation_Capture(YsShellDnmContainer <YsShellExtEditGui>::Node *slHd,int dnmStateIndex)
{
	if(nullptr!=slHd)
	{
		auto &state=dnmState.GetState(slHd);
		if(0>dnmStateIndex)
		{
			// This defines the configuration of when relPos and relAtt are both zero.
			// Which means it needs to calculate posInParent and attInParent
			// And, posInParent and attInParent may not be zero already.
			// Then, the total transformation must be current.PosInParent*current.AttInParent*state.Pos*state.Att

			YsMatrix4x4 tfm;
			tfm.Translate(slHd->posInParent);
			tfm.Rotate(slHd->attInParent);
			tfm.Translate(state.relPos);
			tfm.Rotate(state.relAtt);

			YsVec3 pos=YsVec3::Origin(),ev=YsZeroAtt().GetForwardVector(),uv=YsZeroAtt().GetUpVector();
			tfm.Mul(pos,pos,1.0);
			tfm.Mul(ev,ev,0.0);
			tfm.Mul(uv,uv,0.0);
			slHd->posInParent=pos;
			slHd->attInParent.SetTwoVector(ev,uv);

			state.relPos=YsVec3::Origin();
			state.relAtt=YsZeroAtt();
		}
		else
		{
			while(slHd->stateArray.size()<=dnmStateIndex)
			{
				slHd->stateArray.Increment();
				slHd->stateArray.Last().Initialize();
			}
			slHd->stateArray[dnmStateIndex]=state;
		}
		dnmState.CacheTransformation(slHd);
		SetNeedRedraw(YSTRUE);
		Edit_ClearUIModeExceptDNMEditMode();
	}
}


void GeblGuiEditorBase::Dnm_Show(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		if(YSTRUE!=dnmEditMode)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_MUSTBE_DNMEDIT_MODE);
			return;
		}
		if(0>this->dnmCurrentState)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_SHOW_HIDE_CANNOT_BE_SET_FOR_STATIC_STATE);
			return;
		}
		if(slHd->stateArray.size()<=this->dnmCurrentState)
		{
			auto i0=slHd->stateArray.size();
			slHd->stateArray.resize(this->dnmCurrentState+1);
			for(auto i=i0; i<slHd->stateArray.size(); ++i)
			{
				slHd->stateArray[i].Initialize();
			}
		}
		slHd->stateArray[this->dnmCurrentState].SetShow(YSTRUE);
		auto &state=dnmState.GetState(slHd);
		state.SetShow(YSTRUE);
		dnmState.CacheTransformation(slHd);
		SetNeedRedraw(YSTRUE);
	}
}
void GeblGuiEditorBase::Dnm_Hide(FsGuiPopUpMenuItem *)
{
	if(nullptr!=slHd)
	{
		if(YSTRUE!=dnmEditMode)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_MUSTBE_DNMEDIT_MODE);
			return;
		}
		if(0>this->dnmCurrentState)
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_DNM_SHOW_HIDE_CANNOT_BE_SET_FOR_STATIC_STATE);
			return;
		}
		if(slHd->stateArray.size()<=this->dnmCurrentState)
		{
			auto i0=slHd->stateArray.size();
			slHd->stateArray.resize(this->dnmCurrentState+1);
			for(auto i=i0; i<slHd->stateArray.size(); ++i)
			{
				slHd->stateArray[i].Initialize();
			}
		}
		slHd->stateArray[this->dnmCurrentState].SetShow(YSFALSE);
		auto &state=dnmState.GetState(slHd);
		state.SetShow(YSFALSE);
		dnmState.CacheTransformation(slHd);
		SetNeedRedraw(YSTRUE);
	}
}


////////////////////////////////////////////////////////////
#include <ysshelldnmident.h>
void GeblGuiEditorBase::Dnm_AutoSetState(FsGuiPopUpMenuItem *)
{
	YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *,16> allNode;
	shlGrp.GetNodePointerAll(allNode);
	for(YSSIZE_T nodeIdx=0; nodeIdx<allNode.GetN(); ++nodeIdx)
	{
		auto node=(YsShellDnmContainer <YsShellExtEditGui>::Node *)allNode[nodeIdx];
		switch(node->dnmClassType)
		{
		case YSDNM_CLASSID_AILERON:
			// Always the axis must point away from the Z-axis
			{
				if(3>node->stateArray.size())
				{
					node->stateArray.resize(3);
				}

				auto &state0=node->stateArray[0];
				state0.relPos=YsVec3::Origin();
				state0.relAtt=YsZeroAtt();

				YsRotation ailRot(node->localRotationAxis,YsPi/9.0);
				YsVec3 ev=YsZeroAtt().GetForwardVector();
				YsVec3 uv=YsZeroAtt().GetUpVector();
				ailRot.RotateNegative(ev,ev);
				ailRot.RotateNegative(uv,uv);

				auto &state1=node->stateArray[1];
				state1.relAtt.SetTwoVector(ev,uv);

				ev=YsZeroAtt().GetForwardVector();
				uv=YsZeroAtt().GetUpVector();
				ailRot.RotatePositive(ev,ev);
				ailRot.RotatePositive(uv,uv);

				auto &state2=node->stateArray[2];
				state2.relAtt.SetTwoVector(ev,uv);

				dnmState.CacheTransformation(node);
			}
			break;
		case YSDNM_CLASSID_ELEVATOR:
			{
				if(3>node->stateArray.size())
				{
					node->stateArray.resize(3);
				}
				auto &state0=node->stateArray[0];
				state0.relPos=YsVec3::Origin();
				state0.relAtt=YsZeroAtt();

				YsRotation ailRot(node->localRotationAxis,-YsPi/18.0);
				YsVec3 ev=YsZeroAtt().GetForwardVector();
				YsVec3 uv=YsZeroAtt().GetUpVector();
				ailRot.RotateNegative(ev,ev);
				ailRot.RotateNegative(uv,uv);

				auto &state1=node->stateArray[1];
				state1.relAtt.SetTwoVector(ev,uv);

				ev=YsZeroAtt().GetForwardVector();
				uv=YsZeroAtt().GetUpVector();
				ailRot.RotatePositive(ev,ev);
				ailRot.RotatePositive(uv,uv);

				auto &state2=node->stateArray[2];
				state2.relAtt.SetTwoVector(ev,uv);

				dnmState.CacheTransformation(node);
			}
			break;
		case YSDNM_CLASSID_RUDDER:
			{
				if(3>node->stateArray.size())
				{
					node->stateArray.resize(3);
				}
				auto &state0=node->stateArray[0];
				state0.relPos=YsVec3::Origin();
				state0.relAtt=YsZeroAtt();

				YsRotation rot(node->localRotationAxis,-YsPi/18.0);
				YsVec3 ev=YsZeroAtt().GetForwardVector();
				YsVec3 uv=YsZeroAtt().GetUpVector();
				rot.RotateNegative(ev,ev);
				rot.RotateNegative(uv,uv);

				auto &state1=node->stateArray[1];
				state1.relAtt.SetTwoVector(ev,uv);

				ev=YsZeroAtt().GetForwardVector();
				uv=YsZeroAtt().GetUpVector();
				rot.RotatePositive(ev,ev);
				rot.RotatePositive(uv,uv);

				auto &state2=node->stateArray[2];
				state2.relAtt.SetTwoVector(ev,uv);

				dnmState.CacheTransformation(node);
			}
			break;
		case YSDNM_CLASSID_FLAP:
			{
				if(3>node->stateArray.size())
				{
					node->stateArray.resize(2);
				}
				auto &state0=node->stateArray[0];
				state0.relPos=YsVec3::Origin();
				state0.relAtt=YsZeroAtt();

				YsRotation ailRot(node->localRotationAxis,YsPi/6.0);
				YsVec3 ev=YsZeroAtt().GetForwardVector();
				YsVec3 uv=YsZeroAtt().GetUpVector();
				ailRot.RotateNegative(ev,ev);
				ailRot.RotateNegative(uv,uv);

				auto &state1=node->stateArray[1];
				state1.relAtt.SetTwoVector(ev,uv);

				dnmState.CacheTransformation(node);
			}
			break;
		case YSDNM_CLASSID_AFTERBURNER:
			{
				if(2>node->stateArray.size())
				{
					node->stateArray.resize(2);
				}
				node->stateArray[0].SetShow(YSFALSE);
				node->stateArray[1].SetShow(YSTRUE);
			}
			break;
		}
	}
	Edit_ClearUIModeExceptDNMEditMode();
}
