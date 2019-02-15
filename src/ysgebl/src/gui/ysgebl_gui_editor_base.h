/* ////////////////////////////////////////////////////////////

File Name: ysgebl_gui_editor_base.h
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

#ifndef FSGUI_DAPP_IS_INCLUDED
#define FSGUI_DAPP_IS_INCLUDED
/* { */

#include <thread>
#include <mutex>
#include <functional>

#include <fsgui3d.h>

#include <ysshellkernel.h>
#include <ysglbuffer.h>

#include <ysgebl_gui_foundation.h>

#include <ysshellextedit_duplicateutil.h>
#include <ysshellext_alignmentutil.h>

#include "filename.h"

// Call back functions from the main framework. >>
int FsGui3dAppSleepIntervalInMS(void);
class GeblGuiEditorBase *FsGui3DAppInitialize(int ac,char *av[]);
// Call back functions from the main framework. <<

class GeblGuiEditorMainMenu : public FsGuiPopUpMenu
{
private:
	class GeblGuiEditorBase *canvas;
public:
	GeblGuiEditorMainMenu(class GeblGuiEditorBase *canvas);
	virtual void OnSelectMenuItem(FsGuiPopUpMenuItem *item);

	class GeblGuiEditorBase *GetMainCanvas(void) const;
};

class GeblGuiEditorBase : public GeblGuiFoundation
{
friend class PolyCreProjectionDialog;
friend class PolyCreEditSweepConstEdgeDialog;
friend class PolyCreSlideShowDialog;
friend class PolyCreDnmTreeDialog;
friend class GeblGuiExtension;

public:
	typedef GeblGuiEditorBase THISCLASS;

	YSBOOL mustTerminate;

	class ShortCutKey
	{
	public:
		void (*callBackFunc)(void *,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
		int fskey;
		YSBOOL ctrl,shift;
	};

	enum
	{
		MENU_FILE_EXIT
	};



	FsGuiMenuCallBackConnector <THISCLASS> menuConn;
	int menuIdSource;
	FsGuiPopUpMenuItem *AddMenuItem(FsGuiPopUpMenu *parent,int fsKey,const char label[],void (THISCLASS::*funcPtr)(FsGuiPopUpMenuItem *));
	FsGuiPopUpMenuItem *AddMenuItem(FsGuiPopUpMenu *parent,int fsKey,const wchar_t label[],void (THISCLASS::*funcPtr)(FsGuiPopUpMenuItem *));

	FsGuiDialogCallBackFunction <THISCLASS,int> closeModalCallBack;
	void RegisterCloseModalCallBack(void (THISCLASS::*funcPtr)(FsGuiDialog *,int));
	std::function <void()> draw3dCallBack;
	std::function <void()> draw2dCallBack;
	std::function <void()> spaceKeyCallBack;
	std::function <void()> modeCleanUpCallBack;
	std::function <YSRESULT(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)> mouseMoveCallBack,lButtonDownCallBack,lButtonUpCallBack;




	std::mutex guiLock;
	YSBOOL drawingMasterSwitch; // If false, nothing other than gui widgets will be drawn.



	YsWString lastAccessedFileName;

	class PolyCreFileName *fileAndDirName;
	class PolyCreConfig *config;
	class PolyCreReferenceBitmapStore *refBmpStore;

	FsGui3DInterface threeDInterface;
	FsGui3DViewControlDialogBase *viewControlDlg;
	GeblGuiEditorMainMenu *mainMenu;
	FsGuiStatusBar *topBar,*bottomBar;
	YsWString modeMessage;
	class PolyCreSketchInterface *sketchUI;

	FsGuiPopUpMenu *fileMenu;

	FsGuiPopUpMenu *fileRecent;
	FsGuiRecentFiles recent;

	FsGuiPopUpMenuItem *viewDrawAxis;
	FsGuiPopUpMenuItem *viewDrawVertex;
	FsGuiPopUpMenuItem *viewDrawConstEdge;
	FsGuiPopUpMenuItem *viewDrawPolygonFill;
	FsGuiPopUpMenuItem *viewDrawWireframe;
	FsGuiPopUpMenuItem *viewDrawShrunkPolygon;
	FsGuiPopUpMenuItem *viewDrawBackFaceInDifferentColor;
	FsGuiPopUpMenuItem *viewEnableViewPort;
	FsGuiPopUpMenuItem *viewEnableCrossSection;
	FsGuiPopUpMenuItem *viewUseSameViewPortForAll;
	FsGuiPopUpMenuItem *viewShowHighlight;
	FsGuiPopUpMenuItem *viewHighlightNonManifoldEdge;
	FsGuiPopUpMenuItem *viewDrawBackgroundGradation;

	FsGuiPopUpMenuItem *viewDrawOtherShellWire;
	FsGuiPopUpMenuItem *viewDrawOtherShellFill;
	FsGuiPopUpMenuItem *viewDrawOtherShellNone;

	FsGuiPopUpMenuItem *viewDrawDnmConnection;

	FsGuiPopUpMenuItem *globalTargetCurrentShellOnly;
	FsGuiPopUpMenuItem *globalTargetAllShell;

	YsShellExtEdit_DuplicateUtil cutBuffer;

	YsVec3 preSelectedMovingDirection;


	// [Modeless dialogs]
	// Adding a modeless dialog:
	//   (1) Add a pointer in the following chunk.
	//   (2) Add an initialization in the constructor of the aplication.
	//   (3) Add deletion in the destructor of the application.
	//   (4) Add RemoveDialog in Edit_ClearUIIMode
	class PolyCreCrossSectionDialog *crossSectionDlg;
	class PolyCreSetFaceGroupNameDialog *faceNameDlg;
	class PolyCreSolidOfRevolutionDialog *solidOfRevolutionDlg;
	class PolyCreDnmTreeDialog *dnmTreeDlg;
	class PolyCreAngleSliderDialog *angleSliderDlg;
	class PolyCreEquilateralDialog *equilateralDlg;
	class PolyCreSlideShowDialog *slideShowDlg;
	class PolyCreParallelogramDialog *parallelogramDlg;
	class PolyCreInsertVertexDialog *insertVtxDlg;
	class PolyCreChopOffDialog *chopOffDlg;
	class PolyCreExtendOrShrinkDialog *extendOrShrinkDlg;
	class YsColorPaletteDialog *colorPaletteDlg;
	class FsGuiFileDialog *fdlg;
	FsGuiMessageBoxDialog *mbox;
	FsGuiNumberDialog *numberDlg;
	class YsCreateAirfoilDialog *createAirfoilDlg;
	class PolyCreRefBmpDialog *refBmpDlg;
	class PolyCreEditOffsetThickeningDialog *thickeningDlg;
	class PolyCreEditParallelSweepWithPathDialog *parallelSweepWithPathDlg;
	class PolyCreEditSweepConstEdgeDialog *sweepConstEdgeDlg;
	class PolyCreEditCreateMirrorImageDialog *mirrorImageDlg;
	class PolyCreHemmingDialog *hemmingDlg;
	class PolyCreRoundDialog *roundDlg;
	class PolyCreCreatePrimitiveDialog *createPrimitiveDlg;
	class PolyCreProjectionDialog *projectionDlg;
	class PolyCreBoolDialog *boolDlg;

	class Global_ScalingDialog;

	// [Modal dialogs]

	class YsShellExt_ThickeningInfo *thickeningInfo;

	YSBOOLEANOPERATION boolOpType;

	YSBOOL undoRedoRequireToClearUIMode;
	YSBOOL deletionRequireToClearUIMode;

	YsArray <ShortCutKey> shortCut;

	class GeblGuiExtensionBase *guiExtension;


	GeblGuiEditorBase();
	~GeblGuiEditorBase();
private:
	class DrawingCacheState
	{
	public:
		const YsShellDnmContainer <YsShellExtEditGui>::Node *slHd;
		YsShellExtEdit::SavePoint whenCached;
		void Save(const YsShellDnmContainer <YsShellExtEditGui>::Node *shl);
		YSBOOL IsModified(const YsShellDnmContainer <YsShellExtEditGui>::Node *shl) const;
	};
	class NonManifoldEdgeCache : public DrawingCacheState
	{
	public:
		YsGLVertexBuffer lineVtxBuf;
		YsGLColorBuffer lineColBuf;

		YsGLVertexBuffer pointVtxBuf; // Stupid OpenGL ES2.0 got rid of glPointSize.  All points need to be drawn as triangles.  What a waste.
		YsGLPixOffsetBuffer pointPixOffsetBuf;
		YsGLColorBuffer pointColBuf;

		void CacheNonManifoldEdge(const YsShellDnmContainer <YsShellExtEditGui>::Node *shl);
	};

	void AddViewControlDialog(void);
	void MakeMainMenu(void);
public:
	void SetMenuCheck(void);
	void SetDefaultViewOption(void);
private:
	void DeleteMainMenu(void);

public:
	void MessageDialog(const wchar_t title[],const wchar_t msg[],YSBOOL okBtnBottom=YSFALSE);
	void MessageDialog(const wchar_t title[],const wchar_t msg[],const wchar_t copyBtn[]);
	void YesNoDialog(
	    const wchar_t title[],const wchar_t msg[],const wchar_t yes[],const wchar_t no[],
	    void (*closeModalCallBackFunc)(FsGuiControlBase *,FsGuiDialog *,int));

	template <class objType>
	FsGuiDialog *YesNoDialog(
	    const wchar_t title[],const wchar_t msg[],const wchar_t yes[],const wchar_t no[],
	    void (objType::*closeModalCallBackFunc)(FsGuiDialog *,int),objType *receiver)
	{
		auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		mbox->Make(title,msg,yes,no,1,0);
		mbox->BindCloseModalCallBack(closeModalCallBackFunc,receiver);
		AttachModalDialog(mbox);
		return mbox;
	}



	virtual void OnInterval(void);
	virtual void OnSelectMenuItem(FsGuiPopUpMenuItem *menuItem);
	virtual YSRESULT OnLButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	virtual YSRESULT OnLButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	virtual YSRESULT OnRButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	virtual YSRESULT OnRButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	virtual YSRESULT OnMouseMove(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my,int prevMx,int prevMy);
	virtual YSRESULT OnTouchStateChange(const TouchState &touchState);

	virtual YSRESULT OnSaveView(const class FsGui3DViewControlDialog *dlg);
	void OnSaveView_FileSelected(FsGuiDialog *,int returnValue);
	void OnSaveView_ConfirmOverwriteCallBack(FsGuiDialog *,int returnValue);
	void OnSaveView_OverwriteChecked(FsGuiDialog *,int returnValue);

	virtual YSRESULT OnLoadView(class FsGui3DViewControlDialog *dlg);
	void OnLoadView_FileSelected(FsGuiDialog *,int returnValue);

	void OnModalDialogClosed(int dialogIdent,class FsGuiDialog *closedModalDialog,int modalDialogEndCode);


	YSRESULT (*ModeCleanUpCallBack)(GeblGuiEditorBase &canvas);
	YSRESULT (*SpaceKeyCallBack)(GeblGuiEditorBase &canvas);
	YSRESULT (*BackSpaceKeyCallBack)(GeblGuiEditorBase &canvas);
	YSRESULT (*LButtonCallBack)(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT (*MouseMoveCallBack)(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSBOOL lBtnDown;  // Shouldn't be turned on if lButtonDown is caught by other UI widgets.
	int lBtnDownMx,lBtnDownMy;
	int moveMx,moveMy;
	YSRESULT (*RButtonCallBack)(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT (*LButtonBoxCallBack)(GeblGuiEditorBase &canvas,int xMin,int yMin,int xMax,int yMax);

	YSRESULT OnKeyDown(int fskey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	void DrawGuiOnly(void);
	void Draw(void);
private:
	void DrawInGeometryEditMode(void);
	void DrawShellPlainFill(const YsShellDnmContainer <YsShellExtEditGui>::Node *);
	void DrawInDnmEditMode(void);
	void DrawInDnmEditModeRecursive(YsShellDnmContainer <YsShellExtEditGui>::Node *,const YsShellDnmContainer <YsShellExtEditGui>::DnmState &state,const YsMatrix4x4 &curTfm);
	void DrawDnmConnection(void);
	void DrawDnmConnectionRecursive(const YsVec3 &parentPos,YsShellDnmContainer <YsShellExtEditGui>::Node *,const YsMatrix4x4 &curTfm);

public:
	void RemakeDrawingBuffer(
	    YsShellDnmContainer <YsShellExtEditGui>::Node *slHd,
	    unsigned int needRemakeDrawingBuffer);
	void AddPolygonFragment(YsShellPolygonHandle plHd,const YsVec3 &nom,const YsShellVertexHandle triVtHd[3],YSBOOL flatShaded);

	// PickedSomething and BoxedSomething are moved to gui_foundation.

public:
	enum
	{
		NEED_REMAKE_DRAWING_NONE=0,
		NEED_REMAKE_DRAWING_ALL=~(unsigned int)0,
		NEED_REMAKE_DRAWING_VERTEX=1,
		NEED_REMAKE_DRAWING_SELECTED_VERTEX=2,
		NEED_REMAKE_DRAWING_POLYGON=4,
		NEED_REMAKE_DRAWING_SELECTED_POLYGON=8,
		NEED_REMAKE_DRAWING_CONSTEDGE=16,
		NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE=32,
		NEED_REMAKE_DRAWING_SELECTED_FACEGROUP=64,
	};
	unsigned int needRemakeDrawingBuffer;
	unsigned int needRemakeDrawingBuffer_for_OtherShell;
	YSBOOL drawAxis;
	YSBOOL drawVertex;
	YSBOOL drawPolygonEdge;
	YSBOOL drawPolygon;
	YSBOOL drawConstEdge;
	YSBOOL drawShrunkPolygon;
	YSBOOL drawOtherShell;
	YSBOOL drawBackFaceInDifferentColor;
	YSBOOL drawApplyViewPortToAllShell;
	YSBOOL drawHighlight;
	YSBOOL drawOtherShellWireframe,drawOtherShellFill;
	YSBOOL drawDnmConnection;

	YsPropertyFileIO propIO;

	YsColor backFaceColor;

	int dnmCurrentState; // -1:Static mode
	YSBOOL dnmEditMode;
	YsShellDnmContainer <YsShellExtEditGui>::DnmState dnmState;

private:
	YSBOOL drawNonManifoldEdge,actuallyDrawNonManifoldEdge;
	mutable NonManifoldEdgeCache nonManifoldEdgeCache;

	void (*UIBeforeDrawCallBack)(GeblGuiEditorBase &canvas);
	void (*UIDrawCallBack2D)(GeblGuiEditorBase &canvas);
	void (*UIDrawCallBack3D)(GeblGuiEditorBase &canvas);
	YsGLVertexBuffer UILineVtxBuf;

private:

public:
	void MakeInitialEmptyShell(void);
	SlhdAndErrorCode LoadGeneral(const wchar_t fn[],YsShellDnmContainer <YsShellExtEditGui>::Node *parent,YSBOOL updateRecentFiles);
	void SaveGeneral(const YsShellExtEdit &shl,const wchar_t fn[],YSBOOL updateRecentFiles);
	YsWString GetDataDir(void) const;
	void AddShortCut(int fskey,YSBOOL shift,YSBOOL ctrl,void (*callBackFunc)(void *,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *));


	void RefreshRecentlyUsedFileList(void);
	void AddRecentlyUsedFile(const wchar_t wfn[]);

	void SaveViewPreference(void);
	void LoadViewPreference(void);


	// Picking Aware of Dnm-Editing Mode >>
public:
	YsShellVertexHandle PickedVtHd(int mx,int my,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[]) const;
	template <const int N>
	YsShellVertexHandle PickedVtHd(int mx,int my,YsArray <YsShellVertexHandle,N> &excludeVtHd) const
	{
		YsMatrix4x4 modelTfm;
		if(YSTRUE==dnmEditMode && nullptr!=slHd)
		{
			modelTfm=dnmState.GetNodeToRootTransformation(slHd);
		}
		return GeblGuiFoundation::PickedVtHd(mx,my,excludeVtHd,modelTfm);
	}

	YsShellVertexHandle PickedVtHdAmongSelection(int mx,int my) const;	// Selected vertices are cached in the vertex buffer.  Can be calculated faster.

	YsArray <YsPair <YsShellVertexHandle,double> > BoxedVtHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellVertexHandle excludeVtHd[]) const;
	template <const int N>
	YsArray <YsPair <YsShellVertexHandle,double> > BoxedVtHd(int x0,int y0,int x1,int y1,const YsArray <YsShellVertexHandle,N> &excludeVtHd) const
	{
		YsMatrix4x4 modelTfm;
		if(YSTRUE==dnmEditMode && nullptr!=slHd)
		{
			modelTfm=dnmState.GetNodeToRootTransformation(slHd);
		}
		return GeblGuiFoundation::BoxedVtHd(x0,y0,x1,y1,excludeVtHd,modelTfm);
	}

	YsArray <YsPair <YsShellVertexHandle,double> > BoxedVtHdAmongSelection(int x0,int y0,int x1,int y1) const;

	YsShellPolygonHandle PickedPlHd(int mx,int my,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[]) const;
	template <const int N>
	YsShellPolygonHandle PickedPlHd(int mx,int my,YsArray <YsShellPolygonHandle,N> &excludePlHd) const
	{
		YsMatrix4x4 modelTfm;
		if(YSTRUE==dnmEditMode && nullptr!=slHd)
		{
			modelTfm=dnmState.GetNodeToRootTransformation(slHd);
		}
		return GeblGuiFoundation::PickedPlHd(mx,my,excludePlHd,modelTfm);
	}

	YsArray <YsPair <YsShellPolygonHandle,double> > EnclosedPlHd(const class PolyCreSketchInterface &sketchUI) const;

	YsShellPolygonHandle PickedPlHdAmongSelection(int mx,int my) const;

	YsArray <YsPair <YsShellPolygonHandle,double> > BoxedPlHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellPolygonHandle excludePlHd[]) const;
	template <const int N>
	YsArray <YsPair <YsShellPolygonHandle,double> > BoxedPlHd(int x0,int y0,int x1,int y1,YsArray <YsShellPolygonHandle,N> &excludePlHd) const
	{
		YsMatrix4x4 modelTfm;
		if(YSTRUE==dnmEditMode && nullptr!=slHd)
		{
			modelTfm=dnmState.GetNodeToRootTransformation(slHd);
		}
		return GeblGuiFoundation::BoxedPlHd(x0,y0,x1,y1,excludePlHd,modelTfm);
	}

	YsArray <YsPair <YsShellPolygonHandle,double> > BoxedPlHdAmongSelection(int x0,int y0,int x1,int y1) const;

	YsShellExt::ConstEdgeHandle PickedCeHd(int mx,int my,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[]) const;
	template <const int N>
	YsShellExt::ConstEdgeHandle PickedCeHd(int mx,int my,YsArray <YsShellExt::ConstEdgeHandle,N> &excludeCeHd) const
	{
		YsMatrix4x4 modelTfm;
		if(YSTRUE==dnmEditMode && nullptr!=slHd)
		{
			modelTfm=dnmState.GetNodeToRootTransformation(slHd);
		}
		return GeblGuiFoundation::PickedCeHd(mx,my,excludeCeHd,modelTfm);
	}

	YsShellExt::ConstEdgeHandle PickedCeHdAmongSelection(int mx,int my) const;

	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > BoxedCeHd(int x0,int y0,int x1,int y1,YSSIZE_T nExclude,const YsShellExt::ConstEdgeHandle excludeCeHd[]) const;
	template <const int N>
	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > BoxedCeHd(int x0,int y0,int x1,int y1,const YsArray <YsShellExt::ConstEdgeHandle,N> &excludeCeHd) const
	{
		YsMatrix4x4 modelTfm;
		if(YSTRUE==dnmEditMode && nullptr!=slHd)
		{
			modelTfm=dnmState.GetNodeToRootTransformation(slHd);
		}
		return GeblGuiFoundation::BoxedCeHd(x0,y0,x1,y1,excludeCeHd,modelTfm);
	}

	YsArray <YsPair <YsShellExt::ConstEdgeHandle,double> > BoxedCeHdAmongSelection(int x0,int y0,int x1,int y1) const;

	YsShellExt::FaceGroupHandle PickedFgHd(int mx,int my,int nExclude,const YsShellExt::FaceGroupHandle excludeFgHd[]) const;
	template <const int N>
	YsShellExt::FaceGroupHandle PickedFgHd(int mx,int my,YsArray <YsShellExt::FaceGroupHandle,N> &excludeFgHd) const
	{
		YsMatrix4x4 modelTfm;
		if(YSTRUE==dnmEditMode && nullptr!=slHd)
		{
			modelTfm=dnmState.GetNodeToRootTransformation(slHd);
		}
		return GeblGuiFoundation::PickedFgHd(mx,my,excludeFgHd,modelTfm);
	}

	YsShellExt::FaceGroupHandle PickedFgHdAmongSelection(int mx,int my) const;

	const YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[]) const;
	template <const int N>
	const YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> &excludeSlHd) const
	{
		if(YSTRUE==dnmEditMode)
		{
			return GeblGuiFoundation::PickedSlHd<N>(mx,my,excludeSlHd,&dnmState);
		}
		else
		{
			return GeblGuiFoundation::PickedSlHd<N>(mx,my,excludeSlHd,nullptr);
		}
	}

	YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YSSIZE_T nExclude,const YsShellDnmContainer <YsShellExtEditGui>::Node *excludeSlHd[]);
	template <const int N>
	YsShellDnmContainer <YsShellExtEditGui>::Node *PickedSlHd(int mx,int my,YsArray <const YsShellDnmContainer <YsShellExtEditGui>::Node *> &excludeSlHd)
	{
		if(YSTRUE==dnmEditMode)
		{
			return GeblGuiFoundation::PickedSlHd<N>(mx,my,excludeSlHd,&dnmState);
		}
		else
		{
			return GeblGuiFoundation::PickedSlHd<N>(mx,my,excludeSlHd,nullptr);
		}
	}
	// Picking Aware of Dnm-Editing Mode <<


	// Menu Call Back Functions >>
	void File_New(FsGuiPopUpMenuItem *);

	void File_Open(FsGuiPopUpMenuItem *);
	void File_Open_FileSelected(FsGuiDialog *,int);
	void File_OpenDnm_ConfirmCloseCallBack(FsGuiDialog *,int);
	void File_Open_AfterOpenFile(YsShellDnmContainer <YsShellExtEditGui>::Node *newSlHd,const wchar_t fn[],YSBOOL recordRecentFiles);

	void File_Save(FsGuiPopUpMenuItem *);

	void File_SaveAs(FsGuiPopUpMenuItem *);
	void File_SaveAs_FileSelected(FsGuiDialog *,int);
	void File_SaveAs_STLChecked(FsGuiDialog *,int returnValue);
	void File_SaveAs_CheckOverwrite(void);
	void File_SaveAs_ConfirmOverwriteCallBack(FsGuiDialog *,int returnValue);
	void File_SaveAs_PassedOverWriteCheck(const wchar_t fn[]);

	void File_Close(FsGuiPopUpMenuItem *);
	void File_Close_ConfirmCloseCallBack(FsGuiDialog *,int returnValue);
	void File_Close_Confirmed(void);

	void File_Exit(FsGuiPopUpMenuItem *);
	void File_Exit_ConfirmExitCallBack(FsGuiDialog *,int returnValue);

	static void File_SlideShow(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	static void File_SlideShow_FileSelected(FsGuiControlBase *canvas,FsGuiDialog *,int);
	static YSRESULT File_SlideShow_ModeCleanUpCallBack(GeblGuiEditorBase &canvas);

	static void File_GetInfo_Statistics(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	static void File_GetInfo_Volume(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	static void File_GetInfo_Length_BetweenVertex(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	static void File_GetInfo_Length_ConstEdge(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	static void File_GetInfo_Area_SelectedPolygon(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	static void File_GetInfo_Area_All(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	static void File_GetInfo_Position_SelectedVertex(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);
	void File_GetInfo_DebugInfo_Selection(FsGuiPopUpMenuItem *);
	void File_GetInfo_DebugInfo_Selection_AskedCopyToClipboard(FsGuiDialog *dlg,int returnCode);
	void File_GetInfo_DebugInfo_Selection_ShowOnDialog(void);
	YsWString File_GetInfo_DebugInfo_Selection_MakeMessage(YSBOOL first20) const;

	void File_GetInfo_MaxNormalDeviationAcrossConstEdge(FsGuiPopUpMenuItem *);
	void File_GetInfo_AngleBetweenSelectedPolygon(FsGuiPopUpMenuItem *);
	void File_GetInfo_DihedralAngleAcrossSelectedEdge(FsGuiPopUpMenuItem *);
	void File_GetInfo_MinMaxAvgDihedralAngle(FsGuiPopUpMenuItem *itm);

	static void File_Recent(void *appPtr,FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *);


private:
	YSBOOL nextESCOnlyCancelDnmEditSubMode;
public:
	void Edit_ESCKey(FsGuiPopUpMenuItem *);

	void Edit_ClearUIMode(FsGuiPopUpMenuItem *);
	void Edit_ClearUIMode(void);
	void Edit_ClearUIModeExceptDNMEditMode(void);

	void Edit_Undo(FsGuiPopUpMenuItem *);
	void Edit_Redo(FsGuiPopUpMenuItem *);

	void Edit_Copy(FsGuiPopUpMenuItem *);
	void Edit_Paste(FsGuiPopUpMenuItem *);
	void Edit_PasteSpecial(FsGuiPopUpMenuItem *);
	class Edit_PasteWithTransformationDialog;
	void Edit_PasteWithTransformation(FsGuiPopUpMenuItem *);
	void Edit_PasteWithTransformation_PointMoved(FsGui3DInterface *itfc);
	void Edit_PasteWithTransformation_Draw3DCallBack(void);
	void Edit_PasteWithTransformation_Paste(void);

	void Edit_DeleteSelection(FsGuiPopUpMenuItem *);
	void Edit_DeleteSelection(void);

	void Edit_ForceDeleteSelection(FsGuiPopUpMenuItem *);

	void Edit_InsertVertex(FsGuiPopUpMenuItem *);
	static YSRESULT Edit_InsertVertex_SpaceKeyCallBack(GeblGuiEditorBase &canvas);

	void Edit_InsertPolygon(FsGuiPopUpMenuItem *);

	void Edit_InsertConstEdge(FsGuiPopUpMenuItem *);

	void Edit_InsertFaceGroup(FsGuiPopUpMenuItem *);

	void Edit_InsertVertexAtTwoLineIntersection(FsGuiPopUpMenuItem *);

	void Edit_SimpleStitch(FsGuiPopUpMenuItem *);

	void Edit_SmoothShadeSelectedVertex(FsGuiPopUpMenuItem *);
	void Edit_FlatShadeSelectedVertex(FsGuiPopUpMenuItem *);
	void Edit_ShadeSelectedPolygon(FsGuiPopUpMenuItem *);
	void Edit_SelfLightSelectedPolygon(FsGuiPopUpMenuItem *);
	void Edit_SetPolygonAlpha(FsGuiPopUpMenuItem *);
	void Edit_DivideSmoothShadingRegionOnSelectedConstEdge(FsGuiPopUpMenuItem *);
	void Edit_UndivideSmoothShadingRegionOnSelectedConstEdge(FsGuiPopUpMenuItem *);
	void Edit_SetRenderAsLight(FsGuiPopUpMenuItem *);
	void Edit_DontRenderAsLight(FsGuiPopUpMenuItem *);
	void Edit_ZeroNormalSelectedPolygon(FsGuiPopUpMenuItem *);

	void Edit_Paint_ByPickingPolygon(FsGuiPopUpMenuItem *);
	YSRESULT Select_Paint_ByPickingPolygon_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos);

	static void Edit_Paint_ByPickingFaceGroup(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	void Edit_Paint_SelectedPolygon(FsGuiPopUpMenuItem *);

	void Edit_Paint_All(FsGuiPopUpMenuItem *);

	void Edit_Paint_FloodFill_SameColor(FsGuiPopUpMenuItem *);
	YSRESULT Edit_Paint_FloodFill_SameColor_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos);

	void Edit_Paint_FloodFill_BoundedByConstEdge(FsGuiPopUpMenuItem *);
	YSRESULT Edit_Paint_FloodFill_BoundedByConstEdge_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos);

	void Edit_Paint_PickUpColor(FsGuiPopUpMenuItem *);
	YSRESULT Select_Paint_PickUpColor_LButtonCallBack(FsGuiMouseButtonSet btn,YsVec2i pos);


	void Edit_CreatePrimitiveShape(FsGuiPopUpMenuItem *);
	static YSRESULT Edit_CreatePrimitive_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	static void Edit_CreatePrimitive_CursorMoved(void *appPtr,FsGui3DInterface &);
	static void Edit_CreatePrimitive_UIDrawCallBack3D(GeblGuiEditorBase &canvas);


	void Edit_CreateEquilateral(FsGuiPopUpMenuItem *);
	void Edit_CreateEquilateral_DrawCallBack3D(void);
	void Edit_CreateEquilateral_3DInterfaceCallBack(class FsGui3DInterface *itfc);
	void Edit_CreateEquilateral_SpaceKeyCallBack(void);


	class Edit_CreateEllipseFromSelectedVertexDialog;
	void Edit_CreateEllipseFromSelectedVertex(FsGuiPopUpMenuItem *);
	void Edit_CreateEllipseFromSelectedVertex_Draw3D(void);
	void Edit_CreateEllipseFromSelectedVertex_Create(void);


	void Edit_CreateAirfoil(FsGuiPopUpMenuItem *);
	static void Edit_CreateAirfoil_DrawCallBack3D(GeblGuiEditorBase &canvas);
	void Edit_CreateAirfoil_DrawCallBack3D(void);
	static void Edit_CreateAirfoil_PivotMoved(void *appPtr,FsGui3DInterface &itfc);
	void Edit_CreateAirfoil_Create(void);

	void Edit_CreateMirrorImage(FsGuiPopUpMenuItem *);
	void Edit_CreateMirrorImage_Create(void);

	void Edit_CreateParallelogramFromTwoVertex(FsGuiPopUpMenuItem *);
	static void Edit_CreateParallelogramFromTwoVertex_DrawCallBack3D(GeblGuiEditorBase &canvas);
	static void Edit_CreateParallelogramFromTwoVertex_3DInterfaceCallBack(void *appPtr,class FsGui3DInterface &itfc);
	static YSRESULT Edit_CreateParallelogramFromTwoVertex_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_CreateParallelogramFromTwoVertex_SpaceKeyCallBack(void);

	void Edit_CreateParallelogramFromThreeVertex(FsGuiPopUpMenuItem *);

	void Edit_CreateSolidOfRevolution(FsGuiPopUpMenuItem *);
	static void Edit_CreateSolidOfRevolution_DrawCallBack3D(GeblGuiEditorBase &canvas);
	static void Edit_CreateSolidOfRevolution_3DInterfaceCallBack(void *appPtr,class FsGui3DInterface &itfc);
	static YSRESULT Edit_CreateSolidOfRevolution_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_CreateSolidOfRevolution_SpaceKeyCallBack(void);

	void Edit_CreateConeFromPolygonAndVertex(FsGuiPopUpMenuItem *);

	class Edit_Create_BezierCurveDialog;
	void Edit_Create_BezierCurve(FsGuiPopUpMenuItem *);
	void Edit_Create_BezierCurve_Draw3D(void);
	void Edit_Create_BezierCurve_Create(void);

	void Edit_SewBetweenTwoVertex(FsGuiPopUpMenuItem *);


	void Edit_Move_SelectDirectionFromNormal(FsGuiPopUpMenuItem *);
	void Edit_Move_SelectVectorFromTwoVertex(FsGuiPopUpMenuItem *);
	void Edit_Move_SelectionInPreSelectedDirection(FsGuiPopUpMenuItem *);

	void Edit_Move_SelectionInPreSelectedDirection_SpecificDistance(FsGuiPopUpMenuItem *);
	void Edit_Move_SelectionInPreSelectedDirection_SpecificDistance_CloseModalCallBack(FsGuiDialog *dlg,int returnCode);

	void Edit_Move_SelectionInPreSelectedVector(FsGuiPopUpMenuItem *);


	void Edit_MoveSelection(FsGuiPopUpMenuItem *);

	void Edit_StartMoveSelection(const YSBOOL dirSwitch[3],const YsVec3 dir[3]);
	static void Edit_MoveSelection_DrawCallBack3D(GeblGuiEditorBase &canvas);
	void Edit_MoveSelection_CursorMoved(FsGui3DInterface *itfc);
	static YSRESULT Edit_MoveSelection_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	class Edit_ScaleSelectionDialog;
	void Edit_ScaleSelection(FsGuiPopUpMenuItem *);


	void Edit_MoveVertexToThreePlaneIntersection(FsGuiPopUpMenuItem *);



	void Edit_RotateSelection(FsGuiPopUpMenuItem *);
	static void Edit_RotateSelection_DrawCallBack3D(GeblGuiEditorBase &canvas);
	void Edit_RotateSelection_CursorMoved(FsGui3DInterface *);
	static YSRESULT Edit_RotateSelection_SpaceKeyCallBack(GeblGuiEditorBase &canvas);

	void Edit_SwapEdge(FsGuiPopUpMenuItem *);

	void Edit_MergePolygon(FsGuiPopUpMenuItem *);

	void Edit_SplitPolygon(FsGuiPopUpMenuItem *);

	void Edit_VertexCollapseMidPoint(FsGuiPopUpMenuItem *);
	void Edit_VertexCollapseAtoB(FsGuiPopUpMenuItem *);
	void Edit_VertexCollapse(YsShellVertexHandle vtHdToDel,YsShellVertexHandle vtHdToSurvive,const YsVec3 &newPos);

	void Edit_Hemming_AroundSelectedPolygon(FsGuiPopUpMenuItem *);
	static void Edit_Hemming_AroundSelectedPolygon_UIDrawCallBack3D(GeblGuiEditorBase &canvas);

	void Edit_Hemming_AroundOutsideOfSelectedPolygon(FsGuiPopUpMenuItem *);

	void Edit_Hemming_OneSideOfVertexSequence(FsGuiPopUpMenuItem *);

	void Edit_Hemming_AroundSelectedPolygon(void);

	static void Edit_Intersection_CutByPlane(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Edit_Intersection_ChopOff(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void Edit_Intersection_ChopOff_DrawCallBack3D(GeblGuiEditorBase &canvas);
	static void Edit_Intersection_ChopOff_3DInterfaceCallBack(void *appPtr,class FsGui3DInterface &itfc);
	static YSRESULT Edit_Intersection_ChopOff_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Intersection_ChopOff_SpaceKeyCallBack(void);

	void Edit_Sweep_ParallelSweep(FsGuiPopUpMenuItem *);
	void Edit_Sweep_ParallelSweep_PointMoved(FsGui3DInterface *itfc);
	static void Edit_Sweep_ParallelSweep_DrawCallBack3D(GeblGuiEditorBase &canvas);
	void Edit_Sweep_ParallelSweep_DrawCallBack3D(void);
	static YSRESULT Edit_Sweep_ParallelSweep_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Sweep_ParallelSweep_SpaceKeyCallBack(void);

	static void Edit_Sweep_ParallelSweepWithPath(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void Edit_Sweep_ParallelSweepWithPath_DrawCallBack3D(GeblGuiEditorBase &canvas);
	void Edit_Sweep_ParallelSweepWithPath_DrawCallBack3D(void);
	static YSRESULT Edit_Sweep_ParallelSweepWithPath_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Sweep_ParallelSweepWithPath_SpaceKeyCallBack(void);

	static void Edit_Sweep_AlongSweepPath(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);


	void Edit_Align_MinimumX(FsGuiPopUpMenuItem *);
	void Edit_Align_CenterX(FsGuiPopUpMenuItem *);
	void Edit_Align_MaximumX(FsGuiPopUpMenuItem *);
	void Edit_Align_MinimumY(FsGuiPopUpMenuItem *);
	void Edit_Align_CenterY(FsGuiPopUpMenuItem *);
	void Edit_Align_MaximumY(FsGuiPopUpMenuItem *);
	void Edit_Align_MinimumZ(FsGuiPopUpMenuItem *);
	void Edit_Align_CenterZ(FsGuiPopUpMenuItem *);
	void Edit_Align_MaximumZ(FsGuiPopUpMenuItem *);
	void Edit_Align(int axis,YsShellExt_AlignmentUtil::ALIGNTYPE alignType);

	void Edit_Sweep_BetweenConstEdge(FsGuiPopUpMenuItem *);
	static void Edit_Sweep_BetweenConstEdge_DrawCallBack3D(GeblGuiEditorBase &canvas);
	static YSRESULT Edit_Sweep_BetweenConstEdge_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Sweep_BetweenConstEdge_Sweep(void);

	class Edit_Sweep_RotationalWithPathDialog;
	void Edit_Sweep_RotationalWithPath(FsGuiPopUpMenuItem *);
	void Edit_Sweep_RotationalWithPath_Draw3DCallBack(void);
	void Edit_Sweep_RotationalWithPath_Sweep(void);

	void Edit_Sweep_ExtendOrShrink(FsGuiPopUpMenuItem *);
	static void Edit_Sweep_ExtendOrShrink_DrawCallBack3D(GeblGuiEditorBase &canvas);
	void Edit_Sweep_ExtendOrShrink_3DInterfaceCallBack(class FsGui3DInterface *itfc);
	static YSRESULT Edit_Sweep_ExtendOrShrink_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Sweep_ExtendOrShrink_SpaceKeyCallBack(void);

	static void Edit_Offset_Thickening(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void Edit_Offset_Thickening_DrawCallBack3D(GeblGuiEditorBase &canvas);
	static YSRESULT Edit_Offset_Thickening_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Offset_Thickening_SpaceKeyCallBack(void);

	static void Edit_Round_PolygonOrConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	void Edit_Round_PolygonOrConstEdge(void);

	static void Edit_Round_PolygonOrConstEdgeSelectedCorner(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	void Edit_Round_PolygonOrConstEdgeSelectedCorner(void);

	static void Edit_Round_DrawCallBack2d(GeblGuiEditorBase &canvas);
	void Edit_Round_DrawCallBack2d(void);

	YSRESULT Edit_Round2d(void);

	static void Edit_Round_AroundVertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	void Edit_Round_AroundVertex(const double radius,const int nDiv);

	static void Edit_Round_AroundSelectedPolygon(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Edit_Round_OpenVertexSequence(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Edit_Round_DrawCallBack3d(GeblGuiEditorBase &canvas);
	void Edit_Round_DrawCallBack3d(void);
	static YSRESULT Edit_Round_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Round_SpaceKeyCallBack(void);

	YSRESULT Edit_Round3d(void);

	void Edit_UnroundVertexSequence(FsGuiPopUpMenuItem *);


	static void Edit_Projection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void Edit_Projection_DrawCallBack3d(GeblGuiEditorBase &canvas);
	static YSRESULT Edit_Projection_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT Edit_Projection_Project(void);

	void Edit_ImprintSelection(FsGuiPopUpMenuItem *);

	static void Edit_TriangulateSelection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Edit_InvertConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Edit_SplitConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Edit_MergeConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	void Edit_InvertSelectedPolygon(FsGuiPopUpMenuItem *);



	static void BoolUnion(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void BoolSubtraction(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void BoolIntersection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void BoolMerge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void BoolSeparate(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void BoolOpenSurfaceMinusSolid(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void BoolOpenSurfacePlusSolid(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void BoolOpenSurfaceAndSolid(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static YSRESULT Bool_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	void Bool_DiagnoseDialog(void);
	static void Bool_DiagnoseDialogCallBack(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue);
	YSRESULT Bool_Apply(YSBOOLEANOPERATION boolOpType,YsShellExtEdit &shlA,YsShellExtEdit &shlB);



	void Select_All(FsGuiPopUpMenuItem *);
	void Select_UnselectAll(FsGuiPopUpMenuItem *);

	void Select_PickVertex(FsGuiPopUpMenuItem *);
	static YSRESULT Select_PickVertex_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	static YSRESULT Select_PickVertex_LButtonBoxCallBack(GeblGuiEditorBase &canvas,int xMin,int yMin,int xMax,int yMax);
	static YSRESULT Select_PickVertex_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas);

	void Select_PickPolygon(FsGuiPopUpMenuItem *);
	static YSRESULT Select_PickPolygon_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	static YSRESULT Select_PickPolygon_LButtonBoxCallBack(GeblGuiEditorBase &canvas,int xMin,int yMin,int xMax,int yMax);
	static YSRESULT Select_PickPolygon_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas);

	void Select_PickConstEdge(FsGuiPopUpMenuItem *);
	static YSRESULT Select_PickConstEdge_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	static YSRESULT Select_PickConstEdge_LButtonBoxCallBack(GeblGuiEditorBase &canvas,int xMin,int yMin,int xMax,int yMax);
	static YSRESULT Select_PickConstEdge_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas);

	void Select_PickFaceGroup(FsGuiPopUpMenuItem *);
	static YSRESULT Select_PickFaceGroup_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	static YSRESULT Select_PickFaceGroup_BackSpaceKeyCallBack(GeblGuiEditorBase &canvas);


	void Select_UnselectVertex(FsGuiPopUpMenuItem *);

	void Select_AllVertex(FsGuiPopUpMenuItem *);

	void Select_InvertVertex(FsGuiPopUpMenuItem *);

	void Select_VertexOfPolygon(FsGuiPopUpMenuItem *);

	void Select_VertexOfConstEdge(FsGuiPopUpMenuItem *);

	void Select_VertexAlongBoundaryOrConstEdge(FsGuiPopUpMenuItem *);

	void Select_Vertex_UsedByTwoConstEdge(FsGuiPopUpMenuItem *);

	void Select_Vertex_SingleUseEdgeLoop(FsGuiPopUpMenuItem *);

	void Select_EdgeConnectedVertexBetweenSelection(FsGuiPopUpMenuItem *);

	void Select_Vertex_OneNeighborOfSelection(FsGuiPopUpMenuItem *);

	void Select_UnselectPolygon(FsGuiPopUpMenuItem *);

	void Select_AllPolygon(FsGuiPopUpMenuItem *);

	void Select_InvertPolygon(FsGuiPopUpMenuItem *);

	void Select_PolygonOfFaceGroup(FsGuiPopUpMenuItem *);

	void Select_PolygonConnected(FsGuiPopUpMenuItem *);

	void Select_PolygonFromEdge(FsGuiPopUpMenuItem *);

	void Select_PolygonUsingAtLeastOneOfSelectedVertex(FsGuiPopUpMenuItem *);

	void Select_PolygonShortestPath(FsGuiPopUpMenuItem *);

	void Select_PolygonSameColorFromCurrentSelection(FsGuiPopUpMenuItem *);

	void Select_Polygon_Narrow(FsGuiPopUpMenuItem *);
	void Select_Polygon_Narrow_ThresholdEntered(FsGuiDialog *,int returnCode);

	void Select_Polygon_CannotCalculateNormal(FsGuiPopUpMenuItem *);

	void Select_AllFaceGroup(FsGuiPopUpMenuItem *);

	void Select_InvertFaceGroup(FsGuiPopUpMenuItem *);


	void Select_FaceGroup_PlanarFaceGroup(FsGuiPopUpMenuItem *);
	void Select_FaceGroupWithHighDihedralAngle(FsGuiPopUpMenuItem *);
	void Select_FaceGroupWithHighDihedralAngle_AngleSelected(FsGuiDialog *dlg,int returnCode);
	void Select_FaceGroup_Connected(FsGuiPopUpMenuItem *);

	void Select_ConstEdge_All(FsGuiPopUpMenuItem *);
	void Select_ConstEdge_InvertSelection(FsGuiPopUpMenuItem *);
	void Select_ConstEdge_DividingSmoothShadingRegion(FsGuiPopUpMenuItem *);
	void Select_ConstEdge_FromPolygon(FsGuiPopUpMenuItem *);

	void Select_Shell_ByDialog(FsGuiPopUpMenuItem *);


	void Select_CurrentShellByPicking(FsGuiPopUpMenuItem *);
	static YSRESULT Select_CurrentShellByPicking_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	void Select_NextShell(FsGuiPopUpMenuItem *);

	void Select_PrevShell(FsGuiPopUpMenuItem *);

	void Select_ByProperty(FsGuiPopUpMenuItem *);
	void Select_VertexByHashKey(FsGuiPopUpMenuItem *);
	void Select_VertexByIndex(FsGuiPopUpMenuItem *);
	void Select_PolygonByHashKey(FsGuiPopUpMenuItem *);
	void Select_PolygonByIndex(FsGuiPopUpMenuItem *);
	void Select_FaceGroupByIdNumber(FsGuiPopUpMenuItem *);
	void Select_FaceGroupByLabel(FsGuiPopUpMenuItem *);
	void Select_ConstEdgeByIdNumber(FsGuiPopUpMenuItem *);
	void Select_ConstEdgeByLabel(FsGuiPopUpMenuItem *);
	class PolyCreSelectByPropertyDialog *PrepareSelectByPropDialog(void);
	void SelectTangledPolygon(FsGuiPopUpMenuItem *);
	void SelectNonTriangularPolygon(FsGuiPopUpMenuItem *);

	void SelectZeroNormalPolygon(FsGuiPopUpMenuItem *);

	void Select_PolygonByStroke(FsGuiPopUpMenuItem *);
	void Select_PolygonByStroke_DrawCallBack2D(void);
	YSRESULT Select_PolygonByStroke_LButtonDownCallBack(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT Select_PolygonByStroke_LButtonUpCallBack(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT Select_PolygonByStroke_MouseMoveCallBack(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);



	void View_RefreshBuffer(FsGuiPopUpMenuItem *);

	void View_ResetViewRange(FsGuiPopUpMenuItem *);

	void View_ResetViewOption(FsGuiPopUpMenuItem *);

	void View_DrawAxis(FsGuiPopUpMenuItem *);

	void View_DrawVertex(FsGuiPopUpMenuItem *);

	void View_DrawConstEdge(FsGuiPopUpMenuItem *);

	void View_DrawPolygonFill(FsGuiPopUpMenuItem *);

	void View_DrawWireframe(FsGuiPopUpMenuItem *);

	void View_DrawShrunkPolygon(FsGuiPopUpMenuItem *);

	void View_DrawBackFaceInDifferentColor(FsGuiPopUpMenuItem *);

	void View_DrawOtherShellWireframe(FsGuiPopUpMenuItem *);
	void View_DrawOtherShellFill(FsGuiPopUpMenuItem *);
	void View_DrawOtherShellNone(FsGuiPopUpMenuItem *);

	void View_DrawDnmConnection(FsGuiPopUpMenuItem *);

	static void View_FocusOn_SelectionCenterOfGravity(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_FocusOn_SelectionBoundingBoxCenter(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_FocusOn_Vertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_FocusOn_Vertex_RKey(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static YSRESULT View_FocusOn_Vertex_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT View_FocusOn_Vertex_Click(int mx,int my);

	static void View_FocusOn_Polygon(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_FocusOn_Polygon_ShiftRKey(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static YSRESULT View_FocusOn_Polygon_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	static void View_FocusOn_PolygonStraight(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_FocusOn_Polygon_ShiftCtrlRKey(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static YSRESULT View_FocusOn_PolygonStraight_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	YSRESULT View_FocusOn_Polygon_Click(int mx,int my,YSBOOL alsoChangeOrientation);

	void View_ApplyViewPortToAllShell(void);

	static void View_UseSameViewPortForAllShell(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_EnableViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_ResetViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_MoveViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_MoveViewPort_BoxMoved(void *appPtr,FsGui3DInterface &itfc);

	static void View_MoveViewPortToRotationCenter(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_ExpandViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_ShrinkViewPort(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_ViewPortFromSelection(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_CrossSection_Enable(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void View_CrossSection_Reset(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_CrossSection_Move(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_CrossSection_Invert(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);


	static void View_OpenRefBmpDialog(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_OpenRefBmpDialog_UIDrawCallBack3D(GeblGuiEditorBase &canvas);

	static void View_LoadRefBmpConfig(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_LoadRefBmpConfig_FileSelected(FsGuiControlBase *canvas,FsGuiDialog *,int);

	static void View_SaveRefBmpConfig(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_SaveRefBmpConfig_FileSelected(FsGuiControlBase *canvas,FsGuiDialog *,int);
	static void View_SaveRefBmpConfig_PassedOverwriteCheck(FsGuiControlBase *canvas,FsGuiDialog *,int);
	void View_SaveRefBmpConfig_PassedOverwriteCheck(const wchar_t fn[]);

	static void View_ShowAllRefBmp(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_HideAllRefBmp(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	void View_Highlight_ShowHighlight(FsGuiPopUpMenuItem *);
	static void View_HighlightNonManifoldEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void View_HighlightSelfIntersectingPolygon(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	void View_Highlight_HighAspectRatioTriangleAndQuadrilateral(FsGuiPopUpMenuItem *);
	void View_Highlight_HighAspectRatioTriangleAndQuadrilateral_RefreshCallBack(FsGuiDialog *dlgPtr);

	void View_Highlight_HighDihedralAngleEdgeWithinFaceGroup(FsGuiPopUpMenuItem *);

	void View_Highlight_InconsistentOrientationAcrossEdge(FsGuiPopUpMenuItem *);

	void View_DrawBackgroundGradation(FsGuiPopUpMenuItem *itm);


	YsArray <YsShellDnmContainer <YsShellExtEditGui>::Node *> Global_GetTarget(void);

	void Global_Target_Current(FsGuiPopUpMenuItem *);
	void Global_Target_All(FsGuiPopUpMenuItem *);

	void Global_DeleteUnusedVertexAll(FsGuiPopUpMenuItem *);

	static void Global_InvertPolygonAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Global_SetZeroNormalAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Global_FixOrientationAndNormalClosedVolume(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	void Global_FixOrientationBasedOnAssignedNormal(FsGuiPopUpMenuItem *);

	void Global_FixNormalBasedOnOrientation(FsGuiPopUpMenuItem *);

	static void Global_TriangulateAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void Global_DeleteRedundantVertex(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void Global_DeleteRedundantVertex_ToleranceEntered(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue);

	static void Global_DeleteRedundantVertexNonManifoldEdgeOnly(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void Global_DeleteRedundantVertexNonManifoldEdgeOnly_ToleranceEntered(FsGuiControlBase *appPtr,FsGuiDialog *,int returnValue);

	void Global_DeleteRedundantVertex(const double tol,const YsShellVertexStore *limitedVtx);

	void Global_Scaling(FsGuiPopUpMenuItem *);
	void Global_Scaling_DialogCallBack(double sx,double sy,double sz);

	void Global_Scale_InDirection(FsGuiPopUpMenuItem *);
	void Global_Scale_InDirection_DialogCallBack(FsGuiDialog *dlg,int returnCode);


	static void Sketch_CreateConstEdgeBySketch(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void Sketch_DrawCallBack2D(GeblGuiEditorBase &canvas);
	static YSRESULT Sketch_CreateConstEdge_LButtonCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	static YSRESULT Sketch_CreateConstEdge_MouseMoveCallBack(GeblGuiEditorBase &canvas,YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);



	static void UtilMenu_ClearConstEdgeAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	class SetConstEdgeNameDialog;
	void UtilMenu_SetConstEdgeName(FsGuiPopUpMenuItem *);

	static void UtilMenu_AddConstEdgeByDihedralAngle(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void UtilMenu_AddConstEdgeByDihedralAngle_DrawCallBack3D(GeblGuiEditorBase &canvas);
	static YSRESULT UtilMenu_AddConstEdgeByDihedralAngle_SpaceKeyCallBack(GeblGuiEditorBase &canvas);
	YSRESULT UtilMenu_AddConstEdgeByDihedralAngle_SpaceKeyCallBack(void);

	static void UtilMenu_AddConstEdgeAlongNonManifoldEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	static void UtilMenu_AddConstEdgeOnFaceGroupBoundary(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	void UtilMenu_AddConstEdgeAlongColorBoundary(FsGuiPopUpMenuItem *);

	static void UtilMenu_OpenCloseConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	void UtilMenu_ReconsiderConstEdge(FsGuiPopUpMenuItem *);

	static void UtilMenu_ClearFaceGroupAll(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void UtilMenu_ConstrainAllEdges(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	static void UtilMenu_MakeFaceGroupFromConstEdge(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);
	void UtilMenu_PaintBasedOnFaceGroup(FsGuiPopUpMenuItem *);

	static void UtilMenu_FaceGroup_SetName(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *);

	void UtilMenu_ExpandCurrentFaceGroupToUnassignedPolygon(FsGuiPopUpMenuItem *);



	void RepairMenu_FreeStitching(FsGuiPopUpMenuItem *);

	void RepairMenu_LoopStitching(FsGuiPopUpMenuItem *);

	void RepairMenu_RemoveIdenticalPolygon(FsGuiPopUpMenuItem *);


	void Dnm_DnmEditMode(FsGuiPopUpMenuItem *);
	void Dnm_EditTreeStructure(FsGuiPopUpMenuItem *);
	class Dnm_SetDnmNameDialog;
	void Dnm_SetDnmName(FsGuiPopUpMenuItem *);
	void Dnm_SetDnmName_CloseModalCallBack(FsGuiDialog *closedModalDialog,int returnCode);
	void Dnm_LoadStaticMode(void);
	void Dnm_LoadState0(void);
	void Dnm_LoadState1(void);
	void Dnm_LoadState2(void);
	void Dnm_LoadState3(void);
	void Dnm_Preview(void);
	void Dnm_SetPivot(FsGuiPopUpMenuItem *);
	void Dnm_SetPivot_SpaceKeyCallBack(void);
	void Dnm_SetRotationAxis(FsGuiPopUpMenuItem *);
	class Dnm_TransformationDialog;
	void Dnm_Rotate(FsGuiPopUpMenuItem *);
	void Dnm_Rotate_RotationChanged(FsGui3DInterface *);
	void Dnm_Translate(FsGuiPopUpMenuItem *);
	void Dnm_Translate_TranslationChanged(FsGui3DInterface *);

	void Dnm_Transformation_OK(void);
	void Dnm_Transformation_Capture(YsShellDnmContainer <YsShellExtEditGui>::Node *slHd,int dnmStateIndex);
	void Dnm_Transformation_Cancel(void);

	void Dnm_Show(FsGuiPopUpMenuItem *);
	void Dnm_Hide(FsGuiPopUpMenuItem *);

	void Dnm_AutoSetState(FsGuiPopUpMenuItem *);




	void Option_Config(FsGuiPopUpMenuItem *);
	void Option_ReapplyConfig(class PolyCreConfig &cfg);
	void Option_ReapplyConfig(void);



	void Help_About(FsGuiPopUpMenuItem *);
	void Help_Help(FsGuiPopUpMenuItem *);
	// Menu Call Back Functions <<
};

/* } */
#endif
