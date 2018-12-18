/* ////////////////////////////////////////////////////////////

File Name: filemenu.cpp
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

#include <ysshellext_geomcalc.h>

#include "../ysgebl_gui_editor_base.h"
#include "file_slideshowdialog.h"

#include <fsguifiledialog.h>
#include <ysgl.h>
#include <ysport.h>
#include <ysshellkernel.h>
#include "../config/polycreconfig.h"

#include "../textresource.h"


void GeblGuiEditorBase::File_New(FsGuiPopUpMenuItem *)
{
	YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd=shlGrp.CreateShell(NULL);
	if(NULL!=newSlHd)
	{
		slHd=newSlHd;

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}

void GeblGuiEditorBase::File_Open(FsGuiPopUpMenuItem *)
{
	YsString dataDir;
	dataDir.EncodeUTF8 <wchar_t> (GetDataDir());

	YsString def;
	def.MakeFullPathName(dataDir,"*");
	// YsString last(scn.GetLastOpenedFilename()),def;
	// if(last.Strlen()>0)
	// {
	// 	YsString pth,fil;
	// 	last.SeparatePathFile(pth,fil);
	// 	def.MakeFullPathName(pth,"*.fld");
	// }
	// else
	// {
	// 	def.MakeFullPathName(scnDir,"*.fld");
	// }

	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Open");
	fdlg->fileExtensionArray.Append(L".srf");
	fdlg->fileExtensionArray.Append(L".stl");
	fdlg->fileExtensionArray.Append(L".obj");
	fdlg->fileExtensionArray.Append(L".off");
	fdlg->fileExtensionArray.Append(L".dxf");
	fdlg->fileExtensionArray.Append(L".dnm");
	fdlg->defaultFileName.SetUTF8String(def);
	fdlg->BindCloseModalCallBack(&THISCLASS::File_Open_FileSelected,this);
	AttachModalDialog(fdlg);
}

void GeblGuiEditorBase::File_Open_FileSelected(FsGuiDialog *,int returnValue)
{
	fdlg->UnbindCloseModalCallBack();
	if(YSOK==(YSRESULT)returnValue)
	{
		YsWString ful(fdlg->selectedFileArray[0]);

		YsWString ext;
		ful.GetExtension(ext);
		ext.Capitalize();
		if(0==ext.Strcmp(ext,L".DNM"))
		{
			YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> allNode;
			shlGrp.GetNodePointerAll(allNode);

			for(YSSIZE_T idx=0; idx<allNode.GetN(); ++idx)
			{
				if(YSTRUE==allNode[idx]->IsModified())
				{
					YesNoDialog(
					    FSGUI_DLG_OPENDNM_CONFIRMCLOSE_TITLE,
					    FSGUI_DLG_OPENDNM_CONFIRMCLOSE_MESSAGE,
					    FSGUI_COMMON_OK,
					    FSGUI_COMMON_CANCEL,
					    &THISCLASS::File_OpenDnm_ConfirmCloseCallBack,this);
					return;
				}
			}
		}

		LoadGeneral(fdlg->selectedFileArray[0],NULL,YSTRUE);
	}
}

void GeblGuiEditorBase::File_OpenDnm_ConfirmCloseCallBack(FsGuiDialog *,int returnCode)
{
	if((int)YSOK==returnCode)
	{
		LoadGeneral(fdlg->selectedFileArray[0],NULL,YSTRUE);
	}
}


void GeblGuiEditorBase::File_Open_AfterOpenFile(YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd,const wchar_t fn[],YSBOOL recordRecentFiles)
{
	if(NULL!=slHd &&
	   YSTRUE==slHd->IsEmptyAndHasNeverBeenEdited() &&
	   0==shlGrp.GetNumChildNode(slHd))
	{
		shlGrp.DeleteShell(slHd);
	}

	slHd=newSlHd;

	{
		YsVec3 allBbx[2];
		shlGrp.GetBoundingBox(allBbx);
		drawEnv.SetTargetBoundingBox(allBbx[0],allBbx[1]);
		drawEnv.SetViewDistance((allBbx[1]-allBbx[0]).GetLength()*1.5);
	}

	viewControlDlg->SaveLastView();
	YsVec3 bbx[2];
	slHd->GetBoundingBox(bbx[0],bbx[1]);
	drawEnv.SetViewTarget((bbx[0]+bbx[1])/2.0);
	drawEnv.SetZoom(1.0);

	shlGrp.GetBoundingBox(slHd->GetDrawingBuffer().viewPort);

	// Is this a relative-path or full path file name?
	{
		YsWString ful,cwd;
		YsFileIO::Getcwd(cwd);
		ful.MakeFullPathName(cwd,fn);

		FILE *fp=YsFileIO::Fopen(ful,"rb");
		if(NULL!=fp)
		{
			fclose(fp); // No, fn was a relative path.
		}
		else
		{
			ful.Set(fn); // Yes, fn was a full-path.
		}

		lastAccessedFileName.Set(ful);
		if(YSTRUE==recordRecentFiles)
		{
			AddRecentlyUsedFile(ful);
		}
	}


	needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
	SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::File_Save(FsGuiPopUpMenuItem *menuItem)
{
	if(NULL!=slHd)
	{
		YsWString def;
		if(nullptr!=slHd && 0<YsWString::Strlen(slHd->GetFileName()))
		{
			def=slHd->GetFileName();
		}
		else if(0==YsWString::STRCMP(lastAccessedFileName.GetExtension(),L".DNM"))
		{
			def=lastAccessedFileName;
		}

		if(0==def.Strlen())
		{
			File_SaveAs(menuItem);
		}
		else
		{
			SaveGeneral(*slHd,def,YSTRUE);
		}
	}
}

void GeblGuiEditorBase::File_SaveAs(FsGuiPopUpMenuItem *)
{
	YsWString def;
	def.MakeFullPathName(GetDataDir(),L"*.srf");

	if(nullptr!=slHd && 0<YsWString::Strlen(slHd->GetFileName()))
	{
		def=slHd->GetFileName();
	}
	else if(0==YsWString::STRCMP(lastAccessedFileName.GetExtension(),L".DNM"))
	{
		def=lastAccessedFileName;
	}

	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Save As");
	fdlg->fileExtensionArray.Append(L".srf");
	fdlg->fileExtensionArray.Append(L".dnm");
	fdlg->fileExtensionArray.Append(L".stl");
	fdlg->fileExtensionArray.Append(L".obj");
	fdlg->fileExtensionArray.Append(L".off");
	fdlg->fileExtensionArray.Append(L".t3d");
	fdlg->defaultFileName=def;
	fdlg->BindCloseModalCallBack(&THISCLASS::File_SaveAs_FileSelected,this);
	AttachModalDialog(fdlg);
}

void GeblGuiEditorBase::File_SaveAs_FileSelected(FsGuiDialog *,int returnValue)
{
	fdlg->UnbindCloseModalCallBack();

	if(YSOK==(YSRESULT)returnValue)
	{
		YsWString ful=this->fdlg->selectedFileArray[0];
		YsWString ext;
		ful.GetExtension(ext);

		if(0==ext.STRCMP(ext,L".STL") && YSTRUE==this->config->GetWarnSaveAsSTL())
		{
			this->YesNoDialog(FSGUI_WARNING_STLWARNINGTITLE,FSGUI_WARNING_STLWARNING,FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL,&THISCLASS::File_SaveAs_STLChecked,this);
		}
		else
		{
			this->File_SaveAs_CheckOverwrite();
		}
	}
}

void GeblGuiEditorBase::File_SaveAs_STLChecked(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		File_SaveAs_CheckOverwrite();
	}
}

void GeblGuiEditorBase::File_SaveAs_CheckOverwrite(void)
{
	FILE *fp=YsFileIO::Fopen(fdlg->selectedFileArray[0],"rb");
	if(NULL!=fp)
	{
		fclose(fp);
		YesNoDialog(FSGUI_DLG_CONFIRMOVERWRITE_TITLE,FSGUI_DLG_CONFIRMOVERWRITE_MESSAGE,FSGUI_COMMON_OK,FSGUI_COMMON_CANCEL,&THISCLASS::File_SaveAs_ConfirmOverwriteCallBack,this);
		SetNeedRedraw(YSTRUE);
	}
	else
	{
		File_SaveAs_PassedOverWriteCheck(fdlg->selectedFileArray[0]);
	}
}

void GeblGuiEditorBase::File_SaveAs_ConfirmOverwriteCallBack(FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		printf("Passed overwrite check.\n");
		File_SaveAs_PassedOverWriteCheck(fdlg->selectedFileArray[0]);
	}
}

void GeblGuiEditorBase::File_SaveAs_PassedOverWriteCheck(const wchar_t fn[])
{
	if(NULL!=slHd)
	{
		SaveGeneral(*slHd,fn,YSTRUE);

		// Is this a relative-path or full path file name?
		{
			YsWString ful,cwd;
			YsFileIO::Getcwd(cwd);
			ful.MakeFullPathName(cwd,fn);

			FILE *fp=YsFileIO::Fopen(ful,"rb");
			if(NULL!=fp)
			{
				fclose(fp); // No, fn was a relative path.
			}
			else
			{
				ful.Set(fn); // Yes, fn was a full-path.
			}

			lastAccessedFileName.Set(ful);
			AddRecentlyUsedFile(ful);
		}
	}
}



void GeblGuiEditorBase::File_Exit(FsGuiPopUpMenuItem *)
{
	YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> allNode;
	shlGrp.GetNodePointerAll(allNode);

	for(YSSIZE_T idx=0; idx<allNode.GetN(); ++idx)
	{
		if(YSTRUE==allNode[idx]->IsModified())
		{
			YesNoDialog(
			    FSGUI_DLG_CONFIRMEXIT_TITLE,
			    FSGUI_DLG_CONFIRMEXIT_MESSAGE,
			    FSGUI_COMMON_OK,
			    FSGUI_COMMON_CANCEL,
			    &THISCLASS::File_Exit_ConfirmExitCallBack,this);
			return;
		}
	}

	// if(YSTRUE==shlGrp->DnmTreeIsModified())
	// {
	//		canvas->YesNoDialog(L"Confirm Exit?",
	//		    L"Dynamic Model structure/motion has been modified. Exit anyway?",
	//		    L"Yes",L"No",
	//		    File_Exit_ConfirmExitCallBack);
	// 		return;
	// }

	mustTerminate=YSTRUE;
}

void GeblGuiEditorBase::File_Exit_ConfirmExitCallBack(FsGuiDialog *,int returnValue)
{
	if(YSTRUE==(YSBOOL)returnValue)
	{
		mustTerminate=YSTRUE;
	}
}



void GeblGuiEditorBase::File_Close(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd() && YSTRUE==Slhd()->IsModified())
	{
		YesNoDialog(
		    FSGUI_DLG_CONFIRMCLOSE_TITLE,
		    FSGUI_DLG_CONFIRMCLOSE_MESSAGE,
		    FSGUI_COMMON_OK,
		    FSGUI_COMMON_CANCEL,
		    &THISCLASS::File_Close_ConfirmCloseCallBack,this);
		return;
	}
	File_Close_Confirmed();
}

void GeblGuiEditorBase::File_Close_ConfirmCloseCallBack(FsGuiDialog *,int returnValue)
{
	if(YSTRUE==(YSBOOL)returnValue)
	{
		File_Close_Confirmed();
	}
}

void GeblGuiEditorBase::File_Close_Confirmed(void)
{
	if(NULL!=slHd)
	{
		auto nextSlHd=slHd;

		YsArray <YsShellDnmContainer <YsShellExtEditGui>::Node *> allNode;
		shlGrp.GetNodePointerAll(allNode);
		for(YSSIZE_T idx : allNode.AllIndex())
		{
			if(slHd==allNode[idx])
			{
				nextSlHd=allNode.GetCyclic(idx+1);
				break;
			}
		}

printf("%s %d\n",__FUNCTION__,__LINE__);
		for(auto childPtr : slHd->children)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			shlGrp.Reconnect(childPtr,slHd->parent);
		}
printf("%s %d\n",__FUNCTION__,__LINE__);

		if(nextSlHd==slHd)
		{
			nextSlHd=NULL;
		}

		shlGrp.DeleteShell(slHd);

		Edit_ClearUIMode();

		if(NULL==nextSlHd)
		{
			File_New(NULL);
		}
		else
		{
			slHd=nextSlHd;
		}
		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
printf("%s %d\n",__FUNCTION__,__LINE__);
}

////////////////////////////////////////////////////////////

PolyCreSlideShowDialog::PolyCreSlideShowDialog()
{
	canvas=NULL;
	nextBtn=NULL;
	prevBtn=NULL;
	cancelBtn=NULL;
}

PolyCreSlideShowDialog::~PolyCreSlideShowDialog()
{
}

/*static*/ PolyCreSlideShowDialog *PolyCreSlideShowDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreSlideShowDialog *dlg=new PolyCreSlideShowDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreSlideShowDialog::Delete(PolyCreSlideShowDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreSlideShowDialog::Make(void)
{
	if(NULL==nextBtn)
	{
		FsGuiDialog::Initialize();
		changeViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_SLIDESHOW_CHANGEVIEW,YSFALSE);
		prevBtn=AddTextButton(0,FSKEY_LEFT,FSGUI_PUSHBUTTON,FSGUI_DLG_SLIDESHOW_PREV,YSFALSE);
		nextBtn=AddTextButton(0,FSKEY_RIGHT,FSGUI_PUSHBUTTON,FSGUI_DLG_SLIDESHOW_NEXT,YSFALSE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}

	return YSOK;
}

void PolyCreSlideShowDialog::OnButtonClick(FsGuiButton *btn)
{
	auto &canvas=*(this->canvas);
	if(btn==prevBtn)
	{
		const YsVec3 prevViewTarget=canvas.drawEnv.GetViewTarget();
		const double prevViewDist=canvas.drawEnv.GetViewDistance();
		const double prevOrthoHeght=canvas.drawEnv.GetOrthogonalProjectionHeight();
		const double prevZoom=canvas.drawEnv.GetZoom();

		if(0<fileNamePtr)
		{
			canvas.slHd=slHd;  // Tentatively make it current.  Current shell will (may) be deleted in the next loop.

			auto children=slHd->children; // Make a copy.  It changes as deleted.
			for(auto child : children)
			{
				canvas.shlGrp.DeleteShell(child);
			}
			--fileNamePtr;
			canvas.LoadGeneral(fileNameArray[fileNamePtr],slHd,YSFALSE);

			if(0==fileNamePtr)
			{
				prevBtn->Disable();
			}
			if(fileNamePtr+1<fileNameArray.GetN())
			{
				nextBtn->Enable();
			}
		}

		if(YSTRUE!=changeViewBtn->GetCheck())
		{
			canvas.drawEnv.SetViewTarget(prevViewTarget);
			canvas.drawEnv.SetViewDistance(prevViewDist);
			canvas.drawEnv.SetOrthogonalProjectionHeight(prevOrthoHeght);
			canvas.drawEnv.SetZoom(prevZoom);
		}
	}
	else if(btn==nextBtn)
	{
		const YsVec3 prevViewTarget=canvas.drawEnv.GetViewTarget();
		const double prevViewDist=canvas.drawEnv.GetViewDistance();
		const double prevOrthoHeght=canvas.drawEnv.GetOrthogonalProjectionHeight();
		const double prevZoom=canvas.drawEnv.GetZoom();

		if(fileNamePtr<fileNameArray.GetN()-1)
		{
			canvas.slHd=slHd;  // Tentatively make it current.  Current shell will (may) be deleted in the next loop.

			auto children=slHd->children; // Make a copy.  It changes as deleted.
			for(auto child : children)
			{
				canvas.shlGrp.DeleteShell(child);
			}
			++fileNamePtr;
			canvas.LoadGeneral(fileNameArray[fileNamePtr],slHd,YSFALSE);

			if(fileNamePtr==fileNameArray.GetN()-1)
			{
				nextBtn->Disable();
			}
			if(0<fileNamePtr)
			{
				prevBtn->Enable();
			}
		}

		if(YSTRUE!=changeViewBtn->GetCheck())
		{
			canvas.drawEnv.SetViewTarget(prevViewTarget);
			canvas.drawEnv.SetViewDistance(prevViewDist);
			canvas.drawEnv.SetOrthogonalProjectionHeight(prevOrthoHeght);
			canvas.drawEnv.SetZoom(prevZoom);
		}
	}
	else if(btn==cancelBtn)
	{
		canvas.Edit_ClearUIMode();  // This will call File_SlideShow_ModeCleanUpCallBack
	}
}

/*static*/ void GeblGuiEditorBase::File_SlideShow(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;

	YsString dataDir;
	dataDir.EncodeUTF8 <wchar_t> (canvas.GetDataDir());

	YsString def;
	def.MakeFullPathName(dataDir,"*.srf");
	// YsString last(scn.GetLastOpenedFilename()),def;
	// if(last.Strlen()>0)
	// {
	// 	YsString pth,fil;
	// 	last.SeparatePathFile(pth,fil);
	// 	def.MakeFullPathName(pth,"*.fld");
	// }
	// else
	// {
	// 	def.MakeFullPathName(scnDir,"*.fld");
	// }

	canvas.fdlg->Initialize();
	canvas.fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	canvas.fdlg->multiSelect=YSTRUE;
	canvas.fdlg->title.Set(L"Slide Show");
	canvas.fdlg->fileExtensionArray.Append(L".srf");
	canvas.fdlg->fileExtensionArray.Append(L".stl");
	canvas.fdlg->fileExtensionArray.Append(L".dnm");
	canvas.fdlg->defaultFileName.SetUTF8String(def);
	canvas.fdlg->SetCloseModalCallBack(File_SlideShow_FileSelected);
	canvas.AttachModalDialog(canvas.fdlg);
}

/*static*/ void GeblGuiEditorBase::File_SlideShow_FileSelected(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
		if(0<canvas.fdlg->selectedFileArray.GetN())
		{
			canvas.Edit_ClearUIMode();

			canvas.ModeCleanUpCallBack=File_SlideShow_ModeCleanUpCallBack;

			canvas.slideShowDlg->Make();
			canvas.AddDialog(canvas.slideShowDlg);
			canvas.ArrangeDialog();

			canvas.slideShowDlg->fileNamePtr=0;
			canvas.slideShowDlg->fileNameArray=canvas.fdlg->selectedFileArray;
			canvas.slideShowDlg->slHd=canvas.shlGrp.CreateShell(NULL);

			canvas.slideShowDlg->prevBtn->Disable();
			if(1>=canvas.fdlg->selectedFileArray.GetN())
			{
				canvas.slideShowDlg->nextBtn->Disable();
			}

			canvas.LoadGeneral(canvas.fdlg->selectedFileArray[0],canvas.slideShowDlg->slHd,YSTRUE);

			canvas.needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
			canvas.needRemakeDrawingBuffer_for_OtherShell=(unsigned int)NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

/*static*/ YSRESULT GeblGuiEditorBase::File_SlideShow_ModeCleanUpCallBack(GeblGuiEditorBase &canvas)
{
	canvas.shlGrp.DeleteShell(canvas.slideShowDlg->slHd);
	canvas.slHd=canvas.shlGrp.GetFirstRootNode();
	if(NULL==canvas.slHd)
	{
		canvas.File_New(NULL);
	}

	canvas.needRemakeDrawingBuffer=(unsigned int)GeblGuiEditorBase::NEED_REMAKE_DRAWING_ALL;
	canvas.needRemakeDrawingBuffer_for_OtherShell=(unsigned int)GeblGuiEditorBase::NEED_REMAKE_DRAWING_ALL;
	canvas.SetNeedRedraw(YSTRUE);

	return YSOK;
}

/* static */ void GeblGuiEditorBase::File_GetInfo_Statistics(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		const int nVtx=canvas.slHd->GetNumVertex();
		const int nPlg=canvas.slHd->GetNumPolygon();
		const int nCe=(int)canvas.slHd->GetNumConstEdge();
		const int nFg=(int)canvas.slHd->GetNumFaceGroup();

		YsString msg;
		msg.Printf(
		    "%d vertices.\n"
		    "%d polygons.\n"
		    "%d constraint edges.\n"
		    "%d face groups.\n",
		    nVtx,
		    nPlg,
		    nCe,
		    nFg);

		YsWString wMsg;
		wMsg.SetUTF8String(msg);
		canvas.MessageDialog(FSGUI_MENU_FILE_GETINFO_STATISTICS,wMsg,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
	}
}

/* static */ void GeblGuiEditorBase::File_GetInfo_Volume(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.slHd)
	{
		const double vol=canvas.slHd->ComputeVolume();

		YsString msg;
		msg.Printf("Volume %.10lf",vol);

		YsWString wMsg;
		wMsg.SetUTF8String(msg);
		canvas.MessageDialog(FSGUI_MENU_FILE_GETINFO_VOLUME,wMsg,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
	}
}

/* static */ void GeblGuiEditorBase::File_GetInfo_Length_BetweenVertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	auto slHd=canvas.Slhd();
	if(NULL!=slHd)
	{
		auto &shl=*slHd;
		auto selVtx=shl.GetSelectedVertex();

		if(2<=selVtx.GetN())
		{
			YsString msg;
			double total=0.0;
			auto prevPos=shl.GetVertexPosition(selVtx[0]);
			for(YSSIZE_T idx=1; idx<selVtx.GetN(); ++idx)
			{
				auto pos=shl.GetVertexPosition(selVtx[idx]);
				const auto L=(pos-prevPos).GetLength();

				YsString line;
				line.Printf("%.20lf\n",L);
				msg.Append(line);

				prevPos=pos;
				total+=L;
			}

			if(2<selVtx.GetN())
			{
				YsString line;
				line.Printf("Total: %.20lf\n",total);
			}

			YsWString wMsg;
			wMsg.SetUTF8String(msg);
			canvas.MessageDialog(FSGUI_COMMON_LENGTH,wMsg,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
		}
		else
		{
			canvas.MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT2VERTEX);
		}
	}
}

/* static */ void GeblGuiEditorBase::File_GetInfo_Length_ConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	auto slHd=canvas.Slhd();
	if(NULL!=slHd)
	{
		auto &shl=*slHd;
		auto selCe=shl.GetSelectedConstEdge();
		if(0<selCe.GetN())
		{
			YsString msg;
			for(auto ceHd : selCe)
			{
				YsArray <YsShellVertexHandle,16> ceVtHd;
				YSBOOL isLoop;
				shl.GetConstEdge(ceVtHd,isLoop,ceHd);

				if(2<=ceVtHd.GetN())
				{
					double L=0.0;
					for(YSSIZE_T idx=0; idx<ceVtHd.GetN()-1; ++idx)
					{
						L+=shl.GetEdgeLength(ceVtHd[idx],ceVtHd[idx+1]);
					}
					if(YSTRUE==isLoop)
					{
						L+=shl.GetEdgeLength(ceVtHd[0],ceVtHd.Last());
					}

					YsString line;
					line.Printf("%lf\n",L);
					msg.Append(line);
				}
			}

			YsWString wMsg;
			wMsg.SetUTF8String(msg);
			canvas.MessageDialog(FSGUI_COMMON_LENGTH,wMsg,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
		}
		else
		{
			canvas.MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST1CONSTEDGE);
		}
	}
}

/* static */ void GeblGuiEditorBase::File_GetInfo_Area_SelectedPolygon(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();
		auto selPlHd=shl.GetSelectedPolygon();
		double area=0.0;
		for(auto plHd : selPlHd)
		{
			area+=shl.GetPolygonArea(plHd);
		}

		YsString msg;
		msg.Printf("Area %lf\n",area);

		YsWString wMsg;
		wMsg.SetUTF8String(msg);
		canvas.MessageDialog(FSGUI_COMMON_LENGTH,wMsg,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
	}
}

/* static */ void GeblGuiEditorBase::File_GetInfo_Area_All(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();
		double area=0.0;
		for(auto plHd : shl.AllPolygon())
		{
			area+=shl.GetPolygonArea(plHd);
		}

		YsString msg;
		msg.Printf("Area %lf\n",area);

		YsWString wMsg;
		wMsg.SetUTF8String(msg);
		canvas.MessageDialog(FSGUI_COMMON_LENGTH,wMsg,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
	}
}

/* static */ void GeblGuiEditorBase::File_GetInfo_Position_SelectedVertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();
		auto selVtHd=shl.GetSelectedVertex();

		YsString msg;
		const int nShow=16;
		int ctr=nShow;
		for(auto vtHd : selVtHd)
		{
			auto pos=shl.GetVertexPosition(vtHd);
			YsString line;
			line.Printf("Id:%d (key:%d) (%.12lf, %.12lf, %.12lf)\n",shl.GetVertexIdFromHandle(vtHd),shl.GetSearchKey(vtHd),pos.x(),pos.y(),pos.z());
			msg.Append(line);

			--ctr;
			if(0==ctr)
			{
				break;
			}
		}

		if(nShow<selVtHd.GetN())
		{
			msg.Append("Too many vertices.  Showing first 16.");
		}

		YsWString wMsg;
		wMsg.SetUTF8String(msg);
		canvas.MessageDialog(FSGUI_DLG_VERTEXPOSITION_VERTEXPOSITION,wMsg,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
	}
}

void GeblGuiEditorBase::File_GetInfo_DebugInfo_Selection(FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();
		auto selVtHd=shl.GetSelectedVertex();
		auto selPlHd=shl.GetSelectedPolygon();
		auto selCeHd=shl.GetSelectedConstEdge();
		auto selFgHd=shl.GetSelectedFaceGroup();

		if(80<selVtHd.GetN()+selPlHd.GetN()+selCeHd.GetN()+selFgHd.GetN())
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			dlg->Make(FSGUI_ERROR_TOOMANYSELECTION,FSGUI_DLG_COMMON_ASKCOPYTOCLIPBOARD,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
			AttachModalDialog(dlg);

			RegisterCloseModalCallBack(&THISCLASS::File_GetInfo_DebugInfo_Selection_AskedCopyToClipboard);
		}
		else
		{
			File_GetInfo_DebugInfo_Selection_ShowOnDialog();
		}
	}
}

void GeblGuiEditorBase::File_GetInfo_DebugInfo_Selection_AskedCopyToClipboard(FsGuiDialog *dlg,int returnCode)
{
	if(YSOK==(YSRESULT)returnCode) // Yes to "copy to clipboard"
	{
		auto msg=File_GetInfo_DebugInfo_Selection_MakeMessage(YSFALSE);
		YsString utf8;
		utf8.EncodeUTF8(msg.Txt());
		FsGuiCopyStringToClipBoard(utf8);
	}
	else
	{
		File_GetInfo_DebugInfo_Selection_ShowOnDialog();
	}
}
void GeblGuiEditorBase::File_GetInfo_DebugInfo_Selection_ShowOnDialog(void)
{
	auto msg=File_GetInfo_DebugInfo_Selection_MakeMessage(YSTRUE);

	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
	dlg->Make(L"",msg,FSGUI_COMMON_OK,NULL,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
	AttachModalDialog(dlg);

}
YsWString GeblGuiEditorBase::File_GetInfo_DebugInfo_Selection_MakeMessage(YSBOOL first20) const
{
	YsString msg;
	if(NULL!=Slhd())
	{
		auto &shl=*(Slhd());

		shl.Encache();
		{
			auto selVtHd=shl.GetSelectedVertex();
			if(0<selVtHd.GetN())
			{
				msg.Append("[Selected Vertices]\n");
			}

			if(20<selVtHd.GetN() && YSTRUE==first20)
			{
				msg.Append("(First 20 selected vertices)\n");
			}

			int counter=0;
			for(auto vtHd : selVtHd)
			{
				auto pos=shl.GetVertexPosition(vtHd);
				YsString str;
				str.Printf("Vertex Id:%8d  Internal Hash Key:%8d  Position:(%.12lf, %.12lf, %.12lf)\n",
				    (int)shl.GetVertexIdFromHandle(vtHd),
				    (int)shl.GetSearchKey(vtHd),
				    pos.x(),pos.y(),pos.z());

				msg.Append(str);

				++counter;
				if(YSTRUE==first20 && 20<=counter)
				{
					break;
				}
			}
		}
		{
			auto selPlHd=shl.GetSelectedPolygon();
			if(0<selPlHd.GetN())
			{
				msg.Append("[Selected Polygons]\n");
			}

			if(20<selPlHd.GetN() && YSTRUE==first20)
			{
				msg.Append("(First 20 selected polygons)\n");
			}

			int counter=0;
			for(auto plHd : selPlHd)
			{
				YsString str;
				str.Printf("Polygon Id:%8d  Internal Hash Key:%8d  NVtx:%4d\n",
				    (int)shl.GetPolygonIdFromHandle(plHd),
				    (int)shl.GetSearchKey(plHd),
				    (int)shl.GetPolygonNumVertex(plHd));
				msg.Append(str);

				auto nom=shl.GetNormal(plHd);
				auto col=shl.GetColor(plHd);
				str.Printf("Normal:%s  Color:(%d,%d,%d)\n",
				    nom.Txt(),
				    col.Ri(),col.Gi(),col.Bi());
				msg.Append(str);

				{
					msg.Append("vtId(key):");

					auto plVtHd=shl.GetPolygonVertex(plHd);
					int subCounter=0;
					for(auto vtHd : plVtHd)
					{
						str.Printf(" %d(%d)",(int)shl.GetVertexIdFromHandle(vtHd),(int)shl.GetSearchKey(vtHd));
						msg.Append(str);

						++subCounter;
						if(YSTRUE==first20 && 20<=subCounter)
						{
							msg.Append(" ...");
							break;
						}
					}
					msg.Append("\n");
				}

				++counter;
				if(YSTRUE==first20 && 20<=counter)
				{
					break;
				}
			}
		}
		{
			auto selCeHd=shl.GetSelectedConstEdge();
			if(0<selCeHd.GetN())
			{
				msg.Append("[Selected Constraint Edges]\n");
			}

			if(20<selCeHd.GetN() && YSTRUE==first20)
			{
				msg.Append("(First 20 selected constraint edges)\n");
			}

			int counter=0;
			for(auto ceHd : selCeHd)
			{
				YsArray <YsShellVertexHandle,16> ceVtHd;
				YSBOOL isLoop;
				shl.GetConstEdge(ceVtHd,isLoop,ceHd);

				YsString str;
				str.Printf("Const Edge Id:%8d  Name:%s  NVtx:%4d  IsLoop:%s\n",
				   (int)shl.GetConstEdgeIdent(ceHd),
				   (const char *)shl.GetConstEdgeLabel(ceHd),
				   (int)ceVtHd.GetN(),
				   (const char *)YsBoolToStr(isLoop));
				msg.Append(str);

				{
					msg.Append("vtId(key):");

					int subCounter=0;
					for(auto vtHd : ceVtHd)
					{
						str.Printf(" %d(%d)",(int)shl.GetVertexIdFromHandle(vtHd),(int)shl.GetSearchKey(vtHd));
						msg.Append(str);

						++subCounter;
						if(YSTRUE==first20 && 20<=subCounter)
						{
							msg.Append(" ...");
							break;
						}
					}
					msg.Append("\n");
				}

				++counter;
				if(YSTRUE==first20 && 20<=counter)
				{
					break;
				}
			}
		}
		{
			auto selFgHd=shl.GetSelectedFaceGroup();
			if(0<selFgHd.GetN())
			{
				msg.Append("[Selected Face Groups]\n");
			}

			if(20<selFgHd.GetN() && YSTRUE==first20)
			{
				msg.Append("(First 20 selected face groups)\n");
			}

			int counter=0;
			for(auto fgHd : selFgHd)
			{
				auto fgPlHd=shl.GetFaceGroup(fgHd);

				YsString str;
				str.Printf("Face Group Id:%8d  Name:%s  NPlg:%4d\n",
				    (int)shl.GetFaceGroupIdent(fgHd),
				    (const char *)shl.GetFaceGroupLabel(fgHd),
				    (int)fgPlHd.GetN());
				msg.Append(str);

				{
					msg.Append("plId(key):");

					int subCounter=0;
					for(auto plHd : fgPlHd)
					{
						str.Printf(" %d(%d)",(int)shl.GetPolygonIdFromHandle(plHd),(int)shl.GetSearchKey(plHd));
						msg.Append(str);

						++subCounter;
						if(YSTRUE==first20 && 20<=subCounter)
						{
							msg.Append(" ...");
							break;
						}
					}
					msg.Append("\n");
				}

				++counter;
				if(YSTRUE==first20 && 20<=counter)
				{
					break;
				}
			}
		}
	}

	YsWString wmsg;
	wmsg.SetUTF8String(msg);
	return wmsg;
}

void GeblGuiEditorBase::File_GetInfo_MaxNormalDeviationAcrossConstEdge(FsGuiPopUpMenuItem *)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	YsString msg;
	if(NULL!=Slhd())
	{
		auto &shl=*(Slhd());

		shl.Encache();

		auto selCeHd=shl.GetSelectedConstEdge();
		if(20<selCeHd.GetN())
		{
			msg.Append("Showing first 20\n");
		}
		for(auto ceHd : selCeHd)
		{
			YsShellPolygonHandle plHd[2];
			const auto nomDev=YsShellExt_CalculateMaxNormalDeviationAcrossConstEdge(plHd,shl.Conv(),ceHd);
			YsString str;
			str.Printf("CeId=%d NormalDeviation=%.2lfdeg",
			    (int)shl.GetConstEdgeIdent(ceHd),
			    YsRadToDeg(nomDev));
			msg.Append(str);

			if(NULL!=plHd[0] && NULL!=plHd[1])
			{
				str.Printf("  Between plKey %d and plKey %d",
				    shl.GetSearchKey(plHd[0]),
				    shl.GetSearchKey(plHd[1]));
				msg.Append(str);
			}

			msg.Append("\n");
		}

		YsWString wmsg;
		wmsg.SetUTF8String(msg);

		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		dlg->Make(L"",wmsg,FSGUI_COMMON_OK,NULL,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
		AttachModalDialog(dlg);
	}
}

void GeblGuiEditorBase::File_GetInfo_AngleBetweenSelectedPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selPlHd=shl.GetSelectedPolygon();
		if(2==selPlHd.GetN())
		{
			const YsVec3 nom[2]=
			{
				shl.GetNormal(selPlHd[0]),
				shl.GetNormal(selPlHd[1]),
			};
			const double deg=YsRadToDeg(acos(YsBound(nom[0]*nom[1],-1.0,1.0)));

			YsString msg;
			msg.Printf("%.4lf degree",deg);

			YsWString wmsg;
			wmsg.SetUTF8String(msg);

			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
			dlg->Make(L"",wmsg,FSGUI_COMMON_OK,NULL,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
			AttachModalDialog(dlg);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECT2POLYGON);
		}
	}
}
void GeblGuiEditorBase::File_GetInfo_DihedralAngleAcrossSelectedEdge(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		auto selVtHd=shl.GetSelectedVertex();
		if(2<=selVtHd.GetN())
		{
			YsString msg;
			if(21<selVtHd.GetN())
			{
				selVtHd.Resize(21);
				msg.Printf("Showing first 20\n");
			}

			for(YSSIZE_T idx=0; idx<selVtHd.GetN()-1; ++idx)
			{
				YsString str;
				auto edPlHd=shl.FindPolygonFromEdgePiece(selVtHd[idx],selVtHd[idx+1]);
				if(2==edPlHd.GetN())
				{
					const YsVec3 nom[2]=
					{
						shl.GetNormal(edPlHd[idx]),
						shl.GetNormal(edPlHd[idx+1]),
					};
					const double deg=YsRadToDeg(acos(YsBound(nom[0]*nom[1],-1.0,1.0)));
					str.Printf("%.4lf degree\n",deg);
				}
				else
				{
					str="Unused or Non-Manifold Edge\n";
				}
				msg.Append(str);
			}

			YsWString wmsg;
			wmsg.SetUTF8String(msg);

			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
			dlg->Make(L"",wmsg,FSGUI_COMMON_OK,NULL,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
			AttachModalDialog(dlg);
		}
		else
		{
			MessageDialog(FSGUI_COMMON_ERROR,FSGUI_ERROR_NEEDSELECTATLEAST2VERTEX);
		}
	}
}

void GeblGuiEditorBase::File_GetInfo_MinMaxAvgDihedralAngle(FsGuiPopUpMenuItem *itm)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		YSSIZE_T nSample=0;
		double minDha=0.0,maxDha=0.0,avgDha=0.0;
		for(auto plHd : shl.AllPolygon())
		{
			for(YSSIZE_T edIdx=0; edIdx<shl.GetPolygonNumVertex(plHd); ++edIdx)
			{
				auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
				if(nullptr!=neiPlHd)
				{
					auto dha=YsShellExt_CalculateDihedralAngle(shl.Conv(),plHd,neiPlHd);
					if(0==nSample)
					{
						minDha=dha;
						maxDha=dha;
					}
					else
					{
						YsMakeSmaller(minDha,dha);
						YsMakeGreater(maxDha,dha);
					}
					avgDha+=dha;
					++nSample;
				}
			}
		}
		if(0<nSample)
		{
			avgDha/=(double)nSample;
		}

		YsString msg;
		YsString str;
		str.Printf("Min Dha=%lf deg\n",YsRadToDeg(minDha));
		msg.Append(str);
		str.Printf("Max Dha=%lf deg\n",YsRadToDeg(maxDha));
		msg.Append(str);
		str.Printf("Average Dha=%lf deg\n",YsRadToDeg(avgDha));
		msg.Append(str);

		YsWString wmsg;
		wmsg.SetUTF8String(msg);

		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		dlg->Make(L"",wmsg,FSGUI_COMMON_OK,NULL,FSGUI_DLG_COMMON_COPYTOCLIPBOARD);
		AttachModalDialog(dlg);
	}
}

/* static */ void GeblGuiEditorBase::File_Recent(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *itm)
{
	const wchar_t *wfn=itm->GetString();
	auto &canvas=*(GeblGuiEditorBase *)appPtr;
	canvas.LoadGeneral(wfn,NULL,YSTRUE);
}
