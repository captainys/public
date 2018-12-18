/* ////////////////////////////////////////////////////////////

File Name: refbmpdialog.cpp
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
#include "refbmp.h"
#include "refbmpdialog.h"

#include <fsguifiledialog.h>

#include "../textresource.h"

PolyCreRefBmpDialog::PolyCreRefBmpDialog()
{
	fdlg=new FsGuiFileDialog;
	mbox=new FsGuiMessageBoxDialog;
}

PolyCreRefBmpDialog::~PolyCreRefBmpDialog()
{
	delete fdlg;
	delete mbox;
}


/*static*/ PolyCreRefBmpDialog *PolyCreRefBmpDialog::Create(void)
{
	return new PolyCreRefBmpDialog;
}

/*static*/ void PolyCreRefBmpDialog::Delete(PolyCreRefBmpDialog *ptr)
{
	delete ptr;
}

void PolyCreRefBmpDialog::Make(class GeblGuiEditorBase *canvas,class PolyCreReferenceBitmapStore *refBmpStore)
{
	this->canvas=canvas;
	this->refBmpStore=refBmpStore;

	FsGuiDialog::Initialize();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);

	bmpIdxDrp=AddEmptyDropList(0,FSKEY_NULL,"Image #",PolyCreReferenceBitmapStore::MAX_NUM_REFBMP,32,32,YSTRUE);
	for(int i=0; i<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP; ++i)
	{
		YsString label;
		label.Printf("%2d:",i+1);
		bmpIdxDrp->AddString(label,(0==i ? YSTRUE : YSFALSE));
	}

	centerBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_COMMON_CENTER,YSTRUE);
	xyBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_REFBMP_XY,YSFALSE);
	yzBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_REFBMP_YZ,YSFALSE);
	xzBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_REFBMP_XZ,YSFALSE);

	moveBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_REFBMP_MOVESCALE,YSTRUE);
	rotateBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_COMMON_ROTATE,YSFALSE);

	loadBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_REFBMP_LOADIMAGE,YSTRUE);
	discardBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_REFBMP_DISCARDIMAGE,YSFALSE);

	showBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_COMMON_SHOW,YSTRUE);

	closeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSTRUE);

	transparencySlider=AddHorizontalSlider(0,FSKEY_NULL,50,0.0,1.0,YSTRUE);
	transparencySlider->SetPosition(refBmpStore->refBmp[0].alpha);

	ConfigureForBitmapIndex(0);

	SetTransparency(YSFALSE);
	Fit();
}

int PolyCreRefBmpDialog::GetSelectedBitmapIndex(void) const
{
	return bmpIdxDrp->GetSelection();
}

void PolyCreRefBmpDialog::OnButtonClick(FsGuiButton *btn)
{
	if(closeBtn==btn)
	{
		canvas->Edit_ClearUIMode();

		canvas->RemoveDialog(this);
		canvas->SetNeedRedraw(YSTRUE);
	}
	else if(loadBtn==btn)
	{
		YsString dataDir;
		dataDir.EncodeUTF8 <wchar_t> (canvas->GetDataDir());

		YsString def;
		def.MakeFullPathName(dataDir,"*.png");

		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->multiSelect=YSTRUE;
		fdlg->title.Set(L"Open Bitmap");
		fdlg->fileExtensionArray.Append(L".png");
		fdlg->fileExtensionArray.Append(L".bmp");
		fdlg->defaultFileName.SetUTF8String(def);
		AttachModalDialog(fdlg);
	}
	else if(discardBtn==btn)
	{
		const int sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP && YSTRUE==refBmpStore->refBmp[sel].loaded)
		{
			YsWString msg;
			msg.Set(L"Discard ");
			msg.Append(refBmpStore->refBmp[sel].fn);
			msg.Append(L"?");

			mbox->Make(L"Discard this image?",msg,L"Yes",L"No",1,0);
			AttachModalDialog(mbox);
		}
	}
	else if(xyBtn==btn)
	{
		const int sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			refBmpStore->refBmp[sel].MakeXY();
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	else if(xzBtn==btn)
	{
		const int sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			refBmpStore->refBmp[sel].MakeXZ();
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	else if(yzBtn==btn)
	{
		const int sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			refBmpStore->refBmp[sel].MakeYZ();
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	else if(moveBtn==btn)
	{
		const int sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			YsBoundingBoxMaker3 mkBbx;
			mkBbx.Make(4,refBmpStore->refBmp[sel].corner);

			orgCorner[0]=refBmpStore->refBmp[sel].corner[0];
			orgCorner[1]=refBmpStore->refBmp[sel].corner[1];
			orgCorner[2]=refBmpStore->refBmp[sel].corner[2];
			orgCorner[3]=refBmpStore->refBmp[sel].corner[3];

			YsVec3 bbx[2];
			mkBbx.Get(bbx[0],bbx[1]);

			const YsVec3 d=bbx[1]-bbx[0];
			const double diagonal=d.GetLength();
			if(YsTolerance>d.x())
			{
				bbx[0].SubX(diagonal/10.0);
				bbx[1].AddX(diagonal/10.0);
			}
			if(YsTolerance>d.y())
			{
				bbx[0].SubY(diagonal/10.0);
				bbx[1].AddY(diagonal/10.0);
			}
			if(YsTolerance>d.z())
			{
				bbx[0].SubZ(diagonal/10.0);
				bbx[1].AddZ(diagonal/10.0);
			}

			canvas->threeDInterface.BeginInputBox(bbx[0],bbx[1],/*resize=*/YSTRUE,/*maintain aspect=*/YSTRUE);
			canvas->threeDInterface.SetCallBack(ThreeDInterfaceCallBack,this);
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	else if(rotateBtn==btn)
	{
		const int sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			orgCorner[0]=refBmpStore->refBmp[sel].corner[0];
			orgCorner[1]=refBmpStore->refBmp[sel].corner[1];
			orgCorner[2]=refBmpStore->refBmp[sel].corner[2];
			orgCorner[3]=refBmpStore->refBmp[sel].corner[3];

			const YsVec3 cen=(refBmpStore->refBmp[sel].corner[0]
			                 +refBmpStore->refBmp[sel].corner[1]
			                 +refBmpStore->refBmp[sel].corner[2]
			                 +refBmpStore->refBmp[sel].corner[3])/4.0;
			YsVec3 nom;
			YsGetAverageNormalVector(nom,4,refBmpStore->refBmp[sel].corner);

			if(YsOrigin()==nom)
			{
				nom=YsYVec();
			}

			const double d=(refBmpStore->refBmp[sel].corner[2]-refBmpStore->refBmp[sel].corner[0]).GetLength();

			canvas->threeDInterface.BeginInputRotation(nom,0.0,cen,d/3.0,YSTRUE,YSTRUE,YSTRUE);
			canvas->threeDInterface.SetCallBack(ThreeDInterfaceCallBack,this);
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	else if(showBtn==btn)
	{
		const auto sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			refBmpStore->refBmp[sel].show=showBtn->GetCheck();
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	else if(centerBtn==btn)
	{
		const auto sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			refBmpStore->refBmp[sel].alignCenter=centerBtn->GetCheck();
			refBmpStore->refBmp[sel].ApplyScalingAndPadding();
			refBmpStore->refBmp[sel].MakeTransparency();
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
}

void PolyCreRefBmpDialog::OnModalDialogClosed(int,class FsGuiDialog *closedModalDialog,int returnValue)
{
	if(fdlg==closedModalDialog && YSOK==(YSRESULT)returnValue)
	{
		for(int fileIdx=0; fileIdx<fdlg->selectedFileArray.GetN(); ++fileIdx)
		{
			for(int bmpIdx=0; bmpIdx<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP; ++bmpIdx)
			{
				if(YSTRUE!=refBmpStore->refBmp[bmpIdx].loaded)
				{
					YsString fn;
					fn.EncodeUTF8 <wchar_t> (fdlg->selectedFileArray[fileIdx]);
					if(YSOK==refBmpStore->refBmp[bmpIdx].Load(fn))
					{
						refBmpStore->refBmp[bmpIdx].MakeTentativeCorner();

						YsString pth,fil;
						fn.SeparatePathFile(pth,fil);

						YsString label;
						label.Printf("%2d: %s",bmpIdx+1,fil.Txt());
						bmpIdxDrp->SetString(bmpIdx,label);
					}
					else
					{
						// Better than crash
					}
					break;
				}
			}
		}
		canvas->SetNeedRedraw(YSTRUE);
	}
	else if(mbox==closedModalDialog && 0!=returnValue)
	{
		const int sel=bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			refBmpStore->refBmp[sel].loaded=YSFALSE;
			YsString msg;
			msg.Printf("%2d:",sel+1);
			bmpIdxDrp->SetString(sel,msg);
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
}

void PolyCreRefBmpDialog::OnDropListSelChange(FsGuiDropList *drp,int prevSel)
{
	if(drp==bmpIdxDrp)
	{
		if(prevSel!=drp->GetSelection())
		{
			ConfigureForBitmapIndex(drp->GetSelection());
			canvas->threeDInterface.Initialize();
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
}

/*virtual*/ void PolyCreRefBmpDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &,const double &)
{
	const double trsp=slider->GetPosition();
	for(auto &bmp : refBmpStore->refBmp)
	{
		bmp.alpha=trsp;
	}
	canvas->SetNeedRedraw(YSTRUE);
}

/*static*/ void PolyCreRefBmpDialog::ThreeDInterfaceCallBack(void *contextPtr,FsGui3DInterface &itfc)
{
	PolyCreRefBmpDialog *refBmpDlg=(PolyCreRefBmpDialog *)contextPtr;
printf("%s %d\n",__FUNCTION__,__LINE__);

	if(FsGui3DInterface::IFTYPE_BOX==itfc.GetInterfaceType())
	{
		const int sel=refBmpDlg->bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			for(int i=0; i<4; ++i)
			{
				const YsVec3 orgBox0=itfc.box_pos_org[0];
				const YsVec3 orgBoxD=itfc.box_pos_org[1]-itfc.box_pos_org[0];
				const double u=(refBmpDlg->orgCorner[i].x()-orgBox0.x())/orgBoxD.x();
				const double v=(refBmpDlg->orgCorner[i].y()-orgBox0.y())/orgBoxD.y();
				const double w=(refBmpDlg->orgCorner[i].z()-orgBox0.z())/orgBoxD.z();

				const YsVec3 newBox0=itfc.box_pos[0];
				const YsVec3 newBoxD=itfc.box_pos[1]-itfc.box_pos[0];
				const YsVec3 newPos(
				    newBox0.x()+newBoxD.x()*u,
				    newBox0.y()+newBoxD.y()*v,
				    newBox0.z()+newBoxD.z()*w);

				refBmpDlg->refBmpStore->refBmp[sel].corner[i]=newPos;
			}
		}
		refBmpDlg->canvas->SetNeedRedraw(YSTRUE);
	}
	else if(FsGui3DInterface::IFTYPE_ROTATION==itfc.GetInterfaceType())
	{
		YsMatrix4x4 mat;
		itfc.GetRotationMatrix(mat);

		const int sel=refBmpDlg->bmpIdxDrp->GetSelection();
		if(0<=sel && sel<PolyCreReferenceBitmapStore::MAX_NUM_REFBMP)
		{
			for(int i=0; i<4; ++i)
			{
				refBmpDlg->refBmpStore->refBmp[sel].corner[i]=mat*refBmpDlg->orgCorner[i];
			}
		}

		refBmpDlg->canvas->SetNeedRedraw(YSTRUE);
	}
}

void PolyCreRefBmpDialog::ConfigureForBitmapIndex(int bmpIdx)
{
	auto &bmp=refBmpStore->refBmp[bmpIdx];
	showBtn->SetCheck(bmp.show);
	centerBtn->SetCheck(bmp.alignCenter);
}

