#include <math.h>

#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysglslcpp.h>
#include <ysport.h>

#include "../fsgui3dapp.h"
#include "../textresource.h"



class FsGui3DMainCanvas::WorldPropertyDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiTextBox *labelTxt;

	FsGuiTextBox *defFontSizeTxt;
	FsGuiColorPalette *defForegroundPlt,*defBackgroundPlt;
	FsGuiButton *defTransparentBackGroundBtn;

	FsGuiButton *okBtn,*cancelBtn;

	void Make(FsGui3DMainCanvas *canvasPtr);
	void SetFromWorld(const RetroMap_World &world);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGui3DMainCanvas::WorldPropertyDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();
	labelTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_WORLDPROP_NAME,64,YSTRUE);

	defFontSizeTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_COMMENT_FONTSIZE,4,YSTRUE);
	defFontSizeTxt->SetTextType(FSGUI_INTEGER);
	defForegroundPlt=AddCompactColorPalette(0,FSKEY_NULL,"",255,255,255,YSFALSE);
	defBackgroundPlt=AddCompactColorPalette(0,FSKEY_NULL,"",0,0,0,YSFALSE);
	defTransparentBackGroundBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_COMMENT_TRANSPARENT_BACKGROUND,YSFALSE);

	defFontSizeTxt->SetInteger(canvasPtr->world.GetDefaultFontSize());
	defForegroundPlt->SetColor(canvasPtr->world.GetDefaultMarkUpForegroundColor());
	defBackgroundPlt->SetColor(canvasPtr->world.GetDefaultMarkUpBackgroundColor());
	if(canvasPtr->world.GetDefaultMarkUpBackgroundColor().Af()<0.5f)
	{
		defTransparentBackGroundBtn->SetCheck(YSTRUE);
	}
	else
	{
		defTransparentBackGroundBtn->SetCheck(YSFALSE);
	}


	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	Fit();
}
void FsGui3DMainCanvas::WorldPropertyDialog::SetFromWorld(const RetroMap_World &world)
{
	labelTxt->SetText(world.GetGameTitle());
}
/* virtual */ void FsGui3DMainCanvas::WorldPropertyDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		canvasPtr->world.SetGameTitle(labelTxt->GetWString());

		auto defFontSize=YsGreater(defFontSizeTxt->GetInteger(),8);
		canvasPtr->world.SetDefaultFontSize(defFontSize);

		auto defFgCol=defForegroundPlt->GetColor();
		auto defBgCol=defBackgroundPlt->GetColor();
		if(YSTRUE==defTransparentBackGroundBtn->GetCheck())
		{
			defBgCol.SetAi(0);
		}
		canvasPtr->world.SetDefaultMarkUpForegroundColor(defFgCol);
		canvasPtr->world.SetDefaultMarkUpBackgroundColor(defBgCol);

		CloseModalDialog(YSOK);
	}
	else if(cancelBtn==btn)
	{
		CloseModalDialog(YSERR);
	}
}

void FsGui3DMainCanvas::World_Property(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<WorldPropertyDialog>();
	dlg->Make(this);
	dlg->SetFromWorld(world);
	AttachModalDialog(dlg);
	ArrangeDialog();
}



////////////////////////////////////////////////////////////



class FsGui3DMainCanvas::SetUnitDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiTextBox *xTxt,*yTxt;
	FsGuiButton *setBtn,*setAndApplyToAllBtn,*cancelBtn;
	void Make(FsGui3DMainCanvas *canvasPtr);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
};

void FsGui3DMainCanvas::SetUnitDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;
	FsGuiDialog::Initialize();
	xTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_WORLDPROP_NAME,4,YSTRUE);
	xTxt->SetTextType(FSGUI_INTEGER);
	yTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_WORLDPROP_NAME,4,YSFALSE);
	yTxt->SetTextType(FSGUI_INTEGER);

	setBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SETUNIT_SET,YSFALSE);
	setAndApplyToAllBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_SETUNIT_SET_AND_APPLY_ALL,YSFALSE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	Fit();
}

/* virtual */ void FsGui3DMainCanvas::SetUnitDialog::OnButtonClick(FsGuiButton *btn)
{
	auto min=canvasPtr->threeDInterface.box_pos[0];
	auto max=canvasPtr->threeDInterface.box_pos[1];
	auto d=max-min;

	YsVec2i unit;
	unit.Set((int)d.x(),(int)d.y());
	if(unit.x()<1)
	{
		unit.SetX(1);
	}
	if(unit.y()<1)
	{
		unit.SetY(1);
	}

	if(btn==setBtn)
	{
		canvasPtr->world.SetUnit(unit);
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
	if(btn==setAndApplyToAllBtn)
	{
		canvasPtr->world.SetUnit(unit);
		auto fdHd=canvasPtr->GetCurrentField();
		if(nullptr!=fdHd)
		{
			for(auto mpHd : canvasPtr->world.AllMapPiece(fdHd))
			{
				auto mapPiecePtr=canvasPtr->world.GetMapPiece(fdHd,mpHd);
				auto shape=mapPiecePtr->GetShape();
				shape.ApplyUnit(unit);
				canvasPtr->world.SetPosition(fdHd,mpHd,shape.pos);
			}
		}
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
	if(btn==cancelBtn)
	{
		canvasPtr->Edit_ClearUIMode(nullptr);
	}
}

/* virtual */ void FsGui3DMainCanvas::SetUnitDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	YsVec3 box[2];
	canvasPtr->threeDInterface.GetBox(box);
	auto u=txt->GetInteger();
	if(txt==xTxt)
	{
		box[1].SetX(box[0].x()+(double)u);
		canvasPtr->threeDInterface.SetBox(box);
	}
	else if(txt==yTxt)
	{
		box[1].SetY(box[0].y()+(double)u);
		canvasPtr->threeDInterface.SetBox(box);
	}
}

void FsGui3DMainCanvas::World_SetUnit(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode(nullptr);

	auto unit=world.GetUnit();
	auto fdHd=GetCurrentField();

	YsVec3 rect[2]={drawEnv.GetViewTarget(),drawEnv.GetViewTarget()};
	if(nullptr!=fdHd)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		if(0<selMpHd.GetN())
		{
			auto mapPiecePtr=world.GetMapPiece(fdHd,selMpHd[0]);
			auto bbx=mapPiecePtr->GetBoundingBox();
			rect[0]=bbx[0];
			rect[1]=bbx[0];
		}
	}
	rect[1].AddX(unit.x());
	rect[1].AddY(unit.y());

	rect[0].SetZ(0);
	rect[1].SetZ(0);

	DisableDefaultAction();
	threeDInterface.BeginInputBox(rect[0],rect[1],YSTRUE,YSFALSE);
	threeDInterface.EnableMaintainAspectRatioButton();
	threeDInterface.BindCallBack(&THISCLASS::World_SetUnit_BoxSizeChange,this);

	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<SetUnitDialog>();
	dlg->Make(this);
	AddDialog(dlg);
	ArrangeDialog();

	draw3dCallBack=std::bind(&THISCLASS::World_SetUnit_Draw3DCallBack,this);
}

void FsGui3DMainCanvas::World_SetUnit_Draw3DCallBack(void)
{
	auto min=threeDInterface.box_pos[0];
	auto max=threeDInterface.box_pos[1];
	auto d=max-min;

	YsVec2i unit;
	unit.Set((int)d.x(),(int)d.y());

	YsGLSLPlain3DRenderer renderer;
	float col[4]={0,0,255,255};
	renderer.SetUniformColor(col);

	YsGLVertexBuffer vtxBuf;
	for(int y=-32; y<=32; y++)
	{
		double x1=min.x()-(double)(32*unit.x());
		double x2=min.x()+(double)(32*unit.x());
		double yy=min.y()+(double)(y*unit.y());
		vtxBuf.Add<double>(x1,yy,0);
		vtxBuf.Add<double>(x2,yy,0);
	}
	for(int x=-32; x<=32; x++)
	{
		double xx=min.x()+(double)(x*unit.x());
		double y1=min.y()-(double)(32*unit.y());
		double y2=min.y()+(double)(32*unit.y());
		vtxBuf.Add<double>(xx,y1,0);
		vtxBuf.Add<double>(xx,y2,0);
	}
	renderer.DrawVtx(GL_LINES,vtxBuf.GetN(),vtxBuf);
}

void FsGui3DMainCanvas::World_SetUnit_BoxSizeChange(FsGui3DInterface *itfc)
{
	auto min=itfc->box_pos[0];
	auto max=itfc->box_pos[1];
	auto d=max-min;

	YsVec2i unit;
	unit.Set((int)d.x(),(int)d.y());

	for(YSSIZE_T idx=0; idx<GetNumDialog(); ++idx)
	{
		auto dlg=dynamic_cast<SetUnitDialog *>(GetDialog(idx));
		if(nullptr!=dlg)
		{
			dlg->xTxt->SetInteger(unit.x());
			dlg->yTxt->SetInteger(unit.y());
			break;
		}
	}
}
