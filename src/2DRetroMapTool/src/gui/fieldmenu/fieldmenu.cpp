#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysport.h>

#include "../fsgui3dapp.h"
#include "../textresource.h"



class FsGui3DMainCanvas::FieldPropertyDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiTextBox *labelTxt;
	FsGuiButton *okBtn,*cancelBtn;

	void Make(FsGui3DMainCanvas *canvasPtr);
	void SetFromField(const RetroMap_World &world,RetroMap_World::FieldHandle fdHd);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGui3DMainCanvas::FieldPropertyDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();
	labelTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_FIELDPROP_FIELDNAME,64,YSTRUE);

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	Fit();
}
void FsGui3DMainCanvas::FieldPropertyDialog::SetFromField(const RetroMap_World &world,RetroMap_World::FieldHandle fdHd)
{
	labelTxt->SetText(world.GetFieldName(fdHd));
}
/* virtual */ void FsGui3DMainCanvas::FieldPropertyDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		RetroMap_World::UndoGuard ug(canvasPtr->world);

		auto fdHd=canvasPtr->GetCurrentField();
		if(nullptr!=fdHd)
		{
			canvasPtr->world.SetFieldName(fdHd,labelTxt->GetWString());
			canvasPtr->UpdateFieldSelector();
		}
		CloseModalDialog(YSOK);
	}
	else if(cancelBtn==btn)
	{
		CloseModalDialog(YSERR);
	}
}

void FsGui3DMainCanvas::Field_Property(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentFieldCreateIfNotExist();
	if(nullptr!=fdHd)
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FieldPropertyDialog>();
		dlg->Make(this);

		dlg->SetFromField(world,fdHd);

		AttachModalDialog(dlg);
		ArrangeDialog();
		SetNeedRedraw(YSTRUE);
	}
}



void FsGui3DMainCanvas::Field_NewField(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode(nullptr);

	auto fdHd=world.CreateField();
	SetCurrentField(fdHd);
	SetNeedRedraw(YSTRUE);
}


void FsGui3DMainCanvas::Field_DeleteField(FsGuiPopUpMenuItem *)
{
	if(nullptr!=GetCurrentField())
	{
		auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		msgDlg->Make(L"",FSGUI_DLG_FIELD_CONFIRM_DELETE,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
		msgDlg->BindCloseModalCallBack(&THISCLASS::Field_DeleteField_Confirm,this);
		AttachModalDialog(msgDlg);
	}
}
void FsGui3DMainCanvas::Field_DeleteField_Confirm(FsGuiDialog *dlg,int returnCode)
{
	if((int)YSOK==returnCode)
	{
		auto fdHdToDel=GetCurrentField();
		if(nullptr!=fdHdToDel)
		{
			auto newCurrentFdHd=world.FindNextField(fdHdToDel);
			if(nullptr==newCurrentFdHd)
			{
				newCurrentFdHd=world.FindPrevField(fdHdToDel);
			}
			world.DeleteField(fdHdToDel);
			Edit_ClearUIMode(nullptr);
			SetCurrentField(newCurrentFdHd);
			SetNeedRedraw(YSTRUE);
		}
	}
}


class FsGui3DMainCanvas::FieldOrderDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiListBox *lbx;
	FsGuiButton *moveUpBtn,*moveDownBtn;
	FsGuiButton *closeBtn;

	void Make(FsGui3DMainCanvas *canvasPtr);
	void Update(void);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnListBoxSelChange(FsGuiListBox *lbx,int prevSel);
};

void FsGui3DMainCanvas::FieldOrderDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;
	lbx=AddEmptyListBox(0,FSKEY_NULL,L"Fields",24,32,YSTRUE);
	moveUpBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_FIELDORDER_UP,YSTRUE);
	moveDownBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_FIELDORDER_DOWN,YSFALSE);
	closeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSTRUE);
	Update();
	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
}
void FsGui3DMainCanvas::FieldOrderDialog::Update(void)
{
	

	lbx->ClearChoice();
	for(auto fdHd : canvasPtr->world.AllField())
	{
		auto label=canvasPtr->world.GetFieldName(fdHd);
		auto sel=(fdHd==canvasPtr->GetCurrentField() ? YSTRUE : YSFALSE);
		auto idx=lbx->AddString(label,sel);
		lbx->SetIntAttrib(idx,canvasPtr->world.GetSearchKey(fdHd));
	}
}
void FsGui3DMainCanvas::FieldOrderDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==moveUpBtn)
	{
		auto sel=lbx->GetSelection();
		auto key=lbx->GetIntAttrib(sel);
		auto fdHd=canvasPtr->world.FindField(key);
		auto prevFdHd=canvasPtr->world.FindPrevField(fdHd);
		if(nullptr!=fdHd && nullptr!=prevFdHd)
		{
			canvasPtr->world.Swap(fdHd,prevFdHd);
			canvasPtr->SetCurrentField(prevFdHd);
			auto showTop=YsGreater(0,lbx->GetShowTop()-1);
			Update();
			lbx->SetShowTop(showTop);
			canvasPtr->UpdateFieldSelector();
		}
	}
	else if(btn==moveDownBtn)
	{
		auto sel=lbx->GetSelection();
		auto key=lbx->GetIntAttrib(sel);
		auto fdHd=canvasPtr->world.FindField(key);
		auto nextFdHd=canvasPtr->world.FindNextField(fdHd);
		if(nullptr!=fdHd && nullptr!=nextFdHd)
		{
			auto showTop=lbx->GetShowTop()+1;
			canvasPtr->world.Swap(fdHd,nextFdHd);
			canvasPtr->SetCurrentField(nextFdHd);
			Update();
			lbx->SetShowTop(showTop);
			canvasPtr->UpdateFieldSelector();
		}
	}
	else if(btn==closeBtn)
	{
		CloseModalDialog(YSOK);
	}
}
/* virtual */ void FsGui3DMainCanvas::FieldOrderDialog::OnListBoxSelChange(FsGuiListBox *lbx,int prevSel)
{
	if(lbx==this->lbx)
	{
		auto sel=lbx->GetSelection();
		auto key=lbx->GetIntAttrib(sel);
		auto fdHd=canvasPtr->world.FindField(key);
		if(nullptr!=fdHd)
		{
			canvasPtr->SetCurrentField(fdHd);
		}
	}
}

void FsGui3DMainCanvas::Field_ChangeOrder(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FieldOrderDialog>();
	dlg->Make(this);
	AttachModalDialog(dlg);
	ArrangeDialog();
}

void FsGui3DMainCanvas::Field_ChangeOrder_DialogClosed(FsGuiDialog *dlg,int returnCode)
{
}



void FsGui3DMainCanvas::Field_NextField(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		Edit_ClearUIMode(nullptr);

		fdHd=world.FindNextField(fdHd);
		if(nullptr==fdHd)
		{
			fdHd=world.FindNextField(fdHd);
		}
		SetCurrentField(fdHd);
		SetNeedRedraw(YSTRUE);
	}
}



void FsGui3DMainCanvas::Field_PrevField(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		Edit_ClearUIMode(nullptr);

		fdHd=world.FindPrevField(fdHd);
		if(nullptr==fdHd)
		{
			fdHd=world.FindPrevField(fdHd);
		}
		SetCurrentField(fdHd);
		SetNeedRedraw(YSTRUE);
	}
}
