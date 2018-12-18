/* ////////////////////////////////////////////////////////////

File Name: menusetup.cpp
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


#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>

#include "fsgui3dapp.h"
#include "textresource.h"



void FsGui3DMainCanvas::MakeMainMenu(void)
{
	mainMenu=new FsGuiPopUpMenu;
	mainMenu->Initialize();
	mainMenu->SetIsPullDownMenu(YSTRUE);

	{
		auto *fileMenu=mainMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_FILE)->GetSubMenu();

		fileMenu->AddTextItem(0,FSKEY_O,FSGUI_MENU_FILE_OPEN)->BindCallBack(&THISCLASS::File_Open,this);
		fileMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_FILE_SAVE)->BindCallBack(&THISCLASS::File_Save,this);
		fileMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_FILE_SAVEAS)->BindCallBack(&THISCLASS::File_SaveAs,this);
		fileMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_FILE_EXPORT_AS_BITMAP)->BindCallBack(&THISCLASS::File_SaveAsOneBitmap,this);
		fileMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_FILE_EXPORT_AS_BITMAP_ALL_FIELD)->BindCallBack(&THISCLASS::File_SaveAsOneBitmapForEachField,this);
		fileMenu->AddTextItem(0,FSKEY_X,FSGUI_MENU_FILE_EXIT)->BindCallBack(&THISCLASS::File_Exit,this);
		fileRecent=fileMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_FILE_RECENT)->AddSubMenu();
	}

	{
		auto *editMenu=mainMenu->AddTextItem(0,FSKEY_E,FSGUI_MENU_EDIT)->GetSubMenu();
		editMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_CLEAR_UI_MODE)->BindCallBack(&THISCLASS::Edit_ClearUIMode,this);
		editMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_EDIT_UNDO)->BindCallBack(&THISCLASS::Edit_Undo,this);
		editMenu->AddTextItem(0,FSKEY_R,FSGUI_MENU_EDIT_REDO)->BindCallBack(&THISCLASS::Edit_Redo,this);
		editMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_EDIT_COPY)->BindCallBack(&THISCLASS::Edit_Copy,this);
		editMenu->AddTextItem(0,FSKEY_P,FSGUI_MENU_EDIT_PASTE)->BindCallBack(&THISCLASS::Edit_Paste,this);
		editMenu->AddTextItem(0,FSKEY_T,FSGUI_MENU_EDIT_TRIM)->BindCallBack(&THISCLASS::Edit_Trim,this);
		editMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_SCALING)->BindCallBack(&THISCLASS::Edit_Scaling,this);
		editMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_EDIT_DELETE)->BindCallBack(&THISCLASS::Edit_Delete,this);
		{
			// auto editInsertMenu=editMenu->AddTextItem(0,FSKEY_I,FSGUI_MENU_EDIT_INSERT)->AddSubMenu(); no longer used
		}
		editMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_EDIT_SEARCH)->BindCallBack(&THISCLASS::Edit_Search,this);

		BindKeyCallBack(FSKEY_ESC,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_ClearUIMode,this);
		BindKeyCallBack(FSKEY_DEL,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Edit_Delete,this);
		BindKeyCallBack(FSKEY_Z,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Undo,this);
		BindKeyCallBack(FSKEY_Y,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Redo,this);
		BindKeyCallBack(FSKEY_C,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Copy,this);
		BindKeyCallBack(FSKEY_V,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Edit_Paste,this);
	}

	{
		auto *selectMenu=mainMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_SELECT)->GetSubMenu();
		selectMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_SELECT_SELECTALL)->BindCallBack(&THISCLASS::Select_SelectAll,this);
		selectMenu->AddTextItem(0,FSKEY_T,FSGUI_MENU_SELECT_UNSELECTALL)->BindCallBack(&THISCLASS::Select_UnselectAll,this);

		BindKeyCallBack(FSKEY_T,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::Select_UnselectAll,this);
	}

	{
		auto *viewMenu=mainMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_VIEW)->GetSubMenu();
		view_showPositionReferenceMarker=viewMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_SHOW_POSITION_REFERENCE_MARKER);
		view_showPositionReferenceMarker->BindCallBack(&THISCLASS::View_ShowPositionReferenceMarker,this);
		viewMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_MOVE_POSITION_REFERENCE_MARKER)->BindCallBack(&THISCLASS::View_MovePositionReferenceMarker,this);
		viewMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_VIEW_FIT)->BindCallBack(&THISCLASS::View_Fit,this);

		auto lookAtSubMenu=viewMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_LOOKAT)->AddSubMenu();
		lookAtSubMenu->AddTextItem(0,FSKEY_S,FSGUI_MENU_LOOKAT_SELECTION)->BindCallBack(&THISCLASS::View_LookAtSelection,this);
		BindKeyCallBack(FSKEY_HOME,YSFALSE,YSFALSE,YSFALSE,&THISCLASS::View_LookAtSelection,this);

		view_restoreViewWhenSwitchingField=viewMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_RESTORE_VIEW_PER_FIELD);
		view_restoreViewWhenSwitchingField->BindCallBack(&THISCLASS::View_RestoreViewWhenSwitchingField,this);
		view_restoreViewWhenSwitchingField->SetCheck(YSTRUE);

		view_showMarkUp=viewMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_VIEW_DRAW_MARKUP);
		view_showMarkUp->BindCallBack(&THISCLASS::View_ShowMarkUp,this);
		view_showMarkUp->SetCheck(YSTRUE);
	}

	{
		auto *worldMenu=mainMenu->AddTextItem(0,FSKEY_W,FSGUI_MENU_WORLD)->GetSubMenu();
		worldMenu->AddTextItem(0,FSKEY_P,FSGUI_COMMON_PROPERTIES)->BindCallBack(&THISCLASS::World_Property,this);
		worldMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_WORLD_SETUNIT)->BindCallBack(&THISCLASS::World_SetUnit,this);
	}

	{
		auto *fieldMenu=mainMenu->AddTextItem(0,FSKEY_L,FSGUI_MENU_FIELD)->GetSubMenu();
		fieldMenu->AddTextItem(0,FSKEY_P,FSGUI_COMMON_PROPERTIES)->BindCallBack(&THISCLASS::Field_Property,this);
		fieldMenu->AddTextItem(0,FSKEY_N,FSGUI_MENU_FIELD_NEWFIELD)->BindCallBack(&THISCLASS::Field_NewField,this);
		fieldMenu->AddTextItem(0,FSKEY_NULL,FSGUI_COMMON_DELETE)->BindCallBack(&THISCLASS::Field_DeleteField,this);
		fieldMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_FIELD_CHANGE_ORDER)->BindCallBack(&THISCLASS::Field_ChangeOrder,this);
		fieldMenu->AddTextItem(0,FSKEY_X,FSGUI_MENU_FIELD_NEXTFIELD)->BindCallBack(&THISCLASS::Field_NextField,this);
		fieldMenu->AddTextItem(0,FSKEY_V,FSGUI_MENU_FIELD_PREVFIELD)->BindCallBack(&THISCLASS::Field_PrevField,this);

		BindKeyCallBack(FSKEY_TAB,YSFALSE,YSTRUE,YSFALSE,&THISCLASS::Field_NextField,this);
		BindKeyCallBack(FSKEY_TAB,YSTRUE ,YSTRUE,YSFALSE,&THISCLASS::Field_PrevField,this);
	}

	{
		auto mapPieceMenu=mainMenu->AddTextItem(0,FSKEY_M,FSGUI_MENU_MAPPIECE)->GetSubMenu();
		// Insert Image is transferred from file menu.
		mapPieceMenu->AddTextItem(0,FSKEY_I,FSGUI_MENU_FILE_INSERT_IMAGE)->BindCallBack(&THISCLASS::File_InsertImage,this);
		mapPieceMenu->AddTextItem(0,FSKEY_N,FSGUI_MENU_MAPPIECE_INSERT_NEWSCRNSHOT)->BindCallBack(&THISCLASS::MapPiece_InsertNewScreenshot,this);
		mapPieceMenu->AddTextItem(0,FSKEY_F,FSGUI_MENU_MAPPIECE_MOVE_TO_FRONT)->BindCallBack(&THISCLASS::MapPiece_MoveToFront,this);
		mapPieceMenu->AddTextItem(0,FSKEY_B,FSGUI_MENU_MAPPIECE_MOVE_TO_BACK)->BindCallBack(&THISCLASS::MapPiece_MoveToBack,this);
		mapPieceMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_MAPPIECE_UPDATE_SCRNSHOT_CACHE)->BindCallBack(&THISCLASS::MapPiece_RecacheScreenshot,this);
		mapPieceMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_MAPPIECE_UNTRIM)->BindCallBack(&THISCLASS::MapPiece_Untrim,this);
		mapPieceMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_MAPPIECE_REAPPLYUNITALL)->BindCallBack(&THISCLASS::MapPiece_ReapplyTrimAll,this);
		mapPieceMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_MAPPIECE_TRIM_SELECTION_WITH_LARGE_RECTANGLE)->BindCallBack(&THISCLASS::MapPiece_TrimSelectionWithLargeRectangle,this);
	}

	{
		auto markUpMenu=mainMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_MARKUP)->GetSubMenu();
		// Insert Comment is transferred from Edit->Insert
		markUp_Snap=markUpMenu->AddTextItem(MkId("markUp/snap"),FSKEY_NULL,FSGUI_MENU_MARKUP_SNAP);
		markUp_Snap->BindCallBack(&THISCLASS::MarkUp_SnapSwitch,this);
		markUp_Snap->SetCheck(YSTRUE);
		markUpMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_EDIT_INSERT_COMMENT)->BindCallBack(&THISCLASS::Edit_Insert_Comment,this);
		markUpMenu->AddTextItem(0,FSKEY_D,FSGUI_MENU_MARKUP_OPEN_DRAWING_DIALOG)->BindCallBack(&THISCLASS::MarkUp_OpenDrawingDialog,this);
		markUpMenu->AddTextItem(0,FSKEY_P,FSGUI_COMMON_PROPERTIES)->BindCallBack(&THISCLASS::MarkUp_Property,this);
		markUpMenu->AddTextItem(0,FSKEY_A,FSGUI_MENU_MARKUP_ANCHOR_BY_PICKING)->BindCallBack(&THISCLASS::MarkUp_SetAnchorByPicking,this);
		markUpMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_MARKUP_ANCHOR_SELECTED)->BindCallBack(&THISCLASS::MarkUp_SetAnchorOnSelectedMarkUp,this);
		markUpMenu->AddTextItem(0,FSKEY_U,FSGUI_MENU_MARKUP_UNANCHOR_BY_PICKING)->BindCallBack(&THISCLASS::MarkUp_UnsetAnchorByPicking,this);
		markUpMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_MARKUP_UNANCHOR_SELECTED)->BindCallBack(&THISCLASS::MarkUp_UnsetAnchorOfSelectedMarkUp,this);
		markUpMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_MARKUP_REAPPLY_ANCHOR)->BindCallBack(&THISCLASS::MarkUp_ReapplyAnchor,this);
	}

	{
		auto *configMenu=mainMenu->AddTextItem(0,FSKEY_NULL,FSGUI_MENU_CONFIG)->GetSubMenu();
		configMenu->AddTextItem(0,FSKEY_C,FSGUI_MENU_CONFIG_CONFIG)->BindCallBack(&THISCLASS::Config_Config,this);
		configMenu->AddTextItem(0,FSKEY_NULL,L"Data Integrity Check.")->BindCallBack(&THISCLASS::Config_DataIntegrityCheck,this);

	}

	SetMainMenu(mainMenu);

	RefreshRecentlyUsedFileList();
}

