#include <thread>

#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysglslcpp.h>
#include <ysport.h>

#include <fsguifiledialog.h>

#include "../fsgui3dapp.h"
#include "../textresource.h"



void FsGui3DMainCanvas::File_InsertImage(FsGuiPopUpMenuItem *)
{
	auto defFn=world.GetLastUsedImageFile();
	if(0==defFn.Strlen())
	{
		defFn=L"*.png";
	}
	else
	{
		YsWString pth,fil;
		defFn.SeparatePathFile(pth,fil);
		defFn.MakeFullPathName(pth,L"*.png");
	}

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSTRUE;
	fdlg->title.Set(L"Insert Image");
	fdlg->fileExtensionArray.Append(L".png");
	fdlg->defaultFileName=defFn;
	fdlg->BindCloseModalCallBack(&THISCLASS::File_InsertImage_Open,this);
	AttachModalDialog(fdlg);
}
void FsGui3DMainCanvas::File_InsertImage_Open(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode && 0<fdlg->selectedFileArray.GetN())
	{
		auto fdHd=GetCurrentFieldCreateIfNotExist();

		auto fieldPtr=world.GetField(fdHd);
		auto nMapPiece=fieldPtr->GetNumMapPiece();

		RetroMap_World::MapPieceHandle mpHd=nullptr;
		world.UnselectAllMapPiece(fdHd);
		for(auto fn : fdlg->selectedFileArray)
		{
			mpHd=MapPiece_Insert_Insert(fdHd,fn);
		}
		if(mpHd!=nullptr)
		{
			RetroMap_World::MapPieceStore mpStore;
			mpStore.Add(mpHd);
			UpdateDiff(mpStore,mpStore);
		}

		if(0==nMapPiece)
		{
			auto bbx=world.GetBoundingBox(fdHd);
			drawEnv.SetViewTarget(bbx.GetCenter());
			drawEnv.SetTargetBoundingBox(bbx.Min(),bbx.Max());
		}
	}
}

RetroMap_World::MapPieceHandle FsGui3DMainCanvas::MapPiece_Insert_Insert(RetroMap_World::FieldHandle fdHd,const YsWString &fn)
{
	YsFileIO::File fp(fn,"rb");
	YsArray <unsigned char> buf(65536,nullptr);
	YsArray <unsigned char> binData;
	size_t readSize;
	while(0<(readSize=fread(buf,1,buf.GetN(),fp)))
	{
		binData.Add(readSize,buf);
	}

	auto o=drawEnv.GetViewPosition(),v=drawEnv.GetViewDirection();

	YsVec2i pos=YsVec2i::Origin();
	YsPlane pln(YsOrigin(),YsZVec());
	YsVec3 crs;
	if(YSOK==pln.GetIntersection(crs,o,v))
	{
		pos.Set((int)crs.x(),(int)crs.y());
	}


	auto mpHd=world.LoadImage(fdHd,pos,YsTextureManager::FOM_PNG,binData);
	world.ReadyVbo(fdHd,mpHd);

	world.UnselectAllMarkUp(fdHd);
	world.AddSelectedMapPiece(fdHd,mpHd);

	world.SetLastUsedImageFile(fn);

	UpdateNumBmp();

	return mpHd;
}


void FsGui3DMainCanvas::MapPiece_InsertNewScreenshot(FsGuiPopUpMenuItem *)
{
	if(YSTRUE!=config.IsScreenshotFolderSelected())
	{
		auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		msgDlg->Make(L"",FSGUI_DLG_NEED_CONFIGURE_SCRNSHOTDIR,FSGUI_COMMON_OK,nullptr);
		AttachModalDialog(msgDlg);
	}
	else
	{
		auto fn=config.GetNewScreenshotFileName();
		printf("[%s]\n",fn.GetUTF8String().Txt());
		if(0<fn.Strlen())
		{
			auto fdHd=GetCurrentFieldCreateIfNotExist();

			auto fieldPtr=world.GetField(fdHd);
			auto nMapPiece=fieldPtr->GetNumMapPiece();

			world.UnselectAllMapPiece(fdHd);

			auto mpHd=MapPiece_Insert_Insert(fdHd,fn);
			if(mpHd!=nullptr)
			{
				RetroMap_World::MapPieceStore mpStore;
				mpStore.Add(mpHd);
				UpdateDiff(mpStore,mpStore);
			}

			if(0==nMapPiece)
			{
				auto bbx=world.GetBoundingBox(fdHd);
				drawEnv.SetViewTarget(bbx.GetCenter());
				drawEnv.SetTargetBoundingBox(bbx.Min(),bbx.Max());
			}

			UpdateNumBmp();
			SetNeedRedraw(YSTRUE);
		}
	}
}

void FsGui3DMainCanvas::MapPiece_MoveToFront(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard ug(world);

	auto fdHd=GetCurrentField();
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		decltype(selMpHd) newSelMpHd;
		auto toMpHd=fieldPtr->LastMapPiece();
		YsSimpleMergeSort<RetroMap_World::MapPieceHandle,int>(selMpHd.GetN(),selMpHd,nullptr);
		for(auto idx : selMpHd.ReverseIndex())
		{
			newSelMpHd.Add(toMpHd);
			world.ChangePriority(fdHd,selMpHd[idx],toMpHd);
			toMpHd=fieldPtr->FindPrev(toMpHd);
			if(nullptr==toMpHd)
			{
				break;
			}
		}
		world.SelectMapPiece(fdHd,newSelMpHd);
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::MapPiece_MoveToBack(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard ug(world);

	auto fdHd=GetCurrentField();
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		decltype(selMpHd) newSelMpHd;
		auto toMpHd=fieldPtr->FirstMapPiece();
		YsSimpleMergeSort<RetroMap_World::MapPieceHandle,int>(selMpHd.GetN(),selMpHd,nullptr);
		for(auto idx : selMpHd.AllIndex())
		{
			newSelMpHd.Add(toMpHd);
			world.ChangePriority(fdHd,selMpHd[idx],toMpHd);
			toMpHd=fieldPtr->FindNext(toMpHd);
			if(nullptr==toMpHd)
			{
				break;
			}
		}
		world.SelectMapPiece(fdHd,newSelMpHd);
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::MapPiece_RecacheScreenshot(FsGuiPopUpMenuItem *)
{
	config.UpdateScreenshotCache();
}


void FsGui3DMainCanvas::MapPiece_Untrim(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard ug(world);

	auto fdHd=GetCurrentField();
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : world.SelectedMapPiece(fdHd))
		{
			world.MakeAllVisible(fdHd,mpHd);
		}
		world.ReadyVbo(fdHd);
		SetNeedRedraw(YSTRUE);
	}
}

////////////////////////////////////////////////////////////

class FsGui3DMainCanvas::TrimWithLargeRectangleDialog : public FsGuiDialog
{
public:
	FsGui3DMainCanvas *canvasPtr;
	FsGuiButton *trimBtn,*cancelBtn;
	FsGuiButton *useUnitBtn;
	FsGuiStatic *coordTxt;

	void Make(FsGui3DMainCanvas *canvasPtr);
	void UpdateCoord(void);
	virtual void OnButtonClick(FsGuiButton *btn);
};
void FsGui3DMainCanvas::TrimWithLargeRectangleDialog::Make(FsGui3DMainCanvas *canvasPtr)
{
	this->canvasPtr=canvasPtr;
	FsGuiDialog::Initialize();
	trimBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSFALSE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
	useUnitBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_TRIM_USE_UNIT,YSTRUE);
	useUnitBtn->SetCheck(YSTRUE);
	coordTxt=AddStaticText(0,FSKEY_NULL,"(0000,0000)-(0000,0000) W:0000 H:0000",YSFALSE);
	Fit();
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
}
void FsGui3DMainCanvas::TrimWithLargeRectangleDialog::UpdateCoord(void)
{
	auto largeRect=canvasPtr->MapPiece_TrimSelectionWithLargeRectangle_GetLargeRect();
	auto largeRectSize=largeRect.GetSize();
	YsString msg;
	msg.Printf("(%d,%d)-(%d,%d) W:%d H:%d\n",
	    largeRect[0].x(),largeRect[0].y(),
	    largeRect[1].x(),largeRect[1].y(),
	    largeRectSize.x(),largeRectSize.y());
	coordTxt->SetText(msg);
}
/* virtual */ void FsGui3DMainCanvas::TrimWithLargeRectangleDialog::OnButtonClick(FsGuiButton *btn)
{
	if(cancelBtn==btn)
	{
		canvasPtr->Edit_ClearUIMode(nullptr);
		return;
	}
	if(trimBtn==btn)
	{
		RetroMap_World::UndoGuard ug(canvasPtr->world);

		auto fdHd=canvasPtr->GetCurrentField();
		auto fieldPtr=canvasPtr->world.GetField(fdHd);
		for(auto mpHd : canvasPtr->world.SelectedMapPiece(fdHd))
		{
			auto mapPiecePtr=fieldPtr->GetMapPiece(mpHd);
			auto curRect=mapPiecePtr->GetBoundingBox2i();
			auto largeRect=canvasPtr->MapPiece_TrimSelectionWithLargeRectangle_GetLargeRect();

			YsRect2i overlap;
			curRect.GetOverlap(overlap,largeRect);

			YsVec2 min2d,max2d;
			min2d.Set(overlap[0].x(),overlap[0].y());
			max2d.Set(overlap[1].x(),overlap[1].y());

			auto trimRect=mapPiecePtr->GetShape().VisibleAreaFromWorldXY(min2d,max2d);
			canvasPtr->world.SetTrimRect(fdHd,mpHd,trimRect);
			canvasPtr->world.ReadyVbo(fdHd,mpHd);
		}

		canvasPtr->Edit_ClearUIMode(nullptr);
	}
}

void FsGui3DMainCanvas::MapPiece_TrimSelectionWithLargeRectangle(FsGuiPopUpMenuItem *)
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

			YsBoundingBoxMaker <YsVec3> allBbx;
			for(auto mpHd : selMpHd)
			{
				auto mapPiecePtr=fieldPtr->GetMapPiece(mpHd);
				auto rect=mapPiecePtr->GetBoundingBox();
				allBbx.Add(rect[0]);
				allBbx.Add(rect[1]);
			}

			threeDInterface.BeginInputBox(allBbx[0],allBbx[1],YSTRUE,YSFALSE);
			threeDInterface.BindCallBack(&THISCLASS::MapPiece_TrimSelectionWithLargeRectangle_3DInterfaceCallBack,this);

			draw3dCallBack=std::bind(&THISCLASS::MapPiece_TrimSelectionWithLargeRectangle_DrawCallBack,this);

			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<TrimWithLargeRectangleDialog>();
			dlg->canvasPtr=this;
			dlg->Make(this);
			dlg->UpdateCoord();
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
void FsGui3DMainCanvas::MapPiece_TrimSelectionWithLargeRectangle_3DInterfaceCallBack(FsGui3DInterface *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto fieldPtr=world.GetField(fdHd);
		const auto largeRect=MapPiece_TrimSelectionWithLargeRectangle_GetLargeRect();
		for(auto mpHd : world.SelectedMapPiece(fdHd))
		{
			auto mapPiecePtr=fieldPtr->GetMapPiece(mpHd);
			auto curRect=mapPiecePtr->GetBoundingBox2i();

			YsRect2i overlap;
			curRect.GetOverlap(overlap,largeRect);

			YsVec2 min2d,max2d;
			min2d.Set(overlap[0].x(),overlap[0].y());
			max2d.Set(overlap[1].x(),overlap[1].y());

			auto trimRect=mapPiecePtr->GetShape().VisibleAreaFromWorldXY(min2d,max2d);
			auto shape=mapPiecePtr->GetShape();
			shape.visibleArea=trimRect;
			world.ReadyVbo(fdHd,mpHd,shape,1.0);
		}
		auto dlgPtr=this->FindTypedModelessDialog<TrimWithLargeRectangleDialog>();
		if(nullptr!=dlgPtr)
		{
			dlgPtr->UpdateCoord();
		}
	}
}
void FsGui3DMainCanvas::MapPiece_TrimSelectionWithLargeRectangle_DrawCallBack(void)
{
	auto rect=MapPiece_TrimSelectionWithLargeRectangle_GetLargeRect();

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

YsRect2i FsGui3DMainCanvas::MapPiece_TrimSelectionWithLargeRectangle_GetLargeRect(void) const
{
	YsRect2i trimRect(YsVec2i::Origin(),YsVec2i::Origin());

	auto min3d=threeDInterface.box_pos[0];
	auto max3d=threeDInterface.box_pos[1];

	YsVec2i min,max;
	min.Set((int)min3d.x(),(int)min3d.y());
	max.Set((int)max3d.x(),(int)max3d.y());

	auto dlgPtr=this->FindTypedModelessDialog<TrimWithLargeRectangleDialog>();
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

	trimRect.Set(min,max);
	return trimRect;
}

void FsGui3DMainCanvas::MapPiece_ReapplyTrimAll(FsGuiPopUpMenuItem *)
{
	RetroMap_World::UndoGuard ug(world);

	auto fdHd=GetCurrentField();
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		for(auto mpHd : world.AllMapPiece(fdHd))
		{
			auto mpPtr=world.GetMapPiece(fdHd,mpHd);
			auto shape=mpPtr->GetShape();
			shape.ApplyUnit(world.GetUnit());
			world.SetPosition(fdHd,mpHd,shape.pos);
		}
		world.ReadyVbo(fdHd);
		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::MapPiece_MinimizeErrorThreadInfo::Run(void)
{
	YsBitmap bmp;
	this->diffInfo=this->worldPtr->MakeDiffBitmap(bmp,this->fdHd,this->mpHd,this->shape,*(this->exclusionPtr),this->thr,this->diffColor);
}

void FsGui3DMainCanvas::MapPiece_MinimizeError(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	auto fieldPtr=world.GetField(fdHd);
	if(nullptr!=fieldPtr)
	{
		auto selMpHd=world.SelectedMapPiece(fdHd);
		if(1==selMpHd.GetN())
		{
			auto mpPtr=world.GetMapPiece(fdHd,selMpHd[0]);
			if(nullptr!=mpPtr)
			{
				RetroMap_World::MapPieceStore mpStore;
				mpStore.Add(selMpHd[0]);

				auto shape=mpPtr->GetShape();
				auto unit=world.GetUnit();
				auto mpHd=selMpHd[0];

				YSBOOL improvement=YSFALSE;
				YsBitmap bmp;
				auto diff0=world.MakeDiffBitmap(bmp,fdHd,mpHd,shape,mpStore,GetDiffThreshold(),YsRed());
				if(0<diff0.numSample)
				{
					auto curDiff=diff0;
					for(int step=0; step<16; ++step)
					{
						int offset[8]={-1,0, 0,-1, 1,0, 0,1};
						MapPiece_MinimizeErrorThreadInfo threadInfo[4];
						std::thread thr[4];
						for(int i=0; i<4; ++i)
						{
							threadInfo[i].worldPtr=&world;
							threadInfo[i].fdHd=fdHd;
							threadInfo[i].mpHd=selMpHd[0];
							threadInfo[i].exclusionPtr=&mpStore;
							threadInfo[i].shape=shape;
							threadInfo[i].thr=GetDiffThreshold();
							threadInfo[i].diffColor=YsRed();

							threadInfo[i].shape.pos.AddX(unit.x()*offset[i*2]);
							threadInfo[i].shape.pos.AddY(unit.y()*offset[i*2+1]);

							std::thread t(&MapPiece_MinimizeErrorThreadInfo::Run,&threadInfo[i]);
							thr[i].swap(t);
						}
						for(auto &t : thr)
						{
							t.join();
						}
						YSBOOL stepImprovement=YSFALSE;
						for(auto &i : threadInfo)
						{
							if(0<i.diffInfo.numSample)
							{
								double curScore=(double)curDiff.totalError/(double)curDiff.numSample;
								double newScore=(double)i.diffInfo.totalError/(double)i.diffInfo.numSample;
								if(newScore<curScore)
								{
									stepImprovement=YSTRUE;
									curDiff=i.diffInfo;
									shape=i.shape;
								}
							}
						}
						if(YSTRUE==stepImprovement)
						{
							improvement=YSTRUE;
						}
						else
						{
							break;
						}
					}
					if(YSTRUE==improvement)
					{
						world.SetPosition(fdHd,mpHd,shape.pos);
						world.ReadyVbo(fdHd);
						world.MakeDiff(fdHd,mpStore,mpStore,GetDiffThreshold(),YsRed());// Diff depends on the vertex-buffer.  Must be after ReadyVbo
						SetNeedRedraw(YSTRUE);
					}
				}
			}
		}
		else
		{
			auto msgDlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiMessageBoxDialog>();
			msgDlg->Make(FSGUI_COMMON_ERROR,FSGUI_DLG_COMMON_NEED_SELECT_ONE_MAPPIECE,FSGUI_COMMON_OK,nullptr);
			AttachModalDialog(msgDlg);
		}
	}
}
