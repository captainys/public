/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp.h
Copyright (c) 2015 Soji Yamakawa.  All rights reserved.
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

#ifndef FSGUI_3DAPP_IS_INCLUDED
#define FSGUI_3DAPP_IS_INCLUDED
/* { */

#include <functional>

#include <ystexturemanager.h>
#include <fsgui3d.h>

#include <retromap_data.h>
#include <retromap_pngexport.h>

#include "filename.h"
#include "config/retromap_config.h"

class FsGui3DMainCanvas : public FsGuiCanvas
{
public:
	/*! 3D Interface for point-coordinate input, box input, rotation, etc.
	*/
	FsGui3DInterface threeDInterface;

	/*! 3D drawing environment that manages projection and view transformations.
	*/
	FsGui3DViewControl drawEnv;

	/*! View-control dialog class.
	*/
	FsGui3DViewControlDialog *viewControlDlg;

	/*! Main menu.  MainMenu is created in MakeMainMenu function, which is called 
	    from the constructor.
	*/
	FsGuiPopUpMenu *mainMenu;

	/*! Low-level interface, FsLazyWindow framework, checks for this value to see
	    if the application run-loop should be terminated.
	*/
	YSBOOL appMustTerminate;

	class ControlDialog;
	class CommentDialog;
	class DrawingToolDialog : public FsGuiDialog
	{
	public:
		FsGui3DMainCanvas *canvasPtr;

		FsGuiButton *noneBtn;
		FsGuiButton *lineBtn;
		FsGuiButton *lineStripBtn;

		FsGuiButton *multipleBtn;

		FsGuiColorPalette *lineColor,*fillColor;
		FsGuiButton *frontArrowBtn,*backArrowBtn;
		FsGuiTextBox *arrowSizeTxt;
		FsGuiButton *makeDefaultArrowSizeBtn;
		FsGuiButton *fillBtn;
		FsGuiButton *autoAnchorBtn;
		FsGuiButton *closeBtn;
		void Make(FsGui3DMainCanvas *canvasPtr);
		void TurnOffDrawingMode(void);
		virtual void OnButtonClick(FsGuiButton *btn);
	};

	FileAndDirectoryName fileAndDirName;

private:
	/*! For convenience, you can use THISCLASS instead of FsGui3DMainCanvas 
	    in the member functions.
	*/
	typedef FsGui3DMainCanvas THISCLASS;

public:
	// [Core data structure]
	enum UIMODE
	{
		NORMAL, // Click to select, Drag to move
		SELECTING_BITMAP_TRIM_AREA,
		DRAWING_LINE,
		DRAWING_LINE_STRIP,
	};
	enum DEFAULTACTION
	{
		DEFAULTACTION_STANDBY,
		DEFAULTACTION_MOVING,
		DEFAULTACTION_CLICKED,
		DEFAULTACTION_BOXSELECTING,
		DEFAULTACTION_MOVING_MARKUPPOINT,

		DEFAULTACTION_MARKUPPOINTSELECTION,
		DEFAULTACTION_MARKUPPOINTSELECTION_CLICKED,
		DEFAULTACTION_MARKUPPOINTSELECTION_BOXSELECTING,
	};
	UIMODE mode;
	DEFAULTACTION defAction;
	YsVec3 defAction_DragStart,defAction_CurrentDrag;
	RetroMap_World::MarkUpPointIndex defAction_MovingPointIndex;
	std::function<void()> modeEndingFunction;
	std::function<void()> draw3dCallBack;
	std::function<void(const YsArray <RetroMap_World::MarkUpPointIndex> &)> markUpPointSelectionCallback;

	enum INSERTMARKUP_STATUS
	{
		INSERTMARKUP_STANDBY,
		INSERTMARKUP_CLICKED,
		INSERTMARKUP_DRAGGING,
	};
	INSERTMARKUP_STATUS insertMarkUp_Status;
	YsArray <YsVec3,2> insertMarkUp_Point;

	RetroMap_World world;
	RetroMap_World::Clipboard clipboard;
	YSHASHKEY currentFieldKey;

	YsVec2i position_reference_marker;

	RetroMap_World::FieldHandle GetCurrentFieldCreateIfNotExist(void);
	RetroMap_World::FieldHandle GetCurrentField(void) const;

	RetroMap_Config config;
	YsAVLTree <YsWString,int,YsStringComparer <YsWString> > scrnShotDirCache;

	YSBOOL showDiff;



	// [Modeless dialogs]
	//   (1) Add a pointer in the following chunk.
	//   (2) Add an initialization in the constructor of the aplication.
	//   (3) Add deletion in the destructor of the application.
	//   (4) Add RemoveDialog in Edit_ClearUIIMode
	ControlDialog *controlDlg;
	CommentDialog *commentDlg;
	DrawingToolDialog *drawingDlg;


	// [Modal dialogs]



	// [Menu with Check]
	FsGuiPopUpMenuItem *markUp_Snap;



	/*! Constructor is called after OpenGL context is created.
	    It is safe to make OpenGL function calls inside.
	*/
	FsGui3DMainCanvas();

	/*! */
	~FsGui3DMainCanvas();

	/*! This function is called from the low-level interface to get an
	    application pointer.
	*/
	static FsGui3DMainCanvas *GetMainCanvas();

	/*! This funciion is called from the low-level interface for
	    deleting the application.
	*/
	static void DeleteMainCanvas(void);
protected:
	void AddViewControlDialog(void);

	/*! Customize this function for adding menus.
	*/
	void MakeMainMenu(void);
	void DeleteMainMenu(void);

	YSBOOL ShowDiff(void) const;
	int GetDiffThreshold(void) const;
	void UpdateDiff(void);
	void UpdateDiff(const RetroMap_World::MapPieceStore &mpStore,const RetroMap_World::MapPieceStore &exclusion);

public:
	/*! In this function, shared GLSL renderers are created,
	    View-target is set to (-5,-5,-5) to (5,5,5),
	    and view distance is set to 7.5 by default.
	*/
	void Initialize(int argc,char *argv[]);

	/*! This function is called regularly from the low-level interface.
	*/
	void OnInterval(void);

	/*! This function is called from the low-level interface when the
	    window needs to be re-drawn.
	*/
	void Draw(void);

	RetroMap_World::MapPieceHandle PickedMpHd(YsVec2i pos) const;
	RetroMap_World::MarkUpHandle PickedMuHd(YsVec2i pos) const;
	RetroMap_World::MarkUpPointIndex PickedMarkUpPoint(YsVec2i pos) const;
	RetroMap_World::MarkUpPointIndex PickedMarkUpPointWithinSelection(YsVec2i pos,const YsConstArrayMask <RetroMap_World::MarkUpHandle> &muHdFilter) const;
	YsArray <RetroMap_World::MapPieceHandle> BoxedMpHd(YsVec2 min,YsVec2 max) const;
	YsArray <RetroMap_World::MarkUpHandle> BoxedMuHd(YsVec2 min,YsVec2 max) const;
	YsArray <RetroMap_World::MarkUpPointIndex> BoxedMarkUpPointIndex(YsVec2 min,YsVec2 max) const;

	int GetPickableDistance(void) const;
	YSBOOL IsOnSelectedMapPiece(YsVec2i pos) const;
	void DisableDefaultAction(void);
	void EnableDefaultAction(void);
	YsVec2i GetPointToInsert(void) const;
	void SetCurrentField(RetroMap_World::FieldHandle fdHd);
	void UpdateFieldSelector(void);
	void UpdateNumBmp(void);
	void UpdateDiffThresholdFromWorld(void);
	void OpenDrawingDialog(void);
	void TurnOffDrawingMode(void);
	void SnapToUnit(YsVec3 &pos) const;
	void SnapToUnit(YsVec2i &pos) const;

private:
	// [Menu pointers for check marks]
	FsGuiPopUpMenuItem *view_showPositionReferenceMarker;
	FsGuiPopUpMenuItem *view_restoreViewWhenSwitchingField;
	FsGuiPopUpMenuItem *view_showMarkUp;


	// [Menu call-backs]
	/*! Sample call-back functions.
	*/
	void File_Open(FsGuiPopUpMenuItem *);
	void File_Open_ConfirmDiscardChanges(FsGuiDialog *dlg,int returnCode);
	void File_Open_OpenFileDialog(void);
	void File_Open_FileSelected(FsGuiDialog *dlg,int returnCode);
	void File_Open_Open(YsWString fn);

	void File_Save(FsGuiPopUpMenuItem *);

	void File_SaveAs(FsGuiPopUpMenuItem *);
	void File_SaveAs_FileSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveAs_OverwriteConfirmed(FsGuiDialog *dlg,int returnCode);
	void File_SaveAs_Save(const YsWString fn);

	void File_SaveAsOneBitmap(FsGuiPopUpMenuItem *);
	void File_SaveAsOneBitmap_OptionSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveAsOneBitmapFileSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveAsOneBitmapOverwriteConfirmed(FsGuiDialog *dlg,int returnCode);
	void File_SaveAsOneBitmapSave(const YsWString fn);

	class SaveAsOneBitmapOptionDialog;
	RetroMap_PngExportInfo pngExportInfo;
	YSBOOL File_SaveAsOneBitmapForEachField_IncludeMarkUp;
	void File_SaveAsOneBitmapForEachField(FsGuiPopUpMenuItem *);
	void File_SaveAsOneBitmapForEachField_OptionSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveAsOneBitmapForEachField_FileSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveAsOneBitmapForEachField_OverwriteConfirmed(FsGuiDialog *dlg,int returnCode);
	void File_SaveAsOneBitmapForEachField_Save(const YsWString fn);

	void File_InsertImage(FsGuiPopUpMenuItem *);
	void File_InsertImage_Open(FsGuiDialog *dlg,int returnCode);

	FsGuiPopUpMenu *fileRecent;
	FsGuiRecentFiles recent;
	void RefreshRecentlyUsedFileList(void);
	void AddRecentlyUsedFile(const wchar_t wfn[]);
	void File_Recent(FsGuiPopUpMenuItem *);
	void File_Recent_ConfirmOpen(FsGuiDialog *dlg,int returnCode);

public:
	void File_Exit(FsGuiPopUpMenuItem *);
private:
	void File_Exit_ConfirmExitCallBack(FsGuiDialog *,int);
	void File_Exit_ReallyExit(void);



	void Edit_ClearUIMode(FsGuiPopUpMenuItem *);

	void Edit_Undo(FsGuiPopUpMenuItem *);
	void Edit_Redo(FsGuiPopUpMenuItem *);

	void Edit_Copy(FsGuiPopUpMenuItem *);
	void Edit_Paste(FsGuiPopUpMenuItem *);

	class TrimDialog;
	void Edit_Trim(FsGuiPopUpMenuItem *);
	YsRect2i Edit_CurrentTrimRect(void);
	void Edit_Trim_3DInterfaceCallBack(FsGui3DInterface *);
	void Edit_Trim_DrawCallBack(void);

	class ScalingDialog;
	void Edit_Scaling(FsGuiPopUpMenuItem *);
	YsVec2 Edit_CurrentScaling(void);
	YsVec2i Edit_CurrentPositionForScaling(void);
	void Edit_Scaling_3DInterfaceCallBack(FsGui3DInterface *);

	void Edit_Delete(FsGuiPopUpMenuItem *);

	void Edit_Insert_Comment(FsGuiPopUpMenuItem *);

	class Edit_SearchDialog;
	void Edit_Search(FsGuiPopUpMenuItem *);


	void View_ShowPositionReferenceMarker(FsGuiPopUpMenuItem *);
	void View_MovePositionReferenceMarker(FsGuiPopUpMenuItem *);
	void View_MovePositionReferenceMarker_3DInterfaceCallBack(FsGui3DInterface *);
	void View_Fit(FsGuiPopUpMenuItem *);
	void View_LookAtSelection(FsGuiPopUpMenuItem *);
	void View_RestoreViewWhenSwitchingField(FsGuiPopUpMenuItem *);
	void View_ShowMarkUp(FsGuiPopUpMenuItem *);



	void Select_SelectAll(FsGuiPopUpMenuItem *);
	void Select_UnselectAll(FsGuiPopUpMenuItem *);



	class WorldPropertyDialog;
	void World_Property(FsGuiPopUpMenuItem *);

	class SetUnitDialog;
	void World_SetUnit(FsGuiPopUpMenuItem *);
	void World_SetUnit_Draw3DCallBack(void);
	void World_SetUnit_BoxSizeChange(FsGui3DInterface *);



	class FieldPropertyDialog;
	void Field_Property(FsGuiPopUpMenuItem *);

	void Field_NewField(FsGuiPopUpMenuItem *);

	void Field_DeleteField(FsGuiPopUpMenuItem *);
	void Field_DeleteField_Confirm(FsGuiDialog *dlg,int returnCode);

	class FieldOrderDialog;
	void Field_ChangeOrder(FsGuiPopUpMenuItem *);
	void Field_ChangeOrder_DialogClosed(FsGuiDialog *dlg,int returnCode);

	void Field_NextField(FsGuiPopUpMenuItem *);
	void Field_PrevField(FsGuiPopUpMenuItem *);



	void MapPiece_InsertNewScreenshot(FsGuiPopUpMenuItem *);
	RetroMap_World::MapPieceHandle MapPiece_Insert_Insert(RetroMap_World::FieldHandle fdHd,const YsWString &fn);
	void MapPiece_MoveToFront(FsGuiPopUpMenuItem *);
	void MapPiece_MoveToBack(FsGuiPopUpMenuItem *);

	void MapPiece_RecacheScreenshot(FsGuiPopUpMenuItem *);

	void MapPiece_Untrim(FsGuiPopUpMenuItem *);

	void MapPiece_ReapplyTrimAll(FsGuiPopUpMenuItem *);

	class TrimWithLargeRectangleDialog;
	void MapPiece_TrimSelectionWithLargeRectangle(FsGuiPopUpMenuItem *);
	void MapPiece_TrimSelectionWithLargeRectangle_3DInterfaceCallBack(FsGui3DInterface *);
	void MapPiece_TrimSelectionWithLargeRectangle_DrawCallBack(void);
	YsRect2i MapPiece_TrimSelectionWithLargeRectangle_GetLargeRect(void) const;

	void MapPiece_MinimizeError(FsGuiPopUpMenuItem *);
private:
	class MapPiece_MinimizeErrorThreadInfo
	{
	public:
		RetroMap_World *worldPtr;
		RetroMap_World::FieldHandle fdHd;
		RetroMap_World::MapPieceHandle mpHd;
		const RetroMap_World::MapPieceStore *exclusionPtr;
		RetroMap_World::MapPiece::Shape shape;
		int thr;
		YsColor diffColor;

		RetroMap_World::DiffInfo diffInfo;

		void Run(void);
	};

public:
	void MarkUp_SnapSwitch(FsGuiPopUpMenuItem *);
	void MarkUp_SnapToHalfUnit(YsVec3 &pos) const;
	void MarkUp_SnapToHalfUnit(YsVec2i &pos) const;

	void MarkUp_Property(FsGuiPopUpMenuItem *);

	void MarkUp_OpenDrawingDialog(FsGuiPopUpMenuItem *);

	void MarkUp_InsertLine(FsGuiPopUpMenuItem *);

	void MarkUp_InsertLineStrip(FsGuiPopUpMenuItem *);
	void MarkUp_InsertLineStrip_EndUIMode(void);
	void MarkUp_InsertLineStrip_SpaceKeyCallBack(FsGuiPopUpMenuItem *);
	void MarkUp_InsertLineStrip_BackSpaceKeyCallBack(FsGuiPopUpMenuItem *);

	void MarkUp_InsertLine_DrawCallBack(void);
	void MarkUp_InsertLine_Finalize(void);
	YSRESULT MarkUp_InsertLine_OnLButtonDown(FsGuiMouseButtonSet,YsVec2i);
	YSRESULT MarkUp_InsertLine_OnMouseMove(FsGuiMouseButtonSet,YsVec2i,YsVec2i);
	YSRESULT MarkUp_InsertLine_OnLButtonUp(FsGuiMouseButtonSet,YsVec2i);
	YSRESULT MarkUp_InsertLine_OnTouchStateChange(const FsGuiObject::TouchState &);

	YSRESULT PointerIn(YsVec2i pos);
	YSRESULT PointerMove(YsVec2i pos);
	YSRESULT PointerUp(YsVec2i pos);
	void AxisAlignMarkUpPoint(YsArrayMask <YsVec2i> arrayMask,int idx) const;

	void MarkUp_SetAnchorByPicking(FsGuiPopUpMenuItem *);
	void MarkUp_SetAnchorByPicking_PointSelected(const YsArray <RetroMap_World::MarkUpPointIndex> &);

	void MarkUp_SetAnchorOnSelectedMarkUp(FsGuiPopUpMenuItem *);

	void MarkUp_UnsetAnchorByPicking(FsGuiPopUpMenuItem *);
	void MarkUp_UnsetAnchorByPicking_PointSelected(const YsArray <RetroMap_World::MarkUpPointIndex> &);

	void MarkUp_UnsetAnchorOfSelectedMarkUp(FsGuiPopUpMenuItem *);

	void MarkUp_ReapplyAnchor(FsGuiPopUpMenuItem *);

	void SetAnchor(RetroMap_World::FieldHandle fdHd,RetroMap_World::MarkUpHandle muHd,YSSIZE_T idx);
	void Reanchor(RetroMap_World::FieldHandle fdHd,RetroMap_World::MarkUpHandle muHd);
	void Reanchor(RetroMap_World::FieldHandle fdHd,RetroMap_World::MarkUpHandle muHd,YSSIZE_T idx);


	class ConfigDialog;
	void Config_Config(FsGuiPopUpMenuItem *);

	void Config_DataIntegrityCheck(FsGuiPopUpMenuItem *);


	// [Key/Mouse call backs]
	YSRESULT OnLButtonDown(FsGuiMouseButtonSet,YsVec2i);
	YSRESULT OnMouseMove(FsGuiMouseButtonSet,YsVec2i,YsVec2i);
	YsVec2i MouseMove_Displacement(double x,double y) const;
	YSRESULT OnLButtonUp(FsGuiMouseButtonSet,YsVec2i);
	YSRESULT OnTouchStateChange(const FsGuiObject::TouchState &);
	void OnUpArrowKey(FsGuiPopUpMenuItem *);
	void OnDownArrowKey(FsGuiPopUpMenuItem *);
	void OnLeftArrowKey(FsGuiPopUpMenuItem *);
	void OnRightArrowKey(FsGuiPopUpMenuItem *);

	void OnUpArrowKeyShift(FsGuiPopUpMenuItem *);
	void OnDownArrowKeyShift(FsGuiPopUpMenuItem *);
	void OnLeftArrowKeyShift(FsGuiPopUpMenuItem *);
	void OnRightArrowKeyShift(FsGuiPopUpMenuItem *);

	void OnUpArrowKeyCtrl(FsGuiPopUpMenuItem *);
	void OnDownArrowKeyCtrl(FsGuiPopUpMenuItem *);
	void OnLeftArrowKeyCtrl(FsGuiPopUpMenuItem *);
	void OnRightArrowKeyCtrl(FsGuiPopUpMenuItem *);

	void OnPageUpKey(FsGuiPopUpMenuItem *);
	void OnPageDownKey(FsGuiPopUpMenuItem *);
};

/* } */
#endif
