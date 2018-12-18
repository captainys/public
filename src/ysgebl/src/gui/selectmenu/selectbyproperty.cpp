/* ////////////////////////////////////////////////////////////

File Name: selectbyproperty.cpp
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

#include <ysshellextmisc.h>
#include <ysshellext_geomcalc.h>

#include "../ysgebl_gui_editor_base.h"
#include "../textresource.h"


class PolyCreSelectByPropertyDialog : public FsGuiDialog
{
private:
	GeblGuiEditorBase *canvasPtr;
	FsGuiTextBox *propTxt;

	YsArray <FsGuiButton *> radioBtnGrp;
	FsGuiButton *vtxKeyBtn,*vtxIdBtn,*plgKeyBtn,*plgIdBtn;
	FsGuiButton *highAspectPlgBtn;
	FsGuiButton *ceIdBtn,*ceLabelBtn,*shortCeBtn,*fgIdBtn,*fgLabelBtn;
	FsGuiButton *ceKeyBtn,*fgKeyBtn;

	FsGuiButton *selectBtn,*addSelectBtn,*removeSelectBtn,*clearBtn,*closeBtn;

	YsShellVertexStore vtSet;
	YsShellPolygonStore plSet;
	YsShellExt::ConstEdgeStore ceSet;
	YsShellExt::FaceGroupStore fgSet;

public:
	enum SELECTION_MODE
	{
		MODE_NONE,
		MODE_VTXKEY,
		MODE_VTXID,
		MODE_PLGKEY,
		MODE_PLGID,
		MODE_CEID,
		MODE_CELABEL,
		MODE_FGID,
		MODE_FGLABEL,
		MODE_HIGHASPECTPLG,
	};
	enum ACTION_TYPE
	{
		NEW_SELECTION,
		ADD_SELECTION,
		REMOVE_SELECTION
	};

	PolyCreSelectByPropertyDialog();
	void Make(GeblGuiEditorBase *canvasPtr);
	void SetMode(SELECTION_MODE selMode);

	virtual void OnButtonClick(FsGuiButton *btn);
	YSRESULT WordToSet(const char wd[]);
};

PolyCreSelectByPropertyDialog::PolyCreSelectByPropertyDialog()
{
}
void PolyCreSelectByPropertyDialog::Make(GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();
	radioBtnGrp.CleanUp();


	propTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"",80,YSTRUE);

	radioBtnGrp.Append(vtxIdBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_VTXIDBTN,YSTRUE));
	radioBtnGrp.Append(plgIdBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_PLGIDBTN,YSFALSE));
	radioBtnGrp.Append(ceIdBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_CEBYID,YSFALSE));
	radioBtnGrp.Append(ceLabelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_CEBYLABEL,YSFALSE));
	radioBtnGrp.Append(shortCeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_SHORTCONSTEDGE,YSFALSE));
	radioBtnGrp.Append(fgIdBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_FGBYID,YSFALSE));
	radioBtnGrp.Append(fgLabelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_FGBYLABEL,YSFALSE));

	radioBtnGrp.Add(highAspectPlgBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_HIGHASPECTPOLYGON,YSTRUE));

	radioBtnGrp.Append(vtxKeyBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_VTKEYBTN,YSTRUE));
	radioBtnGrp.Append(plgKeyBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_PLGKEYBTN,YSFALSE));
	radioBtnGrp.Append(ceKeyBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_CEKEY,YSFALSE));
	radioBtnGrp.Append(fgKeyBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_SELECTBYPROP_FGKEY,YSFALSE));


	SetRadioButtonGroup(radioBtnGrp.GetN(),radioBtnGrp);


	selectBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SELECTBYPROP_SELECTBTN,YSTRUE);
	addSelectBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SELECTBYPROP_ADDSELECTBTN,YSFALSE);
	removeSelectBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SELECTBYPROP_REMOVESELECTBTN,YSFALSE);
	clearBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLEAR,YSFALSE);
	closeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSFALSE);


	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetBackgroundAlpha(0.3);
	Fit();
}
void PolyCreSelectByPropertyDialog::SetMode(SELECTION_MODE selMode)
{
	for(auto btnPtr : radioBtnGrp)
	{
		btnPtr->SetCheck(YSFALSE);
	}

	switch(selMode)
	{
	default:
		break;
	case MODE_VTXKEY:
		vtxKeyBtn->SetCheck(YSTRUE);
		break;
	case MODE_VTXID:
		vtxIdBtn->SetCheck(YSTRUE);
		break;
	case MODE_PLGKEY:
		plgKeyBtn->SetCheck(YSTRUE);
		break;
	case MODE_PLGID:
		plgIdBtn->SetCheck(YSTRUE);
		break;
	case MODE_CEID:
		ceIdBtn->SetCheck(YSTRUE);
		break;
	case MODE_CELABEL:
		ceLabelBtn->SetCheck(YSTRUE);
		break;
	case MODE_FGID:
		fgIdBtn->SetCheck(YSTRUE);
		break;
	case MODE_FGLABEL:
		fgLabelBtn->SetCheck(YSTRUE);
		break;
	case MODE_HIGHASPECTPLG:
		highAspectPlgBtn->SetCheck(YSTRUE);
		break;
	}
}
/* virtual */ void PolyCreSelectByPropertyDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==closeBtn)
	{
		canvasPtr->RemoveDialog(this);
	}
	else if(btn==clearBtn)
	{
		propTxt->SetText(L"");
	}
	else if((btn==selectBtn || btn==addSelectBtn || btn==removeSelectBtn) && NULL!=canvasPtr->Slhd())
	{
		auto &shl=*(canvasPtr->Slhd());

		vtSet.SetShell(shl.Conv());
		vtSet.CleanUp();
		plSet.SetShell(shl.Conv());
		plSet.CleanUp();
		ceSet.SetShell(shl.Conv());
		ceSet.CleanUp();
		fgSet.SetShell(shl.Conv());
		fgSet.CleanUp();


		ACTION_TYPE actionType=NEW_SELECTION;
		if(btn==addSelectBtn)
		{
			actionType=ADD_SELECTION;
		}
		else if(btn==removeSelectBtn)
		{
			actionType=REMOVE_SELECTION;
		}

		auto str=propTxt->GetString();
		YsArray <YsString,16> args;
		str.Arguments(args);


		if(YSTRUE==highAspectPlgBtn->GetCheck() && 1<args.GetN())
		{
			auto msg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
			msg->Make(L"Error",L"Only one arguments allowed for this property.",L"Close",NULL);
			AttachModalDialog(msg);
			return;
		}


		// YSBOOL first=YSTRUE;
		for(auto &argv : args)
		{
			if(YSOK!=WordToSet(argv))
			{
				auto msg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
				msg->Make(L"Error",L"Unhandled property type(1)",L"Close",NULL);
				AttachModalDialog(msg);
				return;
			}
		}


		if(YSTRUE==vtxKeyBtn->GetCheck() || YSTRUE==vtxIdBtn->GetCheck())
		{
			auto selVtHd=shl.GetSelectedVertex();
			auto nSelVtHd=selVtHd.GetN();
			if(NEW_SELECTION==actionType)
			{
				selVtHd.CleanUp();
			}
			switch(actionType)
			{
			case NEW_SELECTION:
			case ADD_SELECTION:
				for(auto vtHd : vtSet)
				{
					selVtHd.Append(vtHd);
				}
				break;
			case REMOVE_SELECTION:
				for(YSSIZE_T idx=selVtHd.GetN()-1; 0<=idx; --idx)
				{
					if(YSTRUE==vtSet.IsIncluded(selVtHd[idx]))
					{
						selVtHd.Delete(idx);
					}
				}
				break;
			}
			if(nSelVtHd!=selVtHd.GetN())
			{
				shl.SelectVertex(selVtHd);
				canvasPtr->needRemakeDrawingBuffer|=GeblGuiEditorBase::NEED_REMAKE_DRAWING_SELECTED_VERTEX;
				canvasPtr->SetNeedRedraw(YSTRUE);
			}
		}
		else if(YSTRUE==plgKeyBtn->GetCheck() || YSTRUE==plgIdBtn->GetCheck() || YSTRUE==highAspectPlgBtn->GetCheck())
		{
			auto selPlHd=shl.GetSelectedPolygon();
			auto nSelPlHd=selPlHd.GetN();
			if(NEW_SELECTION==actionType)
			{
				selPlHd.CleanUp();
			}
			switch(actionType)
			{
			case NEW_SELECTION:
			case ADD_SELECTION:
				for(auto plHd : plSet)
				{
					selPlHd.Append(plHd);
				}
				break;
			case REMOVE_SELECTION:
				for(YSSIZE_T idx=selPlHd.GetN()-1; 0<=idx; --idx)
				{
					if(YSTRUE==plSet.IsIncluded(selPlHd[idx]))
					{
						selPlHd.Delete(idx);
					}
				}
				break;
			}
			if(nSelPlHd!=selPlHd.GetN())
			{
				shl.SelectPolygon(selPlHd);
				canvasPtr->needRemakeDrawingBuffer|=GeblGuiEditorBase::NEED_REMAKE_DRAWING_SELECTED_POLYGON;
				canvasPtr->SetNeedRedraw(YSTRUE);
			}
		}
		else if(YSTRUE==ceLabelBtn->GetCheck() || YSTRUE==ceIdBtn->GetCheck() || YSTRUE==shortCeBtn->GetCheck() || YSTRUE==ceKeyBtn->GetCheck())
		{
			auto selCeHd=shl.GetSelectedConstEdge();
			auto nSelCeHd=selCeHd.GetN();
			if(NEW_SELECTION==actionType)
			{
				selCeHd.CleanUp();
			}
			switch(actionType)
			{
			case NEW_SELECTION:
			case ADD_SELECTION:
				for(auto ceHd : ceSet)
				{
					selCeHd.Append(ceHd);
				}
				break;
			case REMOVE_SELECTION:
				for(YSSIZE_T idx=selCeHd.GetN()-1; 0<=idx; --idx)
				{
					if(YSTRUE==ceSet.IsIncluded(selCeHd[idx]))
					{
						selCeHd.Delete(idx);
					}
				}
				break;
			}
			if(nSelCeHd!=selCeHd.GetN())
			{
				shl.SelectConstEdge(selCeHd);
				canvasPtr->needRemakeDrawingBuffer|=GeblGuiEditorBase::NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE;
				canvasPtr->SetNeedRedraw(YSTRUE);
			}
		}
		else if(YSTRUE==fgLabelBtn->GetCheck() || YSTRUE==fgIdBtn->GetCheck() || YSTRUE==fgKeyBtn->GetCheck())
		{
			auto selFgHd=shl.GetSelectedFaceGroup();
			auto nSelFgHd=selFgHd.GetN();
			if(NEW_SELECTION==actionType)
			{
				selFgHd.CleanUp();
			}
			switch(actionType)
			{
			case NEW_SELECTION:
			case ADD_SELECTION:
				for(auto fgHd : fgSet)
				{
					selFgHd.Append(fgHd);
				}
				break;
			case REMOVE_SELECTION:
				for(YSSIZE_T idx=selFgHd.GetN()-1; 0<=idx; --idx)
				{
					if(YSTRUE==fgSet.IsIncluded(selFgHd[idx]))
					{
						selFgHd.Delete(idx);
					}
				}
				break;
			}
			if(nSelFgHd!=selFgHd.GetN())
			{
				shl.SelectFaceGroup(selFgHd);
				canvasPtr->needRemakeDrawingBuffer|=GeblGuiEditorBase::NEED_REMAKE_DRAWING_SELECTED_FACEGROUP;
				canvasPtr->SetNeedRedraw(YSTRUE);
			}
		}
		else
		{
			auto msg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
			msg->Make(L"Error",L"Unhandled property type(2)",L"Close",NULL);
			AttachModalDialog(msg);
		}
	}
}

YSRESULT PolyCreSelectByPropertyDialog::WordToSet(const char wd[])
{
	auto &shl=*(canvasPtr->Slhd());

	if(YSTRUE==vtxKeyBtn->GetCheck())
	{
		YSHASHKEY key=(YSHASHKEY)atoi(wd);
		auto vtHd=shl.FindVertex(key);
		if(NULL!=vtHd)
		{
			vtSet.AddVertex(vtHd);
		}
	}
	else if(YSTRUE==vtxIdBtn->GetCheck())
	{
		int index=atoi(wd);
		auto vtHd=shl.GetVertexHandleFromId(index);
		if(NULL!=vtHd)
		{
			vtSet.AddVertex(vtHd);
		}
	}
	else if(YSTRUE==plgKeyBtn->GetCheck())
	{
		YSHASHKEY key=(YSHASHKEY)atoi(wd);
		auto plHd=shl.FindPolygon(key);
		if(NULL!=plHd)
		{
			plSet.AddPolygon(plHd);
		}
	}
	else if(YSTRUE==plgIdBtn->GetCheck())
	{
		int index=atoi(wd);
		auto plHd=shl.GetPolygonHandleFromId(index);
		if(NULL!=plHd)
		{
			plSet.AddPolygon(plHd);
		}
	}
	else if(YSTRUE==ceIdBtn->GetCheck())
	{
		int id=atoi(wd);
		for(auto ceHd : shl.AllConstEdge())
		{
			if(id==shl.GetConstEdgeIdent(ceHd))
			{
				ceSet.AddConstEdge(ceHd);
			}
		}
	}
	else if(YSTRUE==ceLabelBtn->GetCheck())
	{
		for(auto ceHd : shl.AllConstEdge())
		{
			if(0==strcmp(wd,shl.GetConstEdgeLabel(ceHd)))
			{
				ceSet.AddConstEdge(ceHd);
			}
		}
	}
	else if(YSTRUE==shortCeBtn->GetCheck())
	{
		auto thr=atof(wd);
		for(auto ceHd : shl.AllConstEdge())
		{
			if(YsShellExt_CalculateConstEdgeLength(shl.Conv(),ceHd)<thr)
			{
				ceSet.AddConstEdge(ceHd);
			}
		}
	}
	else if(YSTRUE==fgIdBtn->GetCheck())
	{
		int id=atoi(wd);
		for(auto fgHd : shl.AllFaceGroup())
		{
			if(id==shl.GetFaceGroupIdent(fgHd))
			{
				fgSet.AddFaceGroup(fgHd);
			}
		}
	}
	else if(YSTRUE==fgLabelBtn->GetCheck())
	{
		for(auto fgHd : shl.AllFaceGroup())
		{
			if(0==strcmp(wd,shl.GetFaceGroupLabel(fgHd)))
			{
				fgSet.AddFaceGroup(fgHd);
			}
		}
	}
	else if(YSTRUE==ceKeyBtn->GetCheck())
	{
		YSHASHKEY key=(YSHASHKEY)atoi(wd);
		auto ceHd=shl.FindConstEdge(key);
		if(NULL!=ceHd)
		{
			ceSet.AddConstEdge(ceHd);
		}
	}
	else if(YSTRUE==fgKeyBtn->GetCheck())
	{
		YSHASHKEY key=(YSHASHKEY)atoi(wd);
		auto fgHd=shl.FindFaceGroup(key);
		if(NULL!=fgHd)
		{
			fgSet.AddFaceGroup(fgHd);
		}
	}
	else if(YSTRUE==highAspectPlgBtn->GetCheck())
	{
		const double aspectThr=atof(wd);
		for(auto plHd : shl.AllPolygon())
		{
			double longestEdge,height;
			YsShellExt_CalculateAspectRatio(longestEdge,height,shl.Conv(),plHd);
			if(aspectThr*height<longestEdge) // aspectThr<l/h
			{
				plSet.Add(plHd);
			}
		}
	}
	else
	{
		return YSERR;
	}

	return YSOK;
}

////////////////////////////////////////////////////////////

class PolyCreSelectByPropertyDialog *GeblGuiEditorBase::PrepareSelectByPropDialog(void)
{
	auto dlg=FindTypedModelessDialog<PolyCreSelectByPropertyDialog>();
	if(NULL==dlg)
	{
		dlg=FsGuiDialog::CreateSelfDestructiveDialog<PolyCreSelectByPropertyDialog>();
		dlg->Make(this);
		AddDialog(dlg);
		ArrangeDialog();
	}
	return dlg;
}

void GeblGuiEditorBase::Select_ByProperty(FsGuiPopUpMenuItem *)
{
	auto dlg=PrepareSelectByPropDialog();
	dlg->SetMode(PolyCreSelectByPropertyDialog::MODE_VTXKEY);
}

void GeblGuiEditorBase::Select_VertexByHashKey(FsGuiPopUpMenuItem *)
{
	auto dlg=PrepareSelectByPropDialog();
	dlg->SetMode(PolyCreSelectByPropertyDialog::MODE_VTXKEY);
}

void GeblGuiEditorBase::Select_VertexByIndex(FsGuiPopUpMenuItem *)
{
	auto dlg=PrepareSelectByPropDialog();
	dlg->SetMode(PolyCreSelectByPropertyDialog::MODE_VTXID);
}

void GeblGuiEditorBase::Select_PolygonByHashKey(FsGuiPopUpMenuItem *)
{
}
void GeblGuiEditorBase::Select_PolygonByIndex(FsGuiPopUpMenuItem *)
{
}
void GeblGuiEditorBase::Select_FaceGroupByIdNumber(FsGuiPopUpMenuItem *)
{
}
void GeblGuiEditorBase::Select_FaceGroupByLabel(FsGuiPopUpMenuItem *)
{
}
void GeblGuiEditorBase::Select_ConstEdgeByIdNumber(FsGuiPopUpMenuItem *)
{
}
void GeblGuiEditorBase::Select_ConstEdgeByLabel(FsGuiPopUpMenuItem *)
{
}
