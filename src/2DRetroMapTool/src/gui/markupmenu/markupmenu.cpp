#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysport.h>
#include <ysglslcpp.h>

#include "../fsgui3dapp.h"
#include "../textresource.h"



////////////////////////////////////////////////////////////

class FsGui3DMainCanvas::CommentDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *owner;
	FsGuiTextBox *commentTxt;
	FsGuiTextBox *sizeTxt;
	FsGuiButton *insertBtn,*closeBtn;
	FsGuiColorPalette *foregroundPlt,*backgroundPlt;
	FsGuiButton *transparentBackGroundBtn;
	YSBOOL colorChanged;
	YSBOOL sizeChanged;

	YSBOOL modifying;

	CommentDialog();
	void Make(FsGui3DMainCanvas *owner);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
	virtual void OnColorPaletteChange(FsGuiColorPalette *plt);
};

FsGui3DMainCanvas::CommentDialog::CommentDialog()
{
	modifying=YSFALSE;
}

void FsGui3DMainCanvas::CommentDialog::Make(FsGui3DMainCanvas *owner)
{
	this->owner=owner;

	colorChanged=YSFALSE;
	sizeChanged=YSFALSE;

	FsGuiDialog::Initialize();
	commentTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"",64,YSTRUE);
	commentTxt->SetEatEnterKey(YSFALSE);
	if(YSTRUE!=modifying)
	{
		insertBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_INSERT,YSFALSE);
	}
	else
	{
		insertBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_APPLY,YSFALSE);
	}
	closeBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSFALSE);

	sizeTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_COMMENT_FONTSIZE,4,YSTRUE);
	sizeTxt->SetTextType(FSGUI_INTEGER);
	foregroundPlt=AddCompactColorPalette(0,FSKEY_NULL,"",255,255,255,YSFALSE);
	backgroundPlt=AddCompactColorPalette(0,FSKEY_NULL,"",0,0,0,YSFALSE);
	transparentBackGroundBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_COMMENT_TRANSPARENT_BACKGROUND,YSFALSE);

	sizeTxt->SetInteger(owner->world.GetDefaultFontSize());
	foregroundPlt->SetColor(owner->world.GetDefaultMarkUpForegroundColor());
	backgroundPlt->SetColor(owner->world.GetDefaultMarkUpBackgroundColor());
	if(owner->world.GetDefaultMarkUpBackgroundColor().Af()<0.5f)
	{
		transparentBackGroundBtn->SetCheck(YSTRUE);
	}
	else
	{
		transparentBackGroundBtn->SetCheck(YSFALSE);
	}

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	SetFocus(commentTxt);
	Fit();
}

/* virtual */ void FsGui3DMainCanvas::CommentDialog::OnButtonClick(FsGuiButton *btn)
{
	if(closeBtn==btn)
	{
		if(YSTRUE==modifying)
		{
			CloseModalDialog(YSERR);
		}
		else
		{
			owner->RemoveDialog(this);
		}
	}
	else if(insertBtn==btn)
	{
		RetroMap_World::UndoGuard ug(owner->world);

		YsWString str=commentTxt->GetWString();
		auto fontSize=YsGreater(8,sizeTxt->GetInteger());
		auto fgCol=foregroundPlt->GetColor();
		auto bgCol=backgroundPlt->GetColor();
		if(YSTRUE==transparentBackGroundBtn->GetCheck())
		{
			bgCol.SetAi(0);
		}

		auto fdHd=owner->GetCurrentFieldCreateIfNotExist();
		if(nullptr!=fdHd)
		{
			if(YSTRUE!=modifying)
			{
				if(0<str.Strlen())
				{
					auto pos=owner->GetPointToInsert();
					auto muHd=owner->world.CreateMarkUp(fdHd);
					owner->world.MakeStringMarkUp(fdHd,muHd,str);
					owner->world.SetStringMarkUpFontSize(fdHd,muHd,fontSize);
					owner->world.SetMarkUpForegroundColor(fdHd,muHd,fgCol);
					owner->world.SetMarkUpBackgroundColor(fdHd,muHd,bgCol);
					owner->world.SetPosition(fdHd,muHd,pos);
					owner->world.RemakeStringTexture(fdHd,muHd);
					owner->world.ReadyTexture(fdHd,muHd);
					owner->world.ReadyVbo(fdHd,muHd);
					commentTxt->SetText(L"");
					owner->world.UnselectAllMapPiece(fdHd);
					owner->world.UnselectAllMarkUp(fdHd);
					owner->world.AddSelectedMarkUp(fdHd,muHd);

				}
				owner->RemoveDialog(this);
			}
			else
			{
				auto selMuHd=owner->world.SelectedMarkUp(fdHd);
				for(auto muHd : selMuHd)
				{
					auto markUpPtr=owner->world.GetMarkUp(fdHd,muHd);
					if(1==selMuHd.GetN() && 0<str.Strlen())
					{
						owner->world.MakeStringMarkUp(fdHd,muHd,str);
					}
					if(RetroMap_World::MarkUp::MARKUP_STRING==markUpPtr->GetMarkUpType())
					{
						owner->world.SetStringMarkUpFontSize(fdHd,muHd,fontSize);
					}
					owner->world.SetMarkUpForegroundColor(fdHd,muHd,fgCol);
					owner->world.SetMarkUpBackgroundColor(fdHd,muHd,bgCol);
					owner->world.RemakeStringTexture(fdHd,muHd);
					owner->world.ReadyTexture(fdHd,muHd);
					owner->world.ReadyVbo(fdHd,muHd);
				}
				CloseModalDialog(YSERR);
			}
		}
	}
	else if(btn==transparentBackGroundBtn)
	{
		colorChanged=YSTRUE;
	}
}

/* virtual */ void FsGui3DMainCanvas::CommentDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(txt==sizeTxt)
	{
		sizeChanged=YSTRUE;
	}
}

/* virtual */ void FsGui3DMainCanvas::CommentDialog::OnColorPaletteChange(FsGuiColorPalette *plt)
{
	colorChanged=YSTRUE;
}

void FsGui3DMainCanvas::Edit_Insert_Comment(FsGuiPopUpMenuItem *)
{
	if(nullptr==commentDlg)
	{
		commentDlg=new CommentDialog;
	}
	commentDlg->Make(this);
	AddDialog(commentDlg);
	SetActiveDialog(commentDlg);
	ArrangeDialog();
}



////////////////////////////////////////////////////////////



void FsGui3DMainCanvas::MarkUp_SnapSwitch(FsGuiPopUpMenuItem *itm)
{
	auto c=itm->GetCheck();
	YsFlip(c);
	itm->SetCheck(c);
}

void FsGui3DMainCanvas::MarkUp_SnapToHalfUnit(YsVec3 &pos) const
{
	auto unit=world.GetUnit()/2; // Snap to the middle.
	if(1<unit.x())
	{
		double dx=unit.xd();
		pos.SetX(floor((pos.x()+dx*0.5)/dx)*dx);
	}
	if(1<unit.y())
	{
		double dy=unit.yd();
		pos.SetY(floor((pos.y()+dy*0.5)/dy)*dy);
	}
}

void FsGui3DMainCanvas::MarkUp_SnapToHalfUnit(YsVec2i &pos) const
{
	YsVec3 pos3;
	pos3.Set(pos.xd(),pos.yd(),0.0);
	MarkUp_SnapToHalfUnit(pos3);
	pos.Set((int)pos3.x(),(int)pos3.y());
}

void FsGui3DMainCanvas::MarkUp_Property(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto selMuHd=world.SelectedMarkUp(fdHd);
		if(0<selMuHd.GetN())
		{
			auto commentDlg=FsGuiDialog::CreateSelfDestructiveDialog<CommentDialog>();
			commentDlg->modifying=YSTRUE;
			commentDlg->Make(this);

			auto markUpPtr=world.GetMarkUp(fdHd,selMuHd[0]);
			auto fgCol=markUpPtr->GetFgColor();
			auto bgCol=markUpPtr->GetBgColor();
			commentDlg->foregroundPlt->SetColor(fgCol);
			commentDlg->backgroundPlt->SetColor(bgCol);
			if(bgCol.Ai()<128)
			{
				commentDlg->transparentBackGroundBtn->SetCheck(YSTRUE);
			}
			else
			{
				commentDlg->transparentBackGroundBtn->SetCheck(YSFALSE);
			}

			commentDlg->sizeTxt->SetInteger(markUpPtr->GetFontSize());

			if(1==selMuHd.GetN())
			{
				if(RetroMap_World::MarkUp::MARKUP_STRING==markUpPtr->GetMarkUpType())
				{
					commentDlg->commentTxt->SetText(markUpPtr->GetMarkUpText());
				}
				else
				{
					commentDlg->commentTxt->Disable();
				}
			}
			else
			{
				commentDlg->commentTxt->Disable();
			}

			AttachModalDialog(commentDlg);
			ArrangeDialog();
		}
	}
	else
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		dlg->Make(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEED_SELECT_MARKUP,FSGUI_COMMON_OK,nullptr);
		AttachModalDialog(dlg);
	}
}



void FsGui3DMainCanvas::MarkUp_OpenDrawingDialog(FsGuiPopUpMenuItem *)
{
	OpenDrawingDialog();
}



////////////////////////////////////////////////////////////



void FsGui3DMainCanvas::MarkUp_InsertLine(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode(nullptr);
	BindLButtonDownCallBack(&THISCLASS::MarkUp_InsertLine_OnLButtonDown,this);
	BindMouseMoveCallBack(&THISCLASS::MarkUp_InsertLine_OnMouseMove,this);
	BindLButtonUpCallBack(&THISCLASS::MarkUp_InsertLine_OnLButtonUp,this);
	BindTouchStateChangeCallBack(&THISCLASS::MarkUp_InsertLine_OnTouchStateChange,this);
	draw3dCallBack=std::bind(&THISCLASS::MarkUp_InsertLine_DrawCallBack,this);
	mode=DRAWING_LINE;

	SetNeedRedraw(YSTRUE);
}
void FsGui3DMainCanvas::MarkUp_InsertLineStrip(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode(nullptr);
	BindLButtonDownCallBack(&THISCLASS::MarkUp_InsertLine_OnLButtonDown,this);
	BindMouseMoveCallBack(&THISCLASS::MarkUp_InsertLine_OnMouseMove,this);
	BindLButtonUpCallBack(&THISCLASS::MarkUp_InsertLine_OnLButtonUp,this);
	BindTouchStateChangeCallBack(&THISCLASS::MarkUp_InsertLine_OnTouchStateChange,this);

	BindKeyCallBack(FSKEY_SPACE,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::MarkUp_InsertLineStrip_SpaceKeyCallBack,this);
	BindKeyCallBack(FSKEY_SPACE,YSFALSE,YSFALSE,YSTRUE, &THISCLASS::MarkUp_InsertLineStrip_SpaceKeyCallBack,this);

	BindKeyCallBack(FSKEY_BS,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::MarkUp_InsertLineStrip_BackSpaceKeyCallBack,this);
	BindKeyCallBack(FSKEY_BS,YSFALSE,YSFALSE,YSTRUE, &THISCLASS::MarkUp_InsertLineStrip_BackSpaceKeyCallBack,this);


	draw3dCallBack=std::bind(&THISCLASS::MarkUp_InsertLine_DrawCallBack,this);
	modeEndingFunction=std::bind(&THISCLASS::MarkUp_InsertLineStrip_EndUIMode,this);
	mode=DRAWING_LINE_STRIP;

	SetNeedRedraw(YSTRUE);
}

void FsGui3DMainCanvas::MarkUp_InsertLineStrip_SpaceKeyCallBack(FsGuiPopUpMenuItem *)
{
	if(2<=insertMarkUp_Point.size())
	{
		MarkUp_InsertLine_Finalize();
	}
}
void FsGui3DMainCanvas::MarkUp_InsertLineStrip_BackSpaceKeyCallBack(FsGuiPopUpMenuItem *)
{
	if(0<insertMarkUp_Point.size())
	{
		insertMarkUp_Point.pop_back();
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::MarkUp_InsertLineStrip_EndUIMode(void)
{
	if(2<=insertMarkUp_Point.size())
	{
		MarkUp_InsertLine_Finalize();
	}
}
void FsGui3DMainCanvas::MarkUp_InsertLine_DrawCallBack(void)
{
	if(INSERTMARKUP_DRAGGING==insertMarkUp_Status || DRAWING_LINE_STRIP==mode)
	{
		YsGLVertexBuffer vtxBuf;
		for(auto p : insertMarkUp_Point)
		{
			vtxBuf.Add(p);
		}

		YsGLSLPlain3DRenderer renderer;
		renderer.SetUniformColor(drawingDlg->lineColor->GetColor());
		renderer.DrawVtx(YsGL::LINE_STRIP,vtxBuf.GetN(),vtxBuf);
	}
}
YSRESULT FsGui3DMainCanvas::MarkUp_InsertLine_OnLButtonDown(FsGuiMouseButtonSet,YsVec2i pos)
{
	YsVec3 o,v;
	drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());
	YsPlane pln(YsOrigin(),YsZVec());
	YsVec3 crs;
	if(YSOK==pln.GetIntersection(crs,o,v))
	{
		if(YSTRUE==markUp_Snap->GetCheck())
		{
			MarkUp_SnapToHalfUnit(crs);
		}
		if(0==insertMarkUp_Point.size())
		{
			insertMarkUp_Point.push_back(crs);
		}
		insertMarkUp_Point.push_back(crs);
		insertMarkUp_Status=INSERTMARKUP_CLICKED;
	}
	return YSOK;
}
void FsGui3DMainCanvas::MarkUp_InsertLine_Finalize(void)
{
	if(2>insertMarkUp_Point.size())
	{
		return;
	}

	insertMarkUp_Status=INSERTMARKUP_STANDBY;

	YsArray <YsVec2i> plg;
	for(auto p : insertMarkUp_Point)
	{
		plg.Add(YsVec2i((int)p.x(),(int)p.y()));
	}
	insertMarkUp_Point.CleanUp();

	RetroMap_World::UndoGuard ug(world);

	auto fdHd=GetCurrentFieldCreateIfNotExist();

	auto fgCol=drawingDlg->lineColor->GetColor();
	auto bgCol=drawingDlg->fillColor->GetColor();
	auto muHd=world.CreateMarkUp(fdHd);
	world.MakePointArrayMarkUp(fdHd,muHd,plg);
	world.SetMarkUpForegroundColor(fdHd,muHd,fgCol);
	world.SetMarkUpBackgroundColor(fdHd,muHd,bgCol);

	if(YSTRUE==drawingDlg->frontArrowBtn->GetCheck())
	{
		RetroMap_World::MarkUp::ArrowHeadInfo ahInfo;
		ahInfo.arrowHeadType=RetroMap_World::MarkUp::ARROWHEAD_TRIANGLE;
		ahInfo.arrowHeadSize=drawingDlg->arrowSizeTxt->GetRealNumber();
		world.SetPointArrayMarkUpArrowHead(fdHd,muHd,0,ahInfo);			
	}
	if(YSTRUE==drawingDlg->backArrowBtn->GetCheck())
	{
		RetroMap_World::MarkUp::ArrowHeadInfo ahInfo;
		ahInfo.arrowHeadType=RetroMap_World::MarkUp::ARROWHEAD_TRIANGLE;
		ahInfo.arrowHeadSize=drawingDlg->arrowSizeTxt->GetRealNumber();
		world.SetPointArrayMarkUpArrowHead(fdHd,muHd,1,ahInfo);			
	}

	if(YSTRUE==drawingDlg->autoAnchorBtn->GetCheck())
	{
		for(auto plgIdx : plg.AllIndex())
		{
			SetAnchor(fdHd,muHd,plgIdx);
		}
	}

	world.ReadyVbo(fdHd,muHd);

	if(YSTRUE!=drawingDlg->multipleBtn->GetCheck())
	{
		drawingDlg->noneBtn->SetCheck(YSTRUE);
		drawingDlg->TurnOffDrawingMode();
	}
}
YSRESULT FsGui3DMainCanvas::MarkUp_InsertLine_OnMouseMove(FsGuiMouseButtonSet,YsVec2i pos,YsVec2i)
{
	if(INSERTMARKUP_CLICKED==insertMarkUp_Status || INSERTMARKUP_DRAGGING==insertMarkUp_Status)
	{
		YsVec3 o,v;
		drawEnv.TransformScreenCoordTo3DLine(o,v,pos.x(),pos.y());
		YsPlane pln(YsOrigin(),YsZVec());
		YsVec3 crs;
		if(YSOK==pln.GetIntersection(crs,o,v))
		{
			if(YSTRUE==markUp_Snap->GetCheck())
			{
				MarkUp_SnapToHalfUnit(crs);
			}

			if(YSTRUE==FsGetKeyState(FSKEY_ALT))
			{
				YsVec3 d=crs-insertMarkUp_Point[insertMarkUp_Point.size()-2];
				auto dx=fabs(d.x());
				auto dy=fabs(d.y());
				if(dy<dx/2.0)
				{
					d.SetY(0.0);
				}
				else if(dx<dy/2.0)
				{
					d.SetX(0.0);
				}
				else if(dx>dy)
				{
					d.SetY(dx*d.y()/dy);
				}
				else
				{
					d.SetX(dy*d.x()/dx);
				}
				crs=insertMarkUp_Point[insertMarkUp_Point.size()-2]+d;
			}
			insertMarkUp_Point.back()=crs;
			insertMarkUp_Status=INSERTMARKUP_DRAGGING;
			SetNeedRedraw(YSTRUE);
		}
	}
	return YSOK;
}
YSRESULT FsGui3DMainCanvas::MarkUp_InsertLine_OnLButtonUp(FsGuiMouseButtonSet,YsVec2i)
{
	if(INSERTMARKUP_DRAGGING==insertMarkUp_Status)
	{
		if(DRAWING_LINE==mode)
		{
			MarkUp_InsertLine_Finalize();
		}
		else if(DRAWING_LINE_STRIP==mode)
		{
			insertMarkUp_Status=INSERTMARKUP_STANDBY;
		}
	}
	else
	{
		insertMarkUp_Status=INSERTMARKUP_STANDBY;
	}
	SetNeedRedraw(YSTRUE);
	return YSOK;
}
YSRESULT FsGui3DMainCanvas::MarkUp_InsertLine_OnTouchStateChange(const FsGuiObject::TouchState &)
{
	return YSERR;
}


void FsGui3DMainCanvas::MarkUp_SetAnchorByPicking(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode(nullptr);
	defAction=DEFAULTACTION_MARKUPPOINTSELECTION;
	markUpPointSelectionCallback=std::bind(&THISCLASS::MarkUp_SetAnchorByPicking_PointSelected,this,std::placeholders::_1);
}
void FsGui3DMainCanvas::MarkUp_SetAnchorByPicking_PointSelected(const YsArray <RetroMap_World::MarkUpPointIndex> &pickedMuPi)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd && 0<pickedMuPi.size())
	{
		RetroMap_World::UndoGuard ug(world);

		auto fieldPtr=world.GetField(fdHd);
		for(auto mupi : pickedMuPi)
		{
			SetAnchor(fdHd,mupi.muHd,mupi.idx);
		}
	}
}

void FsGui3DMainCanvas::MarkUp_SetAnchorOnSelectedMarkUp(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		RetroMap_World::UndoGuard ug(world);

		auto fieldPtr=world.GetField(fdHd);
		for(auto muHd : world.SelectedMarkUp(fdHd))
		{
			auto markUpPtr=world.GetMarkUp(fdHd,muHd);
			if(RetroMap_World::MarkUp::MARKUP_POINT_ARRAY!=markUpPtr->GetMarkUpType())
			{
				continue;
			}

			auto plg=markUpPtr->GetPointArray();
			for(auto plgIdx : plg.AllIndex())
			{
				SetAnchor(fdHd,muHd,plgIdx);
			}
		}
	}
}
void FsGui3DMainCanvas::SetAnchor(RetroMap_World::FieldHandle fdHd,RetroMap_World::MarkUpHandle muHd,YSSIZE_T idx)
{
	auto fieldPtr=world.GetField(fdHd);
	auto markUpPtr=world.GetMarkUp(fdHd,muHd);
	if(nullptr==fieldPtr ||
	   nullptr==markUpPtr ||
	   RetroMap_World::MarkUp::MARKUP_POINT_ARRAY!=markUpPtr->GetMarkUpType())
	{
		return;
	}

	auto plg=markUpPtr->GetPointArray();
	if(YSTRUE==plg.IsInRange(idx))
	{
		auto pos=plg[idx];
		YsVec2 tst(pos.x(),pos.y());
		auto baseMuHd=fieldPtr->FindStringMarkUpFromCoordinate(tst);
		auto baseMpHd=fieldPtr->FindMapPieceFromCoordinate(tst);
		if(nullptr!=baseMuHd) // Priority given to string mark up.
		{
			auto baseMarkUpPtr=fieldPtr->GetMarkUp(baseMuHd);
			auto baseMarkUpPos=baseMarkUpPtr->GetPosition();
			auto diff=pos-baseMarkUpPos;
printf("%s %d\n",__FUNCTION__,__LINE__);
			world.CreateAnchor(fdHd,muHd,idx,baseMuHd,diff);
		}
		else if(nullptr!=baseMpHd)
		{
			auto baseMapPiecePtr=fieldPtr->GetMapPiece(baseMpHd);
			auto baseMapPiecePos=baseMapPiecePtr->GetPosition();
			auto diff=pos-baseMapPiecePos;
printf("%s %d\n",__FUNCTION__,__LINE__);
			world.CreateAnchor(fdHd,muHd,idx,baseMpHd,diff);
		}
		else
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
		}
	}
}

void FsGui3DMainCanvas::Reanchor(RetroMap_World::FieldHandle fdHd,RetroMap_World::MarkUpHandle muHd)
{
	RetroMap_World::UndoGuard undoGuard(world);
	auto markUpPtr=world.GetMarkUp(fdHd,muHd);
	if(nullptr!=markUpPtr)
	{
		auto pointCoord=markUpPtr->GetPointArray();
		for(auto idx : pointCoord.AllIndex())
		{
			Reanchor(fdHd,muHd,idx);
		}
	}
}
void FsGui3DMainCanvas::Reanchor(RetroMap_World::FieldHandle fdHd,RetroMap_World::MarkUpHandle muHd,YSSIZE_T idx)
{
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		auto anHd=fieldPtr->FindAnchorFromMarkUp(muHd,idx);
		if(nullptr!=anHd)
		{
			world.DeleteAnchor(fdHd,anHd);
			SetAnchor(fdHd,muHd,idx);
		}
	}
}

void FsGui3DMainCanvas::MarkUp_UnsetAnchorByPicking(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode(nullptr);
	defAction=DEFAULTACTION_MARKUPPOINTSELECTION;
	markUpPointSelectionCallback=std::bind(&THISCLASS::MarkUp_UnsetAnchorByPicking_PointSelected,this,std::placeholders::_1);
}
void FsGui3DMainCanvas::MarkUp_UnsetAnchorByPicking_PointSelected(const YsArray <RetroMap_World::MarkUpPointIndex> &pickedMuPi)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd && 0<pickedMuPi.size())
	{
		RetroMap_World::UndoGuard ug(world);

		auto fieldPtr=world.GetField(fdHd);
		for(auto mupi : pickedMuPi)
		{
			auto anHd=fieldPtr->FindAnchorFromMarkUp(mupi.muHd,mupi.idx);
			if(nullptr!=anHd)
			{
				world.DeleteAnchor(fdHd,anHd);
			}
		}
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::MarkUp_UnsetAnchorOfSelectedMarkUp(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		RetroMap_World::UndoGuard ug(world);

		auto fieldPtr=world.GetField(fdHd);
		for(auto muHd : world.SelectedMarkUp(fdHd))
		{
			for(auto anHd : fieldPtr->FindAnchorFromMarkUp(muHd))
			{
				world.DeleteAnchor(fdHd,anHd);
			}
		}
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::MarkUp_ReapplyAnchor(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		RetroMap_World::UndoGuard ug(world);

		auto fieldPtr=world.GetField(fdHd);
		YsArray <RetroMap_World::AnchorHandle> allAnHd;
		for(auto anHd : fieldPtr->AllAnchor())
		{
			allAnHd.push_back(anHd);
		}
		world.ReapplyAnchor(fdHd,allAnHd);
		for(auto anHd : allAnHd)
		{
			auto anchorPtr=fieldPtr->GetAnchor(anHd);
			auto muHd=anchorPtr->GetMarkUp(*fieldPtr);
			world.ReadyVbo(fdHd,muHd);
		}
		SetNeedRedraw(YSTRUE);
	}
}
