#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysglslcpp.h>
#include <ysport.h>

#include "../fsgui3dapp.h"
#include "../textresource.h"



void FsGui3DMainCanvas::Edit_ClearUIMode(FsGuiPopUpMenuItem *)
{
	if(nullptr!=modeEndingFunction)
	{
		modeEndingFunction();
		modeEndingFunction=nullptr;
	}

	draw3dCallBack=nullptr;
	markUpPointSelectionCallback=nullptr;

	threeDInterface.ClearCallBack();
	threeDInterface.Initialize();

	TurnOffDrawingMode();

	showDiff=YSFALSE;

	mode=NORMAL;
	defAction=DEFAULTACTION_STANDBY;
	insertMarkUp_Status=INSERTMARKUP_STANDBY;
	insertMarkUp_Point.CleanUp();
	SetNeedRedraw(YSTRUE);

	EnableDefaultAction();

	world.ReadyVbo(GetCurrentField());

	RemoveAllNonPermanentDialog();
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::Edit_Undo(FsGuiPopUpMenuItem *)
{
	auto newCurrentFdHd=world.Undo();
	if(nullptr!=newCurrentFdHd)
	{
		SetCurrentField(newCurrentFdHd);
	}
	world.RemakeStringTexture(GetCurrentField());
	Edit_ClearUIMode(nullptr);
}

void FsGui3DMainCanvas::Edit_Redo(FsGuiPopUpMenuItem *)
{
	auto newCurrentFdHd=world.Redo();
	if(nullptr!=newCurrentFdHd)
	{
		SetCurrentField(newCurrentFdHd);
	}
	world.RemakeStringTexture(GetCurrentField());
	Edit_ClearUIMode(nullptr);
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::Edit_Copy(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto fieldPtr=world.GetField(fdHd);
		auto selMpHd=world.SelectedMapPiece(fdHd);
		auto selMuHd=world.SelectedMarkUp(fdHd);
		if(1<=selMpHd.size() || 1<=selMuHd.size())
		{
			clipboard.CleanUp();

			{
				YsKeyStore selMpKey;
				for(auto mpHd : selMpHd)
				{
					selMpKey.Add(fieldPtr->GetSearchKey(mpHd));
				}
				for(auto mpHd : fieldPtr->AllMapPiece())
				{
					if(YSTRUE==selMpKey.IsIncluded(fieldPtr->GetSearchKey(mpHd)))
					{
						clipboard.Add(world,fdHd,mpHd);
					}
				}
			}
			{
				YsKeyStore selMuKey;
				for(auto muHd : selMuHd)
				{
					selMuKey.Add(fieldPtr->GetSearchKey(muHd));
				}
				for(auto muHd : fieldPtr->AllMarkUp())
				{
					if(YSTRUE==selMuKey.IsIncluded(fieldPtr->GetSearchKey(muHd)))
					{
						clipboard.Add(world,fdHd,muHd);
					}
				}
			}
		}
	}
}
void FsGui3DMainCanvas::Edit_Paste(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto cen=clipboard.GetBoundingBoxCenter();
		SnapToUnit(cen);

		auto o=drawEnv.GetViewPosition(),v=drawEnv.GetViewDirection();

		YsVec2i pos=YsVec2i::Origin();
		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			pos.Set((int)crs.x(),(int)crs.y());
		}
		SnapToUnit(pos);

		auto offset=pos-cen;


		auto pasted=world.Paste(fdHd,clipboard,offset);
		world.SelectMapPiece(fdHd,pasted.mapPiece);
		world.SelectMarkUp(fdHd,pasted.markUp);

		for(auto muHd : pasted.markUp)
		{
			world.RemakeStringTexture(fdHd,muHd);
		}

		world.ReadyVbo(fdHd);

		UpdateNumBmp();
		SetNeedRedraw(YSTRUE);
	}
}

////////////////////////////////////////////////////////////

class FsGui3DMainCanvas::TrimDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiButton *trimThisOnlyBtn,*trimThisAndNewBtn,*trimAllBtn,*cancelBtn;
	FsGuiButton *useUnitBtn;

	void Make(void);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGui3DMainCanvas::Edit_Trim(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		Edit_ClearUIMode(nullptr);

		auto fieldPtr=world.GetField(fdHd);
		auto selMpHd=world.SelectedMapPiece(fdHd);
		if(1<=selMpHd.GetN())
		{
			DisableDefaultAction();

			auto mapPiecePtr=fieldPtr->GetMapPiece(selMpHd[0]);
			auto rect=mapPiecePtr->GetBoundingBox();

			threeDInterface.BeginInputBox(rect[0],rect[1],YSTRUE,YSFALSE);
			threeDInterface.BindCallBack(&THISCLASS::Edit_Trim_3DInterfaceCallBack,this);

			draw3dCallBack=std::bind(&THISCLASS::Edit_Trim_DrawCallBack,this);

			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<TrimDialog>();
			dlg->canvasPtr=this;
			dlg->Make();
			AddDialog(dlg);
			ArrangeDialog();
		}
		else
		{
			auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			msgDlg->Make(FSGUI_COMMON_ERROR,FSGUI_DLG_COMMON_NEED_SELECT_ONE_MAPPIECE,FSGUI_COMMON_OK,nullptr);
			AttachModalDialog(msgDlg);
		}
	}
}

void FsGui3DMainCanvas::Edit_Trim_DrawCallBack(void)
{
	auto rect=Edit_CurrentTrimRect();

	YsGLVertexBuffer vtxBuf;
	vtxBuf.Add(YsVec3(rect[0].x(),rect[0].y(),0.0));
	vtxBuf.Add(YsVec3(rect[1].x(),rect[0].y(),0.0));
	vtxBuf.Add(YsVec3(rect[1].x(),rect[1].y(),0.0));
	vtxBuf.Add(YsVec3(rect[0].x(),rect[1].y(),0.0));

	YsGLSLPlain3DRenderer renderer;
	renderer.SetTextureType(YSGLSL_TEX_TYPE_NONE);
	renderer.SetUniformColor(YsGreen());
	renderer.DrawVtx(YsGL::LINE_LOOP,vtxBuf.GetN(),vtxBuf);
}

void FsGui3DMainCanvas::Edit_Trim_3DInterfaceCallBack(FsGui3DInterface *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		if(1<=selMpHd.GetN())
		{
			auto trimRect=Edit_CurrentTrimRect();

			for(auto mpHd : selMpHd)
			{
				auto mapPiecePtr=world.GetMapPiece(fdHd,mpHd);
				auto shape=mapPiecePtr->GetShape();
				shape.visibleArea=trimRect;
				world.ReadyVbo(fdHd,mpHd,shape,1.0);
			}
		}
	}
}

YsRect2i FsGui3DMainCanvas::Edit_CurrentTrimRect(void)
{
	YsRect2i trimRect(YsVec2i::Origin(),YsVec2i::Origin());

	auto dlgPtr=this->FindTypedModelessDialog<TrimDialog>();

	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		auto fieldPtr=world.GetField(fdHd);
		if(1<=selMpHd.GetN())
		{
			auto min3d=threeDInterface.box_pos[0];
			auto max3d=threeDInterface.box_pos[1];

			auto min=min3d.xy(),max=max3d.xy();
			if(nullptr!=dlgPtr && YSTRUE==dlgPtr->useUnitBtn->GetCheck())
			{
				auto unit=world.GetUnit();
				if(1<unit.x())
				{
					min.SetX((double)unit.x()*floor(0.5+min.x()/(double)unit.x()));
					max.SetX((double)unit.x()*floor(0.5+max.x()/(double)unit.x()));
				}
				if(1<unit.y())
				{
					min.SetY((double)unit.y()*floor(0.5+min.y()/(double)unit.y()));
					max.SetY((double)unit.y()*floor(0.5+max.y()/(double)unit.y()));
				}
			}

			auto mapPiecePtr=fieldPtr->GetMapPiece(selMpHd[0]);
			trimRect=mapPiecePtr->GetShape().VisibleAreaFromWorldXY(min,max);
		}
	}

	return trimRect;
}

void FsGui3DMainCanvas::TrimDialog::Make(void)
{
	FsGuiDialog::Initialize();
	trimThisAndNewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_TRIM_TRIM_THIS_AND_NEW,YSFALSE);
	trimThisOnlyBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_TRIM_THIS_MAPPIECE_ONLY,YSFALSE);
	trimAllBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_TRIM_APPLY_ALL,YSFALSE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
	useUnitBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_TRIM_USE_UNIT,YSTRUE);
	useUnitBtn->SetCheck(YSTRUE);
	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
}
/* virtual */ void FsGui3DMainCanvas::TrimDialog::OnButtonClick(FsGuiButton *btn)
{
	if(cancelBtn==btn)
	{
		canvasPtr->Edit_ClearUIMode(nullptr);
		return;
	}
	auto fdHd=canvasPtr->GetCurrentField();
	auto selMpHd=canvasPtr->world.SelectedMapPiece(fdHd);
	if(nullptr!=fdHd && 1<=selMpHd.GetN())
	{
		RetroMap_World::UndoGuard ug(canvasPtr->world);

		auto fieldPtr=canvasPtr->world.GetField(fdHd);
		auto trimRect=canvasPtr->Edit_CurrentTrimRect();
		if(trimThisOnlyBtn==btn)
		{
			for(auto mpHd : selMpHd)
			{
				canvasPtr->world.SetTrimRect(fdHd,mpHd,trimRect);
				canvasPtr->world.ReadyVbo(fdHd,mpHd);
			}
			canvasPtr->Edit_ClearUIMode(nullptr);
		}
		if(trimThisAndNewBtn==btn)
		{
			for(auto mpHd : selMpHd)
			{
				canvasPtr->world.SetTrimRect(fdHd,mpHd,trimRect);
				canvasPtr->world.ReadyVbo(fdHd,mpHd);
			}
			canvasPtr->world.SetDefaultTrimRect(trimRect);
			canvasPtr->Edit_ClearUIMode(nullptr);
		}
		if(trimAllBtn==btn)
		{
			for(auto mpHd : canvasPtr->world.AllMapPiece(fdHd))
			{
				canvasPtr->world.SetTrimRect(fdHd,mpHd,trimRect);
				canvasPtr->world.ReadyVbo(fdHd,mpHd);
			}
			canvasPtr->world.SetDefaultTrimRect(trimRect);
			canvasPtr->Edit_ClearUIMode(nullptr);
		}
	}
}

////////////////////////////////////////////////////////////

class FsGui3DMainCanvas::ScalingDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiButton *scaleThisButton,*scaleAllButton,*scaleThisAndNewButton;

	void Make(FsGui3DMainCanvas *canvasPtr);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGui3DMainCanvas::ScalingDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;
	scaleThisAndNewButton=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SCALING_THIS_AND_NEW,YSTRUE);
	scaleThisButton=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SCALING_THIS_ONLY,YSFALSE);
	scaleAllButton=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SCALING_ALL,YSFALSE);
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	Fit();
}
/* virtual */ void FsGui3DMainCanvas::ScalingDialog::OnButtonClick(FsGuiButton *btn)
{
	auto newScaling=canvasPtr->Edit_CurrentScaling();
	auto newPos=canvasPtr->Edit_CurrentPositionForScaling();

	if(btn==scaleThisButton)
	{
		RetroMap_World::UndoGuard ug(canvasPtr->world);

		auto fdHd=canvasPtr->GetCurrentField();
		if(nullptr!=fdHd)
		{
			for(auto mpHd : canvasPtr->world.SelectedMapPiece(fdHd))
			{
				canvasPtr->world.SetScaling(fdHd,mpHd,newScaling);
				canvasPtr->world.SetPosition(fdHd,mpHd,newPos);
			}
		}
		canvasPtr->RemoveDialog(this);
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
	else if(btn==scaleThisAndNewButton)
	{
		RetroMap_World::UndoGuard ug(canvasPtr->world);

		auto fdHd=canvasPtr->GetCurrentField();
		if(nullptr!=fdHd)
		{
			for(auto mpHd : canvasPtr->world.SelectedMapPiece(fdHd))
			{
				canvasPtr->world.SetScaling(fdHd,mpHd,newScaling);
				canvasPtr->world.SetPosition(fdHd,mpHd,newPos);
			}
		}
		canvasPtr->world.SetDefaultScaling(newScaling);
		canvasPtr->RemoveDialog(this);
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
	else if(btn==scaleAllButton)
	{
		RetroMap_World::UndoGuard ug(canvasPtr->world);

		auto fdHd=canvasPtr->GetCurrentField();
		if(nullptr!=fdHd)
		{
			for(auto mpHd : canvasPtr->world.SelectedMapPiece(fdHd))
			{
				canvasPtr->world.SetPosition(fdHd,mpHd,newPos);
			}
			for(auto mpHd : canvasPtr->world.AllMapPiece(fdHd))
			{
				canvasPtr->world.SetScaling(fdHd,mpHd,newScaling);
			}
		}
		canvasPtr->world.SetDefaultScaling(newScaling);
		canvasPtr->RemoveDialog(this);
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
}

void FsGui3DMainCanvas::Edit_Scaling(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		Edit_ClearUIMode(nullptr);

		auto fieldPtr=world.GetField(fdHd);
		auto selMpHd=world.SelectedMapPiece(fdHd);
		if(1==selMpHd.GetN())
		{
			DisableDefaultAction();

			auto mapPiecePtr=fieldPtr->GetMapPiece(selMpHd[0]);
			auto rect=mapPiecePtr->GetBoundingBox();

			threeDInterface.BeginInputBox(rect[0],rect[1],YSTRUE,YSTRUE);
			threeDInterface.EnableMaintainAspectRatioButton();
			threeDInterface.BindCallBack(&THISCLASS::Edit_Scaling_3DInterfaceCallBack,this);

			{
				auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
				dlg->Make(L"",L"I'm still working on this functionality.  It doesn't work yet.",FSGUI_COMMON_OK,L"");
				AttachModalDialog(dlg);
			}

			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<ScalingDialog>();
			dlg->Make(this);
			AddDialog(dlg);
			ArrangeDialog();
		}
		else if(1<selMpHd.GetN())
		{
			auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			msgDlg->Make(FSGUI_COMMON_ERROR,FSGUI_DLG_COMMON_TOO_MANY_SELECTED,FSGUI_COMMON_OK,nullptr);
			AttachModalDialog(msgDlg);
		}
		else
		{
			auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			msgDlg->Make(FSGUI_COMMON_ERROR,FSGUI_DLG_COMMON_NEED_SELECT_ONE_MAPPIECE,FSGUI_COMMON_OK,nullptr);
			AttachModalDialog(msgDlg);
		}
	}
}
YsVec2 FsGui3DMainCanvas::Edit_CurrentScaling(void)
{
	YsVec2 s(1,1);

	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		auto fieldPtr=world.GetField(fdHd);
		if(1==selMpHd.GetN())
		{
			auto min=threeDInterface.box_pos[0];
			auto max=threeDInterface.box_pos[1];

			auto mapPiecePtr=fieldPtr->GetMapPiece(selMpHd[0]);
			s=mapPiecePtr->GetShape().ScalingFromWorldXY(min.xy(),max.xy());
		}
	}

	return s;
}
YsVec2i FsGui3DMainCanvas::Edit_CurrentPositionForScaling(void)
{
	YsVec2i p(0,0);

	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		auto fieldPtr=world.GetField(fdHd);
		if(1==selMpHd.GetN())
		{
			auto min=threeDInterface.box_pos[0];
			auto max=threeDInterface.box_pos[1];

			auto mapPiecePtr=fieldPtr->GetMapPiece(selMpHd[0]);
			auto shape=mapPiecePtr->GetShape();
			shape.scaling=Edit_CurrentScaling();
			p=shape.PositionFromWorldXY(min.xy());
		}
	}

	return p;
}
void FsGui3DMainCanvas::Edit_Scaling_3DInterfaceCallBack(FsGui3DInterface *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		if(1==selMpHd.GetN())
		{
			auto mapPiecePtr=world.GetMapPiece(fdHd,selMpHd[0]);
			auto shape=mapPiecePtr->GetShape();

			shape.scaling=Edit_CurrentScaling();
			shape.pos=Edit_CurrentPositionForScaling();

			world.ReadyVbo(fdHd,selMpHd[0],shape,1.0);
		}
	}
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::Edit_Delete(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		RetroMap_World::UndoGuard ug(world);

		world.DeleteMapPiece(fdHd,world.SelectedMapPiece(fdHd));
		world.DeleteMarkUp(fdHd,world.SelectedMarkUp(fdHd));
		UpdateNumBmp();
		SetNeedRedraw(YSTRUE);
	}
}

////////////////////////////////////////////////////////////

class FsGui3DMainCanvas::Edit_SearchDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiTextBox *keywordTxt;
	FsGuiButton *exactMatchBtn,*partialMatchBtn;
	FsGuiButton *caseSensitiveBtn;
	FsGuiButton *selectBtn,*closeBtn;

	void Make(FsGui3DMainCanvas *canvasPtr);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGui3DMainCanvas::Edit_SearchDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	FsGuiDialog::Initialize();

	this->canvasPtr=canvasPtr;
	keywordTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"",64,YSTRUE);
	exactMatchBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_MENU_EDIT_SEARCH_EXACT_MATCH,YSTRUE);
	partialMatchBtn=AddTextButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_MENU_EDIT_SEARCH_PARTIAL_MATCH,YSFALSE);

	exactMatchBtn->SetCheck(YSTRUE);

	YsArray <FsGuiButton *> radio;
	radio.Add(exactMatchBtn);
	radio.Add(partialMatchBtn);
	SetRadioButtonGroup(radio);

	caseSensitiveBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_SEARCH_CASE_SENSITIVE,YSFALSE);

	selectBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_DLG_SEARCH_SELECT,YSTRUE);
	closeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSFALSE);

	SetArrangeType(FSDIALOG_ARRANGE_BOTTOM_LEFT);
	Fit();
}
/* virtual */ void FsGui3DMainCanvas::Edit_SearchDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==selectBtn)
	{
		auto fdHd=canvasPtr->GetCurrentField();
		auto fieldPtr=canvasPtr->world.GetField(fdHd);
		auto keyword=keywordTxt->GetWString();
		if(nullptr!=fieldPtr && 0<keyword.Strlen())
		{
			YsArray <RetroMap_World::MarkUpHandle> selMuHd;
			for(auto muHd : fieldPtr->AllMarkUp())
			{
				auto markUpPtr=fieldPtr->GetMarkUp(muHd);
				if(markUpPtr->GetMarkUpType()==markUpPtr->MARKUP_STRING)
				{
					auto str=markUpPtr->GetMarkUpText();
					YSBOOL match=YSFALSE;
					if(YSTRUE==exactMatchBtn->GetCheck())
					{
						if(YSTRUE==caseSensitiveBtn->GetCheck())
						{
							match=(0==str.Strcmp(keyword) ? YSTRUE : YSFALSE);
						}
						else
						{
							match=(0==str.STRCMP(keyword) ? YSTRUE : YSFALSE);
						}
					}
					else if(YSTRUE==partialMatchBtn->GetCheck())
					{
						if(YSTRUE==caseSensitiveBtn->GetCheck())
						{
							match=str.FindWord(nullptr,keyword);
						}
						else
						{
							match=str.FINDWORD(nullptr,keyword);
						}
					}
					if(YSTRUE==match)
					{
						selMuHd.Add(muHd);
					}
				}
			}
			canvasPtr->world.SelectMarkUp(fdHd,selMuHd);
			canvasPtr->SetNeedRedraw(YSTRUE);
		}
	}
	if(btn==closeBtn)
	{
		canvasPtr->RemoveDialog(this);
	}
}

void FsGui3DMainCanvas::Edit_Search(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<Edit_SearchDialog>();
	dlg->Make(this);
	AddDialog(dlg);
	ArrangeDialog();
}


