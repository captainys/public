/* ////////////////////////////////////////////////////////////

File Name: dnmtreedialog.cpp
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
#include "../textresource.h"
#include "dnmtreedialog.h"



PolyCreDnmTreeDialog::PolyCreDnmTreeDialog()
{
	dnmTree=NULL;
	canvas=NULL;
	okBtn=NULL;
	cancelBtn=NULL;
	treeEdited=YSFALSE;
	runningModal=YSFALSE;
}

PolyCreDnmTreeDialog::~PolyCreDnmTreeDialog()
{
}

/*static*/ PolyCreDnmTreeDialog *PolyCreDnmTreeDialog::Create(void)
{
	PolyCreDnmTreeDialog *dlg=new PolyCreDnmTreeDialog;
	return dlg;
}

/*static*/ void PolyCreDnmTreeDialog::Delete(PolyCreDnmTreeDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreDnmTreeDialog::Make(class GeblGuiEditorBase *canvas)
{
	this->canvas=canvas;

	if(NULL==okBtn)
	{
		FsGuiDialog::Initialize();

		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		dnmTree=AddTreeControl(0,FSKEY_NULL,32,24,YSTRUE);
		treeEdited=YSFALSE;

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}

	dnmTree->CleanUp();
	auto rootArray=canvas->shlGrp.GetRootNodeArray();
	for(auto rootNode : rootArray)
	{
		PopulateTreeControl(dnmTree->GetRoot(),rootNode);
	}

	treeEdited=YSFALSE;

	return YSOK;
}

void PolyCreDnmTreeDialog::EnableTreeEdit(void)
{
	if(NULL==dnmTree)
	{
		printf("Error!\n");
		printf("Call this function after making the dialog.\n");
		exit(1);
	}

	dnmTree->SetCanDragNode(YSTRUE);
}

void PolyCreDnmTreeDialog::PopulateTreeControl(const FsGuiTreeControlNode *parent,YsShellDnmContainer <YsShellExtEditGui>::Node *dnmNode)
{
	auto newNode=dnmTree->AddTextChild(parent,dnmNode->nodeName);
	newNode->SetIntAttrib((int)dnmNode->GetSearchKey());
	for(auto childNode : dnmNode->children)
	{
		PopulateTreeControl(newNode,childNode);
	}
}

void PolyCreDnmTreeDialog::RemakeDrawingBuffer(void)
{
	// As required
}

void PolyCreDnmTreeDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvas->Edit_ClearUIMode();
		if(YSTRUE==treeEdited)
		{
			auto &shlGrp=canvas->shlGrp;
			shlGrp.MakeFlatTreeStructure();

			auto rootNode=dnmTree->GetRoot();
			YsArray <const FsGuiTreeControlNode *> todo;
			todo.Add(rootNode);
			for(YSSIZE_T idx=0; idx<todo.GetN(); ++idx)
			{
				auto currentDnmNode=shlGrp.FindNode((YSHASHKEY)todo[idx]->GetIntAttrib());
printf("From:");
printf("%s\n",(nullptr!=currentDnmNode ? currentDnmNode->nodeName.Txt() : "null"));
				for(YSSIZE_T childIdx=0; childIdx<todo[idx]->GetNChild(); ++childIdx)
				{
					auto childNode=todo[idx]->GetChild(childIdx);
					todo.Add(childNode);

					auto childDnmNode=shlGrp.FindNode((YSHASHKEY)childNode->GetIntAttrib());
printf("  To:");
printf("%s\n",(nullptr!=childDnmNode ? childDnmNode->nodeName.Txt() : "null"));
					if(nullptr!=currentDnmNode && nullptr!=childDnmNode)
					{
						shlGrp.Reconnect(childDnmNode,currentDnmNode);
					}
				}
			}
		}
		if(YSTRUE==runningModal)
		{
			CloseModalDialog(YSOK);
		}
	}
	else if(btn==cancelBtn)
	{
		canvas->Edit_ClearUIMode();
		if(YSTRUE==runningModal)
		{
			CloseModalDialog(YSERR);
		}
	}
}

void PolyCreDnmTreeDialog::OnDropListSelChange(FsGuiDropList *,int)
{
}

void PolyCreDnmTreeDialog::OnTextBoxChange(FsGuiTextBox *)
{
}

void PolyCreDnmTreeDialog::OnSliderPositionChange(FsGuiSlider *,const double &/*prevPos*/,const double &/*prevValue*/)
{
}

/* virtual */ void PolyCreDnmTreeDialog::OnTreeControlSelChange(FsGuiTreeControl *tree)
{
	if(tree==dnmTree)
	{
		auto clicked=tree->GetSelection();
		if(NULL!=clicked)
		{
			YSHASHKEY slKey=(YSHASHKEY)clicked->GetIntAttrib();

			canvas->slHd=canvas->shlGrp.FindNode(slKey);
			canvas->needRemakeDrawingBuffer|=GeblGuiEditorBase::NEED_REMAKE_DRAWING_ALL;
			canvas->needRemakeDrawingBuffer_for_OtherShell|=GeblGuiEditorBase::NEED_REMAKE_DRAWING_ALL;
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
}

/* virtual */ void PolyCreDnmTreeDialog::OnTreeControlNodeMoved(FsGuiTreeControl *tree,int nNode,const FsGuiTreeControlNode * const node[])
{
	treeEdited=YSTRUE;
}
