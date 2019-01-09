/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp_menusetup.cpp
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

#include <functional>

#include <ysdebug.h>

#include "ysgebl_gui_editor_base.h"
#include "miscdlg.h"

#include <fsguifiledialog.h>

#include <ysgl.h>

#include "textresource.h"

#include "ysgebl_gui_extension_base.h"


void GeblGuiEditorBase::MakeMainMenu(void)
{
	YsDebug::InOutMessage inOut(__FUNCTION__,__LINE__);

	auto mainMenuOpt=guiExtension->GetMainMenuOption();

	this->fileMenu=nullptr;
	fileRecent=nullptr;

	viewDrawAxis=nullptr;
	viewDrawVertex=nullptr;
	viewDrawConstEdge=nullptr;
	viewDrawPolygonFill=nullptr;
	viewDrawWireframe=nullptr;
	viewDrawShrunkPolygon=nullptr;
	viewDrawBackFaceInDifferentColor=nullptr;
	viewEnableViewPort=nullptr;
	viewEnableCrossSection=nullptr;
	viewUseSameViewPortForAll=nullptr;
	viewShowHighlight=nullptr;
	viewHighlightNonManifoldEdge=nullptr;
	viewDrawBackgroundGradation=nullptr;

	viewDrawOtherShellWire=nullptr;
	viewDrawOtherShellFill=nullptr;
	viewDrawOtherShellNone=nullptr;

	viewDrawDnmConnection=nullptr;

	globalTargetCurrentShellOnly=nullptr;
	globalTargetAllShell=nullptr;

	mainMenu=new GeblGuiEditorMainMenu(this);
	mainMenu->Initialize();
	mainMenu->SetIsPullDownMenu(YSTRUE);

	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_FILE))
	{
		FsGuiPopUpMenuItem *fileMenu=mainMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_FILE);
		FsGuiPopUpMenu *fileSubMenu=fileMenu->GetSubMenu();

		this->fileMenu=fileSubMenu;

		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_NEW))
		{
			fileSubMenu->AddTextItem(mainMenu->MkId("file/new"),      FSKEY_N,FSGUI_MENU_FILE_NEW)->BindCallBack(&THISCLASS::File_New,this);
		}
		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_OPEN))
		{
			fileSubMenu->AddTextItem(mainMenu->MkId("file/open"),     FSKEY_O,FSGUI_MENU_FILE_OPEN)->BindCallBack(&THISCLASS::File_Open,this);
		}
		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_SAVE))
		{
			fileSubMenu->AddTextItem(mainMenu->MkId("file/save"),     FSKEY_S,FSGUI_MENU_FILE_SAVE)->BindCallBack(&THISCLASS::File_Save,this);
		}
		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_SAVEAS))
		{
			fileSubMenu->AddTextItem(mainMenu->MkId("file/saveas"),   FSKEY_A,FSGUI_MENU_FILE_SAVEAS)->BindCallBack(&THISCLASS::File_SaveAs,this);
		}
		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_CLOSE))
		{
			fileSubMenu->AddTextItem(mainMenu->MkId("file/close"),    FSKEY_C,FSGUI_MENU_FILE_CLOSE)->BindCallBack(&THISCLASS::File_Close,this);
		}

		fileSubMenu->AddTextItem(mainMenu->MkId("file/exit"),     FSKEY_X,FSGUI_MENU_FILE_EXIT)->BindCallBack(&THISCLASS::File_Exit,this);

		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_SLIDESHOW))
		{
			fileSubMenu->AddTextItem(mainMenu->MkId("file/slideshow"),FSKEY_NULL,FSGUI_MENU_FILE_SLIDESHOW,File_SlideShow,this);
		}
		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_GETINFO))
		{
			auto getInfoSubMenu=fileSubMenu->AddTextItem(0,FSKEY_I,FSGUI_MENU_FILE_GETINFO)->AddSubMenu();
			getInfoSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_FILE_GETINFO_STATISTICS,File_GetInfo_Statistics,this);
			getInfoSubMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_FILE_GETINFO_VOLUME,File_GetInfo_Volume,this);

			auto getInfoLengthSubMenu=getInfoSubMenu->AddTextItem(0,FSKEY_L,FSGUI_MENU_FILE_GETINFO_LENGTH)->AddSubMenu();
			getInfoLengthSubMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_FILE_GETINFO_LENGTH_BETWEENVTX,File_GetInfo_Length_BetweenVertex,this);
			getInfoLengthSubMenu->AddTextItem(0,FSKEY_E,FSGUI_MENU_FILE_GETINFO_LENGTH_CONSTEDGE,File_GetInfo_Length_ConstEdge,this);

			auto getInfoAngleSubMenu=getInfoSubMenu->AddTextItem(0,FSKEY_G,FSGUI_MENU_FILE_GETINFO_ANGLE)->AddSubMenu();
			AddMenuItem(getInfoAngleSubMenu,FSKEY_NULL,FSGUI_MENU_FILE_GETINFO_ANGLE_NORMALDEVIATIONACROSSCONSTEDGE,&THISCLASS::File_GetInfo_MaxNormalDeviationAcrossConstEdge);
			getInfoAngleSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_FILE_GETINFO_ANGLE_TWOPOLYGON)->BindCallBack(&THISCLASS::File_GetInfo_AngleBetweenSelectedPolygon,this);
			getInfoAngleSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_FILE_GETINFO_ANGLE_DIHEDRALANGLE)->BindCallBack(&THISCLASS::File_GetInfo_DihedralAngleAcrossSelectedEdge,this);
			getInfoAngleSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_FILE_GETINFO_ANGLE_MINMAXAVGDHA)->BindCallBack(&THISCLASS::File_GetInfo_MinMaxAvgDihedralAngle,this);

			auto getInfoAreaSubMenu=getInfoSubMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_FILE_GETINFO_AREA)->AddSubMenu();
			getInfoAreaSubMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_FILE_GETINFO_AREA_ALL,File_GetInfo_Area_All,this);
			getInfoAreaSubMenu->AddTextItem(0,FSKEY_P,FSGUI_MENU_FILE_GETINFO_AREA_SELECTEDPOLYGON,File_GetInfo_Area_SelectedPolygon,this);

			auto getInfoPositionSubMenu=getInfoSubMenu->AddTextItem(0,FSKEY_P,FSGUI_MENU_FILE_GETINFO_POSITION)->AddSubMenu();
			getInfoPositionSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_FILE_GETINFO_POSITION_SELVTX,File_GetInfo_Position_SelectedVertex,this);

			{
				auto debugInfoMenu=getInfoSubMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_FILE_GETINFO_DEBUGINFO)->AddSubMenu();
				AddMenuItem(debugInfoMenu,FSKEY_S,FSGUI_MENU_FILE_GETINFO_DEBUGINFO_OFSELECTION,&THISCLASS::File_GetInfo_DebugInfo_Selection);
			}
		}
		if(0!=(mainMenuOpt.fileMenuOption&mainMenuOpt.ADD_FILE_RECENT))
		{
			this->fileRecent=fileSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_FILE_RECENT)->AddSubMenu();
			RefreshRecentlyUsedFileList();
		}
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_EDIT))
	{
		FsGuiPopUpMenuItem *editMenu=mainMenu->AddTextItem(0,FSKEY_E,FSGUI_MENU_EDIT);
		FsGuiPopUpMenu *editSubMenu=editMenu->GetSubMenu();

		editSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_CLEARUIMODE)->BindCallBack(&THISCLASS::Edit_ClearUIMode,this);
		BindKeyCallBack(FSKEY_ESC,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_ESCKey,this);

		editSubMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_EDIT_UNDO)->BindCallBack(&THISCLASS::Edit_Undo,this);
		BindKeyCallBack(FSKEY_Z,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Undo,this);

		editSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_EDIT_REDO)->BindCallBack(&THISCLASS::Edit_Redo,this);
		BindKeyCallBack(FSKEY_Y,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Redo,this);

		editSubMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_EDIT_COPY)->BindCallBack(&THISCLASS::Edit_Copy,this);
		BindKeyCallBack(FSKEY_C,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Copy,this);
		editSubMenu->AddTextItem(0,FSKEY_P,FSGUI_MENU_EDIT_PASTE)->BindCallBack(&THISCLASS::Edit_Paste,this);
		BindKeyCallBack(FSKEY_V,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Paste,this);
		editSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_EDIT_PASTESPECIAL)->BindCallBack(&THISCLASS::Edit_PasteSpecial,this);
		editSubMenu->AddTextItem(mainMenu->MkId("menu/edit/paste_with_trans"),FSKEY_NULL,FSGUI_MENU_EDIT_PASTE_WITH_TRANSFORMATION)->BindCallBack(&THISCLASS::Edit_PasteWithTransformation,this);

		auto edit_insert=editSubMenu->AddTextItem(0,FSKEY_I,FSGUI_MENU_EDIT_INSERT);
		auto edit_delete=editSubMenu->AddTextItem(0,FSKEY_D,FSGUI_MENU_EDIT_DELETE);
		auto edit_create=editSubMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_EDIT_CREATE);
		auto edit_sweep=editSubMenu->AddTextItem(0,FSKEY_W,FSGUI_MENU_EDIT_SWEEP);
		auto edit_offset=editSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_OFFSET);
		auto edit_align=editSubMenu->AddTextItem(0,FSKEY_L,FSGUI_MENU_EDIT_ALIGN)->AddSubMenu();
		auto edit_round=editSubMenu->AddTextItem(0,FSKEY_O,FSGUI_MENU_EDIT_ROUND);
		auto edit_localOp=editSubMenu->AddTextItem(0,FSKEY_SPACE,FSGUI_MENU_EDIT_LOCAL);
		auto edit_intersection=editSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_INTERSECTION);
		auto edit_stitch=editSubMenu->AddTextItem(0,FSKEY_H,FSGUI_MENU_EDIT_STITCH)->AddSubMenu();

		{
			auto edit_renderingOption=editSubMenu->AddTextItem(0,FSKEY_G,FSGUI_MENU_EDIT_RENDERINGOPTION)->AddSubMenu();
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_SMOOTHSHADESELECTEDVERTEX)->BindCallBack(&THISCLASS::Edit_SmoothShadeSelectedVertex,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_FLATSHADESELECTEDVERTEX)->BindCallBack(&THISCLASS::Edit_FlatShadeSelectedVertex,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_SETSHADEDPOLYGON)->BindCallBack(&THISCLASS::Edit_ShadeSelectedPolygon,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_SETUNSHADEDPOLYGON)->BindCallBack(&THISCLASS::Edit_SelfLightSelectedPolygon,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_SETPOLYGONALPHA)->BindCallBack(&THISCLASS::Edit_SetPolygonAlpha,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_BREAKSMOOTHSHADING)->BindCallBack(&THISCLASS::Edit_DivideSmoothShadingRegionOnSelectedConstEdge,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_SMOOTHSHADINGONCONSTEDGE)->BindCallBack(&THISCLASS::Edit_UndivideSmoothShadingRegionOnSelectedConstEdge,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_RENDERASLIGHT)->BindCallBack(&THISCLASS::Edit_SetRenderAsLight,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_DONTRENDERASLIGHT)->BindCallBack(&THISCLASS::Edit_DontRenderAsLight,this);
			edit_renderingOption->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_RENDERINGOPTION_ZERONORMAL_ON_SELECTION)->BindCallBack(&THISCLASS::Edit_ZeroNormalSelectedPolygon,this);
		}

		{
			auto edit_move=editSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_MOVE)->AddSubMenu();
			edit_move->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_MOVE_PRESELECTDIRECTION_FROMNORMAL)->BindCallBack(&THISCLASS::Edit_Move_SelectDirectionFromNormal,this);
			edit_move->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_MOVE_PRESELECTVECTOR_FROM2VTX)->BindCallBack(&THISCLASS::Edit_Move_SelectVectorFromTwoVertex,this);
			edit_move->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_MOVE_SELECTION_PRESELECTEDDIRECTION)->BindCallBack(&THISCLASS::Edit_Move_SelectionInPreSelectedDirection,this);
			edit_move->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_MOVE_SELECTION_PRESELECTEDDIRECTION_SPECIFYDIST)->BindCallBack(&THISCLASS::Edit_Move_SelectionInPreSelectedDirection_SpecificDistance,this);		edit_move->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_MOVE_SELECTION_BYPRESELECTED_VECTOR)->BindCallBack(&THISCLASS::Edit_Move_SelectionInPreSelectedVector,this);
			edit_move->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_MOVE_VTX_3PLANEITSC)->BindCallBack(&THISCLASS::Edit_MoveVertexToThreePlaneIntersection,this);
		}

		{
			edit_stitch->AddTextItem(mainMenu->MkId("menu/stitch/byselvertex"),FSKEY_S,FSGUI_MENU_EDIT_STITCH_BY_SELECTED_VERTICES)->BindCallBack(&THISCLASS::Edit_SimpleStitch,this);
		}

		editSubMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_EDIT_MOVESELECTION)->BindCallBack(&THISCLASS::Edit_MoveSelection,this);
		editSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_SCALESELECTION)->BindCallBack(&THISCLASS::Edit_ScaleSelection,this);

		editSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ROTATESELECTION)->BindCallBack(&THISCLASS::Edit_RotateSelection,this);

		FsGuiPopUpMenuItem *edit_projection=editSubMenu->AddTextItem(0,FSKEY_J,FSGUI_MENU_EDIT_PROJECTION);

		FsGuiPopUpMenuItem *edit_paint=editSubMenu->AddTextItem(0,FSKEY_T,FSGUI_MENU_EDIT_PAINT);

		{
			auto insertMenu=edit_insert->AddSubMenu();

			insertMenu->AddTextItem(mainMenu->MkId("menu/edit/insertVertex"),FSKEY_V,FSGUI_MENU_EDIT_INSERT_VERTEX)->BindCallBack(&THISCLASS::Edit_InsertVertex,this);
			BindKeyCallBack(FSKEY_V,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_InsertVertex,this);

			insertMenu->AddTextItem(mainMenu->MkId("menu/edit/insertPolygon"),FSKEY_F,FSGUI_MENU_EDIT_INSERT_POLYGON)->BindCallBack(&THISCLASS::Edit_InsertPolygon,this);
			BindKeyCallBack(FSKEY_F,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_InsertPolygon,this);

			insertMenu->AddTextItem(mainMenu->MkId("menu/edit/constEdge"),FSKEY_E,FSGUI_MENU_EDIT_INSERT_CONSTEDGE)->BindCallBack(&THISCLASS::Edit_InsertConstEdge,this);
			BindKeyCallBack(FSKEY_E,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_InsertConstEdge,this);

			insertMenu->AddTextItem(mainMenu->MkId("menu/edit/faceGroup"),FSKEY_G,FSGUI_MENU_EDIT_INSERT_FACEGROUP)->BindCallBack(&THISCLASS::Edit_InsertFaceGroup,this);

			{
				auto insertVertexAtMenu=insertMenu->AddTextItem(0,FSKEY_X,FSGUI_MENU_EDIT_INSERT_VERTEXAT)->AddSubMenu();
				insertVertexAtMenu->AddTextItem(0,FSKEY_I,FSGUI_MENU_EDIT_INSERT_VERTEXAT_NEARESTOF2LINE)->BindCallBack(&THISCLASS::Edit_InsertVertexAtTwoLineIntersection,this);
			}

			edit_delete->AddSubMenu();

			edit_delete->GetSubMenu()->AddTextItem(0,FSKEY_D,FSGUI_MENU_EDIT_DELETE_ALLSELECTION)->BindCallBack(&THISCLASS::Edit_DeleteSelection,this);
			BindKeyCallBack(FSKEY_DEL,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_DeleteSelection,this);

			edit_delete->GetSubMenu()->AddTextItem(0,FSKEY_F,FSGUI_MENU_EDIT_FORCEDELETESELECTION)->BindCallBack(&THISCLASS::Edit_ForceDeleteSelection,this);
			BindKeyCallBack(FSKEY_DEL,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Edit_ForceDeleteSelection,this);



			{
				auto edit_createMenu=edit_create->AddSubMenu();
				
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/primitive")         ,FSKEY_S,FSGUI_MENU_EDIT_CREATE_PRIMITIVESHAPE)->BindCallBack(&THISCLASS::Edit_CreatePrimitiveShape,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/airfoil")           ,FSKEY_A,FSGUI_MENU_EDIT_CREATE_AIRFOIL)->BindCallBack(&THISCLASS::Edit_CreateAirfoil,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/mirror")            ,FSKEY_M,FSGUI_MENU_EDIT_CREATE_MIRRORIMAGE)->BindCallBack(&THISCLASS::Edit_CreateMirrorImage,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/parallelogram_2vtx"),FSKEY_NULL,FSGUI_MENU_EDIT_CREATE_PARALLELOGRAMFROMTWOVTX)->BindCallBack(&THISCLASS::Edit_CreateParallelogramFromTwoVertex,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/parallelogram_3vtx"),FSKEY_NULL,FSGUI_MENU_EDIT_CREATE_PARALLELOGRAMFROMTHREEVTX)->BindCallBack(&THISCLASS::Edit_CreateParallelogramFromThreeVertex,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/equilateral")       ,FSKEY_NULL,FSGUI_MENU_EDIT_CREATE_EQUILATERAL)->BindCallBack(&THISCLASS::Edit_CreateEquilateral,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/ellipsefromvtx"),    FSKEY_NULL,FSGUI_MENU_EDIT_CREATE_ELLIPSE_FROM_VTX)->BindCallBack(&THISCLASS::Edit_CreateEllipseFromSelectedVertex,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/solidOfRevolution") ,FSKEY_R,FSGUI_MENU_EDIT_CREATE_SOLIDOFREVOLUTION)->BindCallBack(&THISCLASS::Edit_CreateSolidOfRevolution,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/cone")              ,FSKEY_NULL,FSGUI_MENU_EDIT_CREATE_CONEFROMVTXANDPLG)->BindCallBack(&THISCLASS::Edit_CreateConeFromPolygonAndVertex,this);
				edit_createMenu->AddTextItem(mainMenu->MkId("edit/create/beziercurve")       ,FSKEY_NULL,FSGUI_MENU_EDIT_CREATE_BEZIERCURVE)->BindCallBack(&THISCLASS::Edit_Create_BezierCurve,this);
			}

			edit_paint->AddSubMenu();

			edit_paint->GetSubMenu()->AddTextItem(0,FSKEY_F,FSGUI_MENU_EDIT_PAINT_BYPICKINGPLG)->BindCallBack(&THISCLASS::Edit_Paint_ByPickingPolygon,this);

			edit_paint->GetSubMenu()->AddTextItem(0,FSKEY_C,FSGUI_MENU_EDIT_PAINT_BYPICKINGFG,Edit_Paint_ByPickingFaceGroup,this);

			edit_paint->GetSubMenu()->AddTextItem(0,FSKEY_S,FSGUI_MENU_EDIT_PAINT_SELECTEDPLG)->BindCallBack(&THISCLASS::Edit_Paint_SelectedPolygon,this);

			edit_paint->GetSubMenu()->AddTextItem(0,FSKEY_A,FSGUI_MENU_EDIT_PAINT_ALL)->BindCallBack(&THISCLASS::Edit_Paint_All,this);

			edit_paint->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_PAINT_FLOODFILLSAMECOLOR)->BindCallBack(&THISCLASS::Edit_Paint_FloodFill_SameColor,this);

			edit_paint->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_PAINT_FLOODFILLCONSTEDGE)->BindCallBack(&THISCLASS::Edit_Paint_FloodFill_BoundedByConstEdge,this);

			edit_paint->GetSubMenu()->AddTextItem(0,FSKEY_K,FSGUI_MENU_EDIT_PAINT_PICKUPCOLOR)->BindCallBack(&THISCLASS::Edit_Paint_PickUpColor,this);

			edit_localOp->AddSubMenu();

			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_W,FSGUI_MENU_EDIT_LOCAL_SWAPEDGE)->BindCallBack(&THISCLASS::Edit_SwapEdge,this);
			BindKeyCallBack(FSKEY_W,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_SwapEdge,this);

			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_G,FSGUI_MENU_EDIT_LOCAL_MERGEPLG)->BindCallBack(&THISCLASS::Edit_MergePolygon,this);
			BindKeyCallBack(FSKEY_G,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_MergePolygon,this);

			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_S,FSGUI_MENU_EDIT_LOCAL_SPLITPLG)->BindCallBack(&THISCLASS::Edit_MergePolygon,this);
			BindKeyCallBack(FSKEY_S,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_SplitPolygon,this);

			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_INVERTSELECTEDPOLYGON)->BindCallBack(&THISCLASS::Edit_InvertSelectedPolygon,this);

			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_MERGEVTXMIDPOINT)->BindCallBack(&THISCLASS::Edit_VertexCollapseMidPoint,this);
			BindKeyCallBack(FSKEY_D,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_VertexCollapseMidPoint,this);

			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_MERGEVTXATOB)->BindCallBack(&THISCLASS::Edit_VertexCollapseAtoB,this);
			BindKeyCallBack(FSKEY_D,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Edit_VertexCollapseAtoB,this);

			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_HEMMINGAROUNDPLG)->BindCallBack(&THISCLASS::Edit_Hemming_AroundSelectedPolygon,this);
			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_HEMMINGOUTSIDEPLG)->BindCallBack(&THISCLASS::Edit_Hemming_AroundOutsideOfSelectedPolygon,this);
			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_HEMMINGONESIDEVTXSEQ)->BindCallBack(&THISCLASS::Edit_Hemming_OneSideOfVertexSequence,this);
			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_T,FSGUI_MENU_EDIT_LOCAL_TRIANGULATESELECTION,Edit_TriangulateSelection,this);
			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_I,FSGUI_MENU_EDIT_LOCAL_INVERTCONSTEDGE,Edit_InvertConstEdge,this);
			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_SPLITCONSTEDGE,Edit_SplitConstEdge,this);
			edit_localOp->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_MERGECONSTEDGE,Edit_MergeConstEdge,this);
			AddMenuItem(edit_localOp->GetSubMenu(),FSKEY_NULL,FSGUI_MENU_EDIT_LOCAL_SEWBETWEENVTXS,&THISCLASS::Edit_SewBetweenTwoVertex);


			edit_intersection->AddSubMenu();

			edit_intersection->GetSubMenu()->AddTextItem(0,FSKEY_P,FSGUI_MENU_EDIT_INTERSECTION_CUTBYPLANE,Edit_Intersection_CutByPlane,this);

			edit_intersection->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_INTERSECTION_CHOPOFF,Edit_Intersection_ChopOff,this);


			edit_sweep->AddSubMenu();

			edit_sweep->GetSubMenu()->AddTextItem(mainMenu->MkId("edit/sweep/parallelsweep"),FSKEY_P,FSGUI_MENU_EDIT_SWEEP_PARALLEL)->BindCallBack(&THISCLASS::Edit_Sweep_ParallelSweep,this);
			edit_sweep->GetSubMenu()->AddTextItem(0,FSKEY_S,FSGUI_MENU_EDIT_SWEEP_NONPARALLELSWEEPWITHPATH,Edit_Sweep_AlongSweepPath,this);
			edit_sweep->GetSubMenu()->AddTextItem(mainMenu->MkId("edit/sweep/rotational_with_path"),FSKEY_R,FSGUI_MENU_EDIT_SWEEP_ROTATIONAL_WITH_PATH)->BindCallBack(&THISCLASS::Edit_Sweep_RotationalWithPath,this);
			edit_sweep->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_SWEEP_PARALLELWITHPATH,Edit_Sweep_ParallelSweepWithPath,this);
			edit_sweep->GetSubMenu()->AddTextItem(0,FSKEY_C,FSGUI_MENU_EDIT_SWEEP_BETWEENCE)->BindCallBack(&THISCLASS::Edit_Sweep_BetweenConstEdge,this);
			edit_sweep->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_SWEEP_EXTENDORSHRINK)->BindCallBack(&THISCLASS::Edit_Sweep_ExtendOrShrink,this);


			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_MINIMUMX)->BindCallBack(&THISCLASS::Edit_Align_MinimumX,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_CENTERX)->BindCallBack(&THISCLASS::Edit_Align_CenterX,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_MAXIMUMX)->BindCallBack(&THISCLASS::Edit_Align_MaximumX,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_MINIMUMY)->BindCallBack(&THISCLASS::Edit_Align_MinimumY,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_CENTERY)->BindCallBack(&THISCLASS::Edit_Align_CenterY,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_MAXIMUMY)->BindCallBack(&THISCLASS::Edit_Align_MaximumY,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_MINIMUMZ)->BindCallBack(&THISCLASS::Edit_Align_MinimumZ,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_CENTERZ)->BindCallBack(&THISCLASS::Edit_Align_CenterZ,this);
			edit_align->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ALIGN_MAXIMUMZ)->BindCallBack(&THISCLASS::Edit_Align_MaximumZ,this);


			edit_offset->AddSubMenu();
			edit_offset->GetSubMenu()->AddTextItem(0,FSKEY_T,FSGUI_MENU_EDIT_OFFSET_THICKENING,Edit_Offset_Thickening,this);


			edit_round->AddSubMenu();
			edit_round->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ROUND_POLYGON_CONSTEDGE_ALLCORNERS,Edit_Round_PolygonOrConstEdge,this);
			edit_round->GetSubMenu()->AddTextItem(0,FSKEY_C,FSGUI_MENU_EDIT_ROUND_POLYGON_CONSTEDGE_SELECTEDCORNERS,Edit_Round_PolygonOrConstEdgeSelectedCorner,this);
			edit_round->GetSubMenu()->AddTextItem(0,FSKEY_V,FSGUI_MENU_EDIT_ROUND_AROUNDSELECTEDVERTEX,Edit_Round_AroundVertex,this);
			edit_round->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ROUND_AROUNDSELECTEDPOLYGONS,Edit_Round_AroundSelectedPolygon,this);
			edit_round->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ROUND_ALONGVERTEXSEQ,Edit_Round_OpenVertexSequence,this);

			edit_round->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_ROUND_UNROUNDVTSEQ)->BindCallBack(&THISCLASS::Edit_UnroundVertexSequence,this);


			edit_projection->AddSubMenu();
			edit_projection->GetSubMenu()->AddTextItem(0,FSKEY_D,FSGUI_MENU_EDIT_PROJECTION_OPENPROJDIALOG,Edit_Projection,this);
			edit_projection->GetSubMenu()->AddTextItem(0,FSKEY_I,FSGUI_MENU_EDIT_PROJECTION_IMPRINTING)->BindCallBack(&THISCLASS::Edit_ImprintSelection,this);
		}
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_BOOL))
	{
		FsGuiPopUpMenuItem *boolMenu=mainMenu->AddTextItem(0,FSKEY_B,FSGUI_MENU_BOOLEAN);
		FsGuiPopUpMenu *boolSubMenu=boolMenu->GetSubMenu();

		boolSubMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_BOOL_UNION,BoolUnion,this);
		boolSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_BOOL_SUBTRACTION,BoolSubtraction,this);
		boolSubMenu->AddTextItem(0,FSKEY_I,FSGUI_MENU_BOOL_INTERSECTION,BoolIntersection,this);
		boolSubMenu->AddTextItem(0,FSKEY_M,FSGUI_MENU_BOOL_MERGE,BoolMerge,this);
		boolSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_BOOL_SEPARATE,BoolSeparate,this);
		boolSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_BOOL_OPENPLUSSOLID,BoolOpenSurfacePlusSolid,this);
		boolSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_BOOL_OPENMINUSSOLID,BoolOpenSurfaceMinusSolid,this);
		boolSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_BOOL_OPENANDSOLID,BoolOpenSurfaceAndSolid,this);
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_SELECT))
	{
		FsGuiPopUpMenuItem *selectMenu=mainMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_SELECT);
		FsGuiPopUpMenu *selectSubMenu=selectMenu->GetSubMenu();

		AddMenuItem(selectSubMenu,FSKEY_P,FSGUI_MENU_SELECT_BYPROP,&THISCLASS::Select_ByProperty);

		selectSubMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_SELECT_ALL)->BindCallBack(&THISCLASS::Select_All,this);
		FsGuiPopUpMenu *select_unselect=nullptr;
		FsGuiPopUpMenu *select_pick=nullptr;
		FsGuiPopUpMenu *select_vertex=nullptr;
		FsGuiPopUpMenu *select_polygon=nullptr;
		FsGuiPopUpMenu *select_constEdge=nullptr;
		FsGuiPopUpMenu *select_faceGroup=nullptr;
		FsGuiPopUpMenu *select_shell=nullptr;

		select_unselect=selectSubMenu->AddTextItem(0,FSKEY_T,FSGUI_MENU_SELECT_UNSELECT)->AddSubMenu();

		if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_PICK))
		{
			select_pick=selectSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_SELECT_PICK)->AddSubMenu();
		}

		auto select_sketch=selectSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_SKETCHSTROKE)->AddSubMenu();

		if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_VERTEX))
		{
			select_vertex=selectSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_SELECT_VERTEX)->AddSubMenu();
		}
		if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_POLYGON))
		{
			select_polygon=selectSubMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_SELECT_POLYGON)->AddSubMenu();
		}
		if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_CONSTEDGE))
		{
			select_constEdge=selectSubMenu->AddTextItem(0,FSKEY_E,FSGUI_MENU_SELECT_CONSTEDGE)->AddSubMenu();
		}
		if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_FACEGROUP))
		{
			select_faceGroup=selectSubMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_SELECT_FACEGROUP)->AddSubMenu();
		}
		if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_SHELL))
		{
			select_shell=selectSubMenu->AddTextItem(0,FSKEY_L,FSGUI_MENU_SELECT_SHELL)->AddSubMenu();
		}

		{
			select_unselect->AddTextItem(0,FSKEY_T,FSGUI_MENU_SELECT_UNSELECT_ALL)->BindCallBack(&THISCLASS::Select_UnselectAll,this);
			BindKeyCallBack(FSKEY_T,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Select_UnselectAll,this);


			if(nullptr!=select_pick)
			{
				if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_VERTEX))
				{
					select_pick->AddTextItem(0,FSKEY_V,FSGUI_MENU_SELECT_PICK_VERTEX)->BindCallBack(&THISCLASS::Select_PickVertex,this);
					BindKeyCallBack(FSKEY_V,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Select_PickVertex,this);
				}
				if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_POLYGON))
				{
					select_pick->AddTextItem(0,FSKEY_F,FSGUI_MENU_SELECT_PICK_POLYGON)->BindCallBack(&THISCLASS::Select_PickPolygon,this);
					BindKeyCallBack(FSKEY_F,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Select_PickPolygon,this);
				}
				if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_CONSTEDGE))
				{
					select_pick->AddTextItem(0,FSKEY_E,FSGUI_MENU_SELECT_PICK_CONSTEDGE)->BindCallBack(&THISCLASS::Select_PickConstEdge,this);
					BindKeyCallBack(FSKEY_E,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Select_PickConstEdge,this);
				}
				if(0!=(mainMenuOpt.selectMenuOption&mainMenuOpt.ADD_SELECT_FACEGROUP))
				{
					select_pick->AddTextItem(0,FSKEY_C,FSGUI_MENU_SELECT_PICK_FACEGROUP)->BindCallBack(&THISCLASS::Select_PickFaceGroup,this);
					BindKeyCallBack(FSKEY_C,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Select_PickFaceGroup,this);
				}
			}
			if(nullptr!=select_sketch)
			{
				select_sketch->AddTextItem(0,FSKEY_F,FSGUI_COMMON_POLYGON)->BindCallBack(&THISCLASS::Select_PolygonByStroke,this);
			}

			if(nullptr!=select_vertex)
			{
				select_vertex->AddTextItem(0,FSKEY_U,FSGUI_MENU_SELECT_VERTEX_UNSELECT)->BindCallBack(&THISCLASS::Select_UnselectVertex,this);
				select_vertex->AddTextItem(0,FSKEY_A,FSGUI_MENU_SELECT_VERTEX_ALL)->BindCallBack(&THISCLASS::Select_AllVertex,this);
				select_vertex->AddTextItem(0,FSKEY_I,FSGUI_MENU_SELECT_VERTEX_INVERT)->BindCallBack(&THISCLASS::Select_InvertVertex,this);
				select_vertex->AddTextItem(0,FSKEY_P,FSGUI_MENU_SELECT_VERTEX_OFSELECTEDPOLYGONS)->BindCallBack(&THISCLASS::Select_VertexOfPolygon,this);
				select_vertex->AddTextItem(0,FSKEY_E,FSGUI_MENU_SELECT_VERTEX_OFSELECTEDCONSTEDGE)->BindCallBack(&THISCLASS::Select_VertexOfConstEdge,this);
				select_vertex->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_VERTEX_FOLLOWBOUNDARY)->BindCallBack(&THISCLASS::Select_VertexAlongBoundaryOrConstEdge,this);
				select_vertex->AddTextItem(0,FSKEY_L,FSGUI_MENU_SELECT_VERTEX_SINGLE_USE_EDGE_LOOP)->BindCallBack(&THISCLASS::Select_Vertex_SingleUseEdgeLoop,this);
				select_vertex->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_VERTEX_USEDBYTWOCONSEDGE)->BindCallBack(&THISCLASS::Select_Vertex_UsedByTwoConstEdge,this);
				select_vertex->AddTextItem(0,FSKEY_1,FSGUI_MENU_SELECT_VERTEX_ONENEIGHBORFROMSELECTION)->BindCallBack(&THISCLASS::Select_Vertex_OneNeighborOfSelection,this);
				BindKeyCallBack(FSKEY_X,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Select_VertexAlongBoundaryOrConstEdge,this);
				BindKeyCallBack(FSKEY_X,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Select_VertexAlongBoundaryOrConstEdge,this);
				AddMenuItem(select_vertex,FSKEY_NULL,FSGUI_MENU_SELECT_VERTEX_EDGECONNECTEDBTWNSELVTHD,&THISCLASS::Select_EdgeConnectedVertexBetweenSelection);
				AddMenuItem(select_vertex,FSKEY_NULL,FSGUI_MENU_SELECT_VERTEX_BYHASHKEY,&THISCLASS::Select_VertexByHashKey);
				AddMenuItem(select_vertex,FSKEY_NULL,FSGUI_MENU_SELECT_VERTEX_BYINDEX,&THISCLASS::Select_VertexByIndex);
			}

			if(nullptr!=select_polygon)
			{
				select_polygon->AddTextItem(0,FSKEY_U,FSGUI_MENU_SELECT_POLYGON_UNSELECT)->BindCallBack(&THISCLASS::Select_UnselectPolygon,this);
				select_polygon->AddTextItem(0,FSKEY_A,FSGUI_MENU_SELECT_POLYGON_ALL)->BindCallBack(&THISCLASS::Select_AllPolygon,this);
				select_polygon->AddTextItem(0,FSKEY_I,FSGUI_MENU_SELECT_POLYGON_INVERT)->BindCallBack(&THISCLASS::Select_InvertPolygon,this);
				select_polygon->AddTextItem(0,FSKEY_C,FSGUI_MENU_SELECT_POLYGON_OFSELECTEDFACEGROUP)->BindCallBack(&THISCLASS::Select_PolygonOfFaceGroup,this);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_EDGECONNECTED)->BindCallBack(&THISCLASS::Select_PolygonConnected,this);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_USINGATLEASTONESELECTEDVERTEX)->BindCallBack(&THISCLASS::Select_PolygonUsingAtLeastOneOfSelectedVertex,this);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_SHORTESTPATH)->BindCallBack(&THISCLASS::Select_PolygonShortestPath,this);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_FROMEDGE)->BindCallBack(&THISCLASS::Select_PolygonFromEdge,this);
				AddMenuItem(select_polygon,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_TANGLED,&THISCLASS::SelectTangledPolygon);
				AddMenuItem(select_polygon,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_ZERONORMAL,&THISCLASS::SelectZeroNormalPolygon);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_NONTRIANGULAR)->BindCallBack(&THISCLASS::SelectNonTriangularPolygon,this);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_SAMECOLORFROMCURRENTSELECTION)->BindCallBack(&THISCLASS::Select_PolygonSameColorFromCurrentSelection,this);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_NARROW)->BindCallBack(&THISCLASS::Select_Polygon_Narrow,this);
				select_polygon->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_POLYGON_CANNOT_COMPUTE_NORMAL)->BindCallBack(&THISCLASS::Select_Polygon_CannotCalculateNormal,this);
			}

			if(nullptr!=select_constEdge)
			{
				select_constEdge->AddTextItem(0,FSKEY_A,FSGUI_MENU_SELECT_CONSTEDGE_ALL)->BindCallBack(&THISCLASS::Select_ConstEdge_All,this);
				select_constEdge->AddTextItem(0,FSKEY_I,FSGUI_MENU_SELECT_CONSTEDGE_INVERT)->BindCallBack(&THISCLASS::Select_ConstEdge_InvertSelection,this);
				select_constEdge->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_CONSTEDGE_DIVIDINGSMOOTHSHADINGREGION)->BindCallBack(&THISCLASS::Select_ConstEdge_DividingSmoothShadingRegion,this);
				select_constEdge->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_SELECT_CONSTEDGE_FROMPOLYGON)->BindCallBack(&THISCLASS::Select_ConstEdge_FromPolygon,this);
			}

			if(nullptr!=select_faceGroup)
			{
				select_faceGroup->AddTextItem(0,FSKEY_A,FSGUI_MENU_SELECT_FACEGROUP_ALL)->BindCallBack(&THISCLASS::Select_AllFaceGroup,this);
				select_faceGroup->AddTextItem(0,FSKEY_I,FSGUI_MENU_SELECT_FACEGROUP_INVERT)->BindCallBack(&THISCLASS::Select_InvertFaceGroup,this);
				AddMenuItem(select_faceGroup,FSKEY_P,FSGUI_MENU_SELECT_FACEGROUP_PLANARFACEGROUP,&THISCLASS::Select_FaceGroup_PlanarFaceGroup);
				select_faceGroup->AddTextItem(0,FSKEY_D,FSGUI_MENU_SELECT_FACEGROUP_HIGHDHA)->BindCallBack(&THISCLASS::Select_FaceGroupWithHighDihedralAngle,this);
				select_faceGroup->AddTextItem(0,FSKEY_C,FSGUI_MENU_SELECT_FACEGROUP_CONNECTED)->BindCallBack(&THISCLASS::Select_FaceGroup_Connected,this);
			}

			if(nullptr!=select_shell)
			{
				select_shell->AddTextItem(0,FSKEY_C,FSGUI_MENU_SELECT_SHELL_PICKCURRENTSHELL)->BindCallBack(&THISCLASS::Select_CurrentShellByPicking,this);
				BindKeyCallBack(FSKEY_S,YSTRUE,YSFALSE,YSFALSE,&THISCLASS::Select_CurrentShellByPicking,this);

				select_shell->AddTextItem(0,FSKEY_N,FSGUI_MENU_SELECT_SHELL_NEXTSHELL)->BindCallBack(&THISCLASS::Select_NextShell,this);
				BindKeyCallBack(FSKEY_TAB,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Select_NextShell,this);

				select_shell->AddTextItem(0,FSKEY_P,FSGUI_MENU_SELECT_SHELL_PREVSHELL)->BindCallBack(&THISCLASS::Select_PrevShell,this);
				BindKeyCallBack(FSKEY_TAB,YSTRUE,YSTRUE,YSFALSE,&THISCLASS::Select_PrevShell,this);

				select_shell->AddTextItem(0,FSKEY_D,FSGUI_MENU_SELECT_SHELL_BYDIALOG)->BindCallBack(&THISCLASS::Select_Shell_ByDialog,this);
			}
		}
	}



	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_SKETCHIF))
	{
		auto sketchSubMenu=mainMenu->AddTextItem(0,FSKEY_K,FSGUI_MENU_SKETCH)->GetSubMenu();
		{
			sketchSubMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_SKETCH_CREATECONSTEDGE,Sketch_CreateConstEdgeBySketch,this);
		}
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_VIEW))
	{
		FsGuiPopUpMenuItem *viewMenu=mainMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_VIEW);
		FsGuiPopUpMenu *viewSubMenu=viewMenu->GetSubMenu();

		AddMenuItem(viewSubMenu,FSKEY_NULL,FSGUI_MENU_VIEW_REFRESHBUFFERS,&THISCLASS::View_RefreshBuffer);
		viewSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_RESETVIEWRANGE)->BindCallBack(&THISCLASS::View_ResetViewRange,this);
		viewSubMenu->AddTextItem(mainMenu->MkId("view/resetviewoption"),FSKEY_NULL,FSGUI_MENU_VIEW_RESETVIEWOPTION)->BindCallBack(&THISCLASS::View_ResetViewOption,this);
		viewDrawAxis=         viewSubMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_VIEW_DRAWAXES);
		viewDrawAxis->BindCallBack(&THISCLASS::View_DrawAxis,this);
		viewDrawVertex=       viewSubMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_VIEW_DRAWVERTEX);
		viewDrawVertex->BindCallBack(&THISCLASS::View_DrawVertex,this);
		viewDrawConstEdge=    viewSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_VIEW_DRAWCONSTEDGE);
		viewDrawConstEdge->BindCallBack(&THISCLASS::View_DrawConstEdge,this);
		viewDrawPolygonFill=  viewSubMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_VIEW_DRAWPOLYGONFILL);
		viewDrawPolygonFill->BindCallBack(&THISCLASS::View_DrawPolygonFill,this);
		viewDrawWireframe=    viewSubMenu->AddTextItem(0,FSKEY_E,FSGUI_MENU_VIEW_DRAWPOLYGONEDGES);
		viewDrawWireframe->BindCallBack(&THISCLASS::View_DrawWireframe,this);
		viewDrawShrunkPolygon=viewSubMenu->AddTextItem(0,FSKEY_H,FSGUI_MENU_VIEW_DRAWSHURNKPOLYGONS);
		viewDrawShrunkPolygon->BindCallBack(&THISCLASS::View_DrawShrunkPolygon,this);
		viewDrawBackFaceInDifferentColor=viewSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_DRAWBACKFACEDIFFCOLOR);
		viewDrawBackFaceInDifferentColor->BindCallBack(&THISCLASS::View_DrawBackFaceInDifferentColor,this);
		viewDrawBackgroundGradation=viewSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_DRAWBACKGROUNDGRADATION);
		viewDrawBackgroundGradation->BindCallBack(&THISCLASS::View_DrawBackgroundGradation,this);
		viewDrawBackgroundGradation->SetCheck(YSTRUE);

		if(0!=(mainMenuOpt.viewMenuOption&&mainMenuOpt.ADD_VIEW_MULTIMODEL))
		{
			auto viewDrawOtherShellMenu=viewSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_VIEW_DRAWOTHERSHELL)->AddSubMenu();
			viewDrawOtherShellWire=viewDrawOtherShellMenu->AddTextItem(mainMenu->MkId("view/drawothershell/wireframe"),FSKEY_W,FSGUI_MENU_VIEW_DRAWOTHERSHELL_WIREFRAME);
			viewDrawOtherShellWire->BindCallBack(&THISCLASS::View_DrawOtherShellWireframe,this);
			viewDrawOtherShellFill=viewDrawOtherShellMenu->AddTextItem(mainMenu->MkId("view/drawothershell/fill"),FSKEY_F,FSGUI_MENU_VIEW_DRAWOTHERSHELL_FILL);
			viewDrawOtherShellFill->BindCallBack(&THISCLASS::View_DrawOtherShellFill,this);
			viewDrawOtherShellNone=viewDrawOtherShellMenu->AddTextItem(mainMenu->MkId("view/drawothershell/dontdraw"),FSKEY_N,FSGUI_MENU_VIEW_DRAWOTHERSHELL_DONTDRAW);
			viewDrawOtherShellNone->BindCallBack(&THISCLASS::View_DrawOtherShellNone,this);
		}

		if(0!=(mainMenuOpt.viewMenuOption&&mainMenuOpt.ADD_VIEW_DNM))
		{
			viewDrawDnmConnection=viewSubMenu->AddTextItem(mainMenu->MkId("view/drawdnmconnection"),FSKEY_NULL,FSGUI_MENU_VIEW_DRAWDNMCONNECTION);
			viewDrawDnmConnection->BindCallBack(&THISCLASS::View_DrawDnmConnection,this);
		}

		FsGuiPopUpMenuItem *view_focusOn=viewSubMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_VIEW_LOOKAT);
		FsGuiPopUpMenuItem *view_viewPort=viewSubMenu->AddTextItem(0,FSKEY_W,FSGUI_MENU_VIEW_VIEWPORT);
		FsGuiPopUpMenuItem *view_crossSection=viewSubMenu->AddTextItem(0,FSKEY_X,FSGUI_MENU_VIEW_CROSSSECTION);
		FsGuiPopUpMenuItem *view_refBmp=viewSubMenu->AddTextItem(0,FSKEY_B,FSGUI_MENU_VIEW_REFBMP);
		FsGuiPopUpMenuItem *view_highlight=viewSubMenu->AddTextItem(0,FSKEY_L,FSGUI_MENU_VIEW_HIGHLIGHT);
		{
			view_focusOn->AddSubMenu();

			view_focusOn->GetSubMenu()->AddTextItem(0,FSKEY_S,FSGUI_MENU_VIEW_LOOKAT_SELECTIONCG,View_FocusOn_SelectionCenterOfGravity,this);
			view_focusOn->GetSubMenu()->AddTextItem(0,FSKEY_C,FSGUI_MENU_VIEW_LOOKAT_SELECTIONBB,View_FocusOn_SelectionBoundingBoxCenter,this);
			view_focusOn->GetSubMenu()->AddTextItem(0,FSKEY_V,FSGUI_MENU_VIEW_LOOKAT_PICKEDVERTEX,View_FocusOn_Vertex,this);
			AddShortCut(FSKEY_R,YSFALSE,YSFALSE,View_FocusOn_Vertex_RKey);
			view_focusOn->GetSubMenu()->AddTextItem(0,FSKEY_F,FSGUI_MENU_VIEW_LOOKAT_POLYGON,View_FocusOn_Polygon,this);
			AddShortCut(FSKEY_R,YSTRUE,YSFALSE,View_FocusOn_Polygon_ShiftRKey);
			view_focusOn->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_LOOKAT_POLYGONSTRAIGHT,View_FocusOn_PolygonStraight,this);
			AddShortCut(FSKEY_R,YSTRUE,YSTRUE,View_FocusOn_Polygon_ShiftCtrlRKey);

			view_viewPort->AddSubMenu();

			viewEnableViewPort=view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_E,FSGUI_MENU_VIEW_VIEWPORT_ENABLE,View_EnableViewPort,this);
			viewUseSameViewPortForAll=view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_M,FSGUI_MENU_VIEW_VIEWPORT_USESAMEVIEWPORTFORALL,View_UseSameViewPortForAllShell,this);
			view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_R,FSGUI_MENU_VIEW_VIEWPORT_RESET,View_ResetViewPort,this);
			view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_V,FSGUI_MENU_VIEW_VIEWPORT_MOVE,View_MoveViewPort,this);
			view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_C,FSGUI_MENU_VIEW_VIEWPORT_MOVETOROTATIONCENTER,View_MoveViewPortToRotationCenter,this);
			view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_X,FSGUI_MENU_VIEW_VIEWPORT_EXPAND,View_ExpandViewPort,this);
			AddShortCut(FSKEY_P,YSFALSE,YSFALSE,View_ExpandViewPort);
			view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_S,FSGUI_MENU_VIEW_VIEWPORT_SHRINK,View_ShrinkViewPort,this);
			AddShortCut(FSKEY_P,YSTRUE,YSFALSE,View_ShrinkViewPort);
			view_viewPort->GetSubMenu()->AddTextItem(0,FSKEY_F,FSGUI_MENU_VIEW_VIEWPORT_FROM_SELECTION,View_ViewPortFromSelection,this);

			view_crossSection->AddSubMenu();
			viewEnableCrossSection=view_crossSection->GetSubMenu()->AddTextItem(0,FSKEY_E,FSGUI_MENU_VIEW_CROSSSECTION_ENABLE,View_CrossSection_Enable,this);
			view_crossSection->GetSubMenu()->AddTextItem(0,FSKEY_R,FSGUI_MENU_VIEW_CROSSSECTION_RESET,View_CrossSection_Reset,this);
			view_crossSection->GetSubMenu()->AddTextItem(0,FSKEY_V,FSGUI_MENU_VIEW_CROSSSECTION_MOVE,View_CrossSection_Move,this);
			view_crossSection->GetSubMenu()->AddTextItem(0,FSKEY_I,FSGUI_MENU_VIEW_CROSSSECTION_INVERT,View_CrossSection_Invert,this);

			view_refBmp->AddSubMenu();
			view_refBmp->GetSubMenu()->AddTextItem(0,FSKEY_D,FSGUI_MENU_VIEW_REFBMP_OPENDIALOG,View_OpenRefBmpDialog,this);
			view_refBmp->GetSubMenu()->AddTextItem(0,FSKEY_L,FSGUI_MENU_VIEW_REFBMP_LOADCONFIG,View_LoadRefBmpConfig,this);
			view_refBmp->GetSubMenu()->AddTextItem(0,FSKEY_S,FSGUI_MENU_VIEW_REFBMP_SAVECONFIG,View_SaveRefBmpConfig,this);
			view_refBmp->GetSubMenu()->AddTextItem(0,FSKEY_W,FSGUI_MENU_VIEW_REFBMP_SHOWALL,View_ShowAllRefBmp,this);
			view_refBmp->GetSubMenu()->AddTextItem(0,FSKEY_E,FSGUI_MENU_VIEW_REFBMP_HIDEALL,View_HideAllRefBmp,this);

			view_highlight->AddSubMenu();
			viewShowHighlight=AddMenuItem(view_highlight->GetSubMenu(),FSKEY_S,FSGUI_MENU_VIEW_HIGHLIGHT_SHOWHIGHLIGHT,&THISCLASS::View_Highlight_ShowHighlight);
			viewHighlightNonManifoldEdge=view_highlight->GetSubMenu()->AddTextItem(0,FSKEY_N,FSGUI_MENU_VIEW_HIGHLIGHT_NONMANIFOLDEDGES,View_HighlightNonManifoldEdge,this);
			view_highlight->GetSubMenu()->AddTextItem(0,FSKEY_I,FSGUI_MENU_VIEW_HIGHLIGHT_SELFINTERSECTINGPOLYGONS,View_HighlightSelfIntersectingPolygon,this);
			view_highlight->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_HIGHLIGHT_HIGHDHAWITHINFACE)->BindCallBack(&THISCLASS::View_Highlight_HighDihedralAngleEdgeWithinFaceGroup,this);
			AddMenuItem(view_highlight->GetSubMenu(),FSKEY_H,FSGUI_MENU_VIEW_HIGHLIGHT_HIGHASPECTRATIOTRIANDQUAD,&THISCLASS::View_Highlight_HighAspectRatioTriangleAndQuadrilateral);
			view_highlight->GetSubMenu()->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_HIGHLIGHT_INCONSISTENT_ORIENTATION_ACROSS_EDGE)->BindCallBack(&THISCLASS::View_Highlight_InconsistentOrientationAcrossEdge,this);
		}
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_GLOBAL))
	{
		FsGuiPopUpMenu *globalSubMenu=mainMenu->AddTextItem(0,FSKEY_G,FSGUI_MENU_GLOBAL)->GetSubMenu();

		FsGuiPopUpMenu *globalTargetSubMenu=globalSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_GLOBAL_TARGET)->AddSubMenu();
		globalTargetCurrentShellOnly=AddMenuItem(globalTargetSubMenu,FSKEY_C,FSGUI_MENU_GLOBAL_TARGET_CURRENT,&THISCLASS::Global_Target_Current);
		globalTargetAllShell=AddMenuItem(globalTargetSubMenu,FSKEY_A,FSGUI_MENU_GLOBAL_TARGET_ALL,&THISCLASS::Global_Target_All);
		globalTargetCurrentShellOnly->SetCheck(YSTRUE);

		globalSubMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_GLOBAL_DELETEUNUSED)->BindCallBack(&THISCLASS::Global_DeleteUnusedVertexAll,this);
		globalSubMenu->AddTextItem(0,FSKEY_I,FSGUI_MENU_GLOBAL_INVERTPOLYGON,Global_InvertPolygonAll,this);
		globalSubMenu->AddTextItem(0,FSKEY_Z,FSGUI_MENU_GLOBAL_ZERONORMAL,Global_SetZeroNormalAll,this);
		globalSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_GLOBAL_REORIENT,Global_FixOrientationAndNormalClosedVolume,this);
		globalSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_GLOBAL_REORIENT_ASSIGNED_NORMAL)->BindCallBack(&THISCLASS::Global_FixOrientationBasedOnAssignedNormal,this);
		globalSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_GLOBAL_RENORMAL_CURRENT_ORIENTATION)->BindCallBack(&THISCLASS::Global_FixNormalBasedOnOrientation,this);
		globalSubMenu->AddTextItem(0,FSKEY_T,FSGUI_MENU_GLOBAL_TRIANGULATE,Global_TriangulateAll,this);
		globalSubMenu->AddTextItem(0,FSKEY_M,FSGUI_MENU_GLOBAL_MERGESAMEVERTEX,Global_DeleteRedundantVertex,this);
		globalSubMenu->AddTextItem(0,FSKEY_G,FSGUI_MENU_GLOBAL_MERGESAMEVERTEXONNONMANIFOLDEDGE,Global_DeleteRedundantVertexNonManifoldEdgeOnly,this);
		globalSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_GLOBAL_SCALING)->BindCallBack(&THISCLASS::Global_Scaling,this);
		globalSubMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_GLOBAL_SCALEINDIRECTION)->BindCallBack(&THISCLASS::Global_Scale_InDirection,this);
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_UTIL))
	{
		FsGuiPopUpMenu *utilSubMenu=mainMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_UTILITY)->GetSubMenu();

		{
			FsGuiPopUpMenu *constEdgeSubMenu=utilSubMenu->AddTextItem(0,FSKEY_E,FSGUI_MENU_UTILITY_CONSTEDGE)->AddSubMenu();

			constEdgeSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_UTILITY_CONSTEDGE_SETNAME)->BindCallBack(&THISCLASS::UtilMenu_SetConstEdgeName,this);
			constEdgeSubMenu->AddTextItem(mainMenu->MkId("util/constEdge/clear"),   FSKEY_NULL,FSGUI_MENU_UTILITY_CONSTEDGE_CLEAR,UtilMenu_ClearConstEdgeAll,this);
			constEdgeSubMenu->AddTextItem(mainMenu->MkId("util/constEdge/constAll"),FSKEY_A,FSGUI_MENU_UTILITY_CONSTEDGE_CONSTRAINALL,UtilMenu_ConstrainAllEdges,this);
			constEdgeSubMenu->AddTextItem(0,FSKEY_D,FSGUI_MENU_UTILITY_CONSTEDGE_ADDBYDHA,UtilMenu_AddConstEdgeByDihedralAngle,this);
			constEdgeSubMenu->AddTextItem(0,FSKEY_N,FSGUI_MENU_UTILITY_CONSTEDGE_ADDNONMANIFOLD,UtilMenu_AddConstEdgeAlongNonManifoldEdge,this);
			constEdgeSubMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_UTILITY_CONSTEDGE_ADDFGBOUNDARY,UtilMenu_AddConstEdgeOnFaceGroupBoundary,this);
			constEdgeSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_UTILITY_CONSTEDGE_ADDONCOLORBOUNDARY)->BindCallBack(&THISCLASS::UtilMenu_AddConstEdgeAlongColorBoundary,this);
			constEdgeSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_UTILITY_CONSTEDGE_OPENCLOSELOOP,UtilMenu_OpenCloseConstEdge,this);
			AddMenuItem(constEdgeSubMenu,FSKEY_R,FSGUI_MENU_UTILITY_CONSTEDGE_RECONSIDER,&THISCLASS::UtilMenu_ReconsiderConstEdge);
		}
		{
			FsGuiPopUpMenu *faceGroupSubMenu=utilSubMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_UTILITY_FACEGROUP)->AddSubMenu();
			faceGroupSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_UTILITY_FACEGROUP_CLEARALL,UtilMenu_ClearFaceGroupAll,this);
			faceGroupSubMenu->AddTextItem(mainMenu->MkId("util/faceGroup/fromConstEdge"),FSKEY_C,FSGUI_MENU_UTILITY_FACEGROUP_FROM_CONSTEDGE,UtilMenu_MakeFaceGroupFromConstEdge,this);
			faceGroupSubMenu->AddTextItem(0,FSKEY_P,FSGUI_MENU_UTILITY_PAINT_BASED_ON_FACEGROUP)->BindCallBack(&THISCLASS::UtilMenu_PaintBasedOnFaceGroup,this);
			faceGroupSubMenu->AddTextItem(0,FSKEY_N,FSGUI_MENU_UTILITY_FACEGROUP_SETNAME,UtilMenu_FaceGroup_SetName,this);
			faceGroupSubMenu->AddTextItem(0,FSKEY_X,FSGUI_MENU_UTILITY_FACEGROUP_EXPAND_TO_UNASSIGNED)->BindCallBack(&THISCLASS::UtilMenu_ExpandCurrentFaceGroupToUnassignedPolygon,this);
		}
		{
			FsGuiPopUpMenu *repairSubMenu=utilSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_UTILITY_REPAIR)->AddSubMenu();
			repairSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_UTILITY_REPAIR_FREESTITCH)->BindCallBack(&THISCLASS::RepairMenu_FreeStitching,this);
			repairSubMenu->AddTextItem(0,FSKEY_L,FSGUI_MENU_UTILITY_REPAIR_LOOPSTITCH)->BindCallBack(&THISCLASS::RepairMenu_LoopStitching,this);
			repairSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_UTILITY_REPAIR_DELETE_IDENTICALPOLYGON)->BindCallBack(&THISCLASS::RepairMenu_RemoveIdenticalPolygon,this);
		}
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_DNM))
	{
		FsGuiPopUpMenuItem *dnmMenu=mainMenu->AddTextItem(0,FSKEY_D,FSGUI_MENU_DYNAMICMODEL);
		auto dnmSubMenu=dnmMenu->GetSubMenu();

		dnmSubMenu->AddTextItem(0,FSKEY_D,FSGUI_MENU_DNM_DNMEDITMODE)->BindCallBack(&THISCLASS::Dnm_DnmEditMode,this);
		dnmSubMenu->AddTextItem(0,FSKEY_T,FSGUI_MENU_DNM_EDITTREESTRUCTURE)->BindCallBack(&THISCLASS::Dnm_EditTreeStructure,this);
		dnmSubMenu->AddTextItem(0,FSKEY_L,FSGUI_MENU_DNM_SETNAME)->BindCallBack(&THISCLASS::Dnm_SetDnmName,this);
		dnmSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_DNM_LOADSTATICMODE)->SetCallBack(std::bind(&THISCLASS::Dnm_LoadStaticMode,this));
		dnmSubMenu->AddTextItem(0,FSKEY_0,FSGUI_MENU_DNM_LOADSTATE0)->SetCallBack(std::bind(&THISCLASS::Dnm_LoadState0,this));
		dnmSubMenu->AddTextItem(0,FSKEY_1,FSGUI_MENU_DNM_LOADSTATE1)->SetCallBack(std::bind(&THISCLASS::Dnm_LoadState1,this));
		dnmSubMenu->AddTextItem(0,FSKEY_2,FSGUI_MENU_DNM_LOADSTATE2)->SetCallBack(std::bind(&THISCLASS::Dnm_LoadState2,this));
		dnmSubMenu->AddTextItem(0,FSKEY_3,FSGUI_MENU_DNM_LOADSTATE3)->SetCallBack(std::bind(&THISCLASS::Dnm_LoadState3,this));
		dnmSubMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_DNM_PREVIEW)->SetCallBack(std::bind(&THISCLASS::Dnm_Preview,this));

		auto editStateSubMenu=dnmSubMenu->AddTextItem(0,FSKEY_E,FSGUI_MENU_DNM_EDITSTATE)->AddSubMenu();
		editStateSubMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_DNM_SETPIVOT)->BindCallBack(&THISCLASS::Dnm_SetPivot,this);
		editStateSubMenu->AddTextItem(0,FSKEY_X,FSGUI_MENU_DNM_SETAXIS)->BindCallBack(&THISCLASS::Dnm_SetRotationAxis,this);
		editStateSubMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_DNM_ROTATE)->BindCallBack(&THISCLASS::Dnm_Rotate,this);
		editStateSubMenu->AddTextItem(0,FSKEY_T,FSGUI_MENU_DNM_TRANSLATE)->BindCallBack(&THISCLASS::Dnm_Translate,this);
		editStateSubMenu->AddTextItem(mainMenu->MkId("dnm/editstate/show"),FSKEY_NULL,FSGUI_MENU_DNM_SHOW)->BindCallBack(&THISCLASS::Dnm_Show,this);
		editStateSubMenu->AddTextItem(mainMenu->MkId("dnm/editstate/hide"),FSKEY_NULL,FSGUI_MENU_DNM_HIDE)->BindCallBack(&THISCLASS::Dnm_Hide,this);
		editStateSubMenu->AddTextItem(mainMenu->MkId("dnm/editstate/auto"),FSKEY_NULL,FSGUI_MENU_DNM_AUTO)->BindCallBack(&THISCLASS::Dnm_AutoSetState,this);
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_OPTION))
	{
		FsGuiPopUpMenuItem *optionMenu=mainMenu->AddTextItem(0,FSKEY_O,FSGUI_MENU_OPTION);
		FsGuiPopUpMenu *optionSubMenu=optionMenu->GetSubMenu();

		optionSubMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_OPTION_CONFIG)->BindCallBack(&THISCLASS::Option_Config,this);
	}


	if(0!=(mainMenuOpt.menuOption&mainMenuOpt.ADD_HELP))
	{
		auto helpMenu=mainMenu->AddTextItem(0,FSKEY_H,FSGUI_MENU_HELP)->GetSubMenu();
		helpMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_HELP_ABOUT)->BindCallBack(&THISCLASS::Help_About,this);
		helpMenu->AddTextItem(0,FSKEY_H,FSGUI_MENU_HELP_HELP)->BindCallBack(&THISCLASS::Help_Help,this);
	}


	guiExtension->AddMenu(mainMenu);


	YsColor bgCol=mainMenu->GetBackgroundColor();
	bgCol.SetAi(200);
	mainMenu->SetBackgroundColor(bgCol);

	int wid,hei;
	FsGetWindowSize(wid,hei);
	mainMenu->FitPullDownMenu(wid);

	SetMainMenu(mainMenu);
}



void GeblGuiEditorBase::SetMenuCheck(void)
{
	FsGuiPopUpMenuItem::SetCheck(viewDrawAxis,drawAxis);
	FsGuiPopUpMenuItem::SetCheck(viewDrawVertex,drawVertex);
	FsGuiPopUpMenuItem::SetCheck(viewDrawConstEdge,drawConstEdge);
	FsGuiPopUpMenuItem::SetCheck(viewDrawPolygonFill,drawPolygon);
	FsGuiPopUpMenuItem::SetCheck(viewDrawWireframe,drawPolygonEdge);
	FsGuiPopUpMenuItem::SetCheck(viewDrawShrunkPolygon,drawShrunkPolygon);
	FsGuiPopUpMenuItem::SetCheck(viewDrawBackFaceInDifferentColor,drawBackFaceInDifferentColor);
	FsGuiPopUpMenuItem::SetCheck(viewHighlightNonManifoldEdge,drawNonManifoldEdge);
	FsGuiPopUpMenuItem::SetCheck(viewUseSameViewPortForAll,drawApplyViewPortToAllShell);
	FsGuiPopUpMenuItem::SetCheck(viewShowHighlight,drawHighlight);

	FsGuiPopUpMenuItem::SetCheck(viewDrawOtherShellWire,drawOtherShellWireframe);
	FsGuiPopUpMenuItem::SetCheck(viewDrawOtherShellFill,drawOtherShellFill);
	if(YSTRUE!=drawOtherShellWireframe && YSTRUE!=drawOtherShellFill)
	{
		FsGuiPopUpMenuItem::SetCheck(viewDrawOtherShellNone,YSTRUE);
	}
	else
	{
		FsGuiPopUpMenuItem::SetCheck(viewDrawOtherShellNone,YSFALSE);
	}

	FsGuiPopUpMenuItem::SetCheck(viewDrawDnmConnection,drawDnmConnection);

	if(NULL!=slHd)
	{
		FsGuiPopUpMenuItem::SetCheck(viewEnableViewPort,slHd->GetDrawingBuffer().viewPortEnabled);
		FsGuiPopUpMenuItem::SetCheck(viewEnableCrossSection,slHd->GetDrawingBuffer().CrossSectionEnabled());
	}
}

void GeblGuiEditorBase::SetDefaultViewOption(void)
{
	drawAxis=YSTRUE;
	drawVertex=YSTRUE;
	drawConstEdge=YSTRUE;
	drawPolygonEdge=YSTRUE;
	drawPolygon=YSTRUE;
	drawShrunkPolygon=YSTRUE;
	drawBackFaceInDifferentColor=YSTRUE;
	drawApplyViewPortToAllShell=YSTRUE;
	drawOtherShell=YSTRUE;
	drawNonManifoldEdge=YSFALSE;
	drawHighlight=YSTRUE;

	drawOtherShellWireframe=YSTRUE;
	drawOtherShellFill=YSFALSE;

	drawDnmConnection=YSTRUE;
}
