/* ////////////////////////////////////////////////////////////

File Name: textresource.h
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

#ifndef FSTEXTRESOURCE_IS_INCLUDED
#define FSTEXTRESOURCE_IS_INCLUDED
/* { */

#include <ysclass.h>



extern YsTextResource fsGuiTextResource;

inline const wchar_t *FsGetTextResource(const char *key,const wchar_t *alternative)
{
	const wchar_t *primary=fsGuiTextResource.FindWString(key);
	if(NULL!=primary)
	{
		return primary;
	}
	return alternative;
}


#define FSGUI_COMMON_APPLY                                FsGetTextResource("common/apply",L"Apply")
#define FSGUI_COMMON_CLEAR                                FsGetTextResource("common/clear",L"Clear")
#define FSGUI_COMMON_CUTPLANE                             FsGetTextResource("common/cutplane",L"Cutting Plane")
#define FSGUI_COMMON_DISTTHR                              FsGetTextResource("common/distthr",L"Distance Threshold")
#define FSGUI_COMMON_G0CONTINUOUS                         FsGetTextResource("common/g0continuous",L"G0 continuous")
#define FSGUI_COMMON_G1CONTINUOUS                         FsGetTextResource("common/g1continuous",L"G1 continuous")
#define FSGUI_COMMON_G2CONTINUOUS                         FsGetTextResource("common/g2continuous",L"G2 continuous")
#define FSGUI_COMMON_HELP                                 FsGetTextResource("common/help",L"Help")
#define FSGUI_COMMON_INSERT                               FsGetTextResource("common/insert",L"Insert")
#define FSGUI_COMMON_LENGTH                               FsGetTextResource("common/length",L"Length")
#define FSGUI_COMMON_NORMAL                               FsGetTextResource("common/normal",L"Normal")
#define FSGUI_COMMON_OK                                   FsGetTextResource("common/ok",L"OK")
#define FSGUI_COMMON_CANCEL                               FsGetTextResource("common/cancel",L"Cancel")
#define FSGUI_COMMON_CLOSE                                FsGetTextResource("common/close",L"Close")
#define FSGUI_COMMON_DELETE                               FsGetTextResource("common/delete",L"Delete")
#define FSGUI_COMMON_PROPERTIES                           FsGetTextResource("common/properties",L"Properties")
#define FSGUI_COMMON_REVERSE                              FsGetTextResource("common/reverse",L"Reverse")
#define FSGUI_COMMON_TANGENT                              FsGetTextResource("common/tangent",L"Tangent")
#define FSGUI_COMMON_TYPE                                 FsGetTextResource("common/type",L"Type")
#define FSGUI_COMMON_YES                                  FsGetTextResource("common/yes",L"Yes")
#define FSGUI_COMMON_NO                                   FsGetTextResource("common/no",L"No")
#define FSGUI_COMMON_ERROR                                FsGetTextResource("common/error",L"Error!")
#define FSGUI_COMMON_ANGLE                                FsGetTextResource("common/angle",L"Angle")



#define FSGUI_DLG_ABOUT_DONATELINE1                       FsGetTextResource("dlg/about/donateline1",L"Polygon Crest is a free and open-source polygon editor.\nBut, I appreicate if you can donate some money for supporting the development.")
#define FSGUI_DLG_ABOUT_DONATELINE2                       FsGetTextResource("dlg/about/donateline2",L"If you don't mind making some donation, please send money\nvia PayPal from the following button.")
#define FSGUI_DLG_ABOUT_DONATELINE3                       FsGetTextResource("dlg/about/donateline3",L"...")
#define FSGUI_DLG_ABOUT_DONATEPAYPAL                      FsGetTextResource("dlg/about/donatepaypal",L"Donate via PayPal")
#define FSGUI_DLG_ABOUT_DONATEVECTOR                      FsGetTextResource("dlg/about/donatevector",L"Donate via Vector ShareReg")



#define FSGUI_DLG_COMMENT_FONTSIZE                        FsGetTextResource("dlg/comment/fontsize",L"Font Size")
#define FSGUI_DLG_COMMENT_TRANSPARENT_BACKGROUND          FsGetTextResource("dlg/comment/transparent_background",L"Transparent Background")
#define FSGUI_DLG_COMMON_ASK_OVERWRITE                    FsGetTextResource("dlg/common/ask_overwrite",L"Overwrite the existing file?")
#define FSGUI_DLG_COMMON_FILE_ALREADY_EXIST               FsGetTextResource("dlg/common/file_already_exist",L"File Already Exists.")
#define FSGUI_DLG_COMMON_NEED_SELECT_ONE_MAPPIECE         FsGetTextResource("dlg/common/need_select_one_mappiece",L"One map piece must be selected.")
#define FSGUI_DLG_COMMON_TOO_MANY_SELECTED                FsGetTextResource("dlg/common/too_many_selected",L"Too many map pieces are selected.")
#define FSGUI_DLG_CONFIG_BROWSE                           FsGetTextResource("dlg/config/browse",L"Browse")
#define FSGUI_DLG_CONFIG_BROWSE_MESSAGE                   FsGetTextResource("dlg/config/browse_message",L"Please select one .png file to select a directory.")
#define FSGUI_DLG_CONFIG_FOLDERNAME                       FsGetTextResource("dlg/config/foldername",L"Screenshot Folder Locations")
#define FSGUI_DLG_CONFIRM_QUIT_MESSAGE                    FsGetTextResource("dlg/confirm_quit/message",L"The map has been modified.\nThe changes will be discarded if you quit.\nDo you really want to quit?")
#define FSGUI_DLG_CONFIRM_QUIT_TITLE                      FsGetTextResource("dlg/confirm_quit/title",L"Really Quit?")
#define FSGUI_DLG_CONTROL_ADD_COMMENT                     FsGetTextResource("dlg/control/add_comment",L"Add Comment")
#define FSGUI_DLG_CONTROL_ADD_IMAGE_FILE                  FsGetTextResource("dlg/control/add_image_file",L"Add Image")
#define FSGUI_DLG_CONTROL_ADD_NEW_SCRNSHOT                FsGetTextResource("dlg/control/add_new_scrnshot",L"Add New Screenshot")
#define FSGUI_DLG_CONTROL_ADJUSTMENT                      FsGetTextResource("dlg/control/adjustment",L"Make Adjustment")
#define FSGUI_DLG_CONTROL_DIFFTHR                         FsGetTextResource("dlg/control/diffthr",L"Threshold")
#define FSGUI_DLG_CONTROL_DOWN                            FsGetTextResource("dlg/control/down",L"Down")
#define FSGUI_DLG_CONTROL_LEFT                            FsGetTextResource("dlg/control/left",L"Left")
#define FSGUI_DLG_CONTROL_RIGHT                           FsGetTextResource("dlg/control/right",L"Right")
#define FSGUI_DLG_CONTROL_SHOWDIFF                        FsGetTextResource("dlg/control/showdiff",L"Show Diff")
#define FSGUI_DLG_CONTROL_UP                              FsGetTextResource("dlg/control/up",L"Up")
#define FSGUI_DLG_DRAWING_AUTOANCHOR                      FsGetTextResource("dlg/drawing/autoanchor",L"Auto Anchor")
#define FSGUI_DLG_DRAWING_FILL                            FsGetTextResource("dlg/drawing/fill",L"Fill")
#define FSGUI_DLG_DRAWING_LINE                            FsGetTextResource("dlg/drawing/line",L"Line")
#define FSGUI_DLG_DRAWING_LINESTRIP                       FsGetTextResource("dlg/drawing/linestrip",L"LineStrip")
#define FSGUI_DLG_DRAWING_MULTIPLEELEM                    FsGetTextResource("dlg/drawing/multipleelem",L"Multiple Elements")
#define FSGUI_DLG_DRAWING_NONE                            FsGetTextResource("dlg/drawing/none",L"None")
#define FSGUI_DLG_EXPORTPNG_INCLUDE_MARKUP                FsGetTextResource("dlg/exportpng/include_markup",L"Include Mark Up")
#define FSGUI_DLG_FIELDORDER_DOWN                         FsGetTextResource("dlg/fieldorder/down",L"Down")
#define FSGUI_DLG_FIELDORDER_UP                           FsGetTextResource("dlg/fieldorder/up",L"Up")
#define FSGUI_DLG_FIELDPROP_FIELDNAME                     FsGetTextResource("dlg/fieldprop/fieldname",L"Field Name")
#define FSGUI_DLG_FIELD_CONFIRM_DELETE                    FsGetTextResource("dlg/field/confirm_delete",L"Really delete this field?")
#define FSGUI_DLG_NEED_CONFIGURE_SCRNSHOTDIR              FsGetTextResource("dlg/need_configure_scrnshotdir",L"Please select screen-shot directory from Config.")
#define FSGUI_DLG_OPEN_CONFIRM_DISCARD_CHANGES            FsGetTextResource("dlg/open/confirm_discard_changes",L"The map has been modified.\nOpening a file will discard changes.\nDo you really want to discard changes and open a file?")
#define FSGUI_DLG_SAVE_SAVED                              FsGetTextResource("dlg/save/saved",L"Saved.")
#define FSGUI_DLG_SCALING_ALL                             FsGetTextResource("dlg/scaling/all",L"Scale All Map Pieces")
#define FSGUI_DLG_SCALING_MAINTAIN_ASPECT                 FsGetTextResource("dlg/scaling/maintain_aspect",L"Maintain Aspect Ratio")
#define FSGUI_DLG_SCALING_THIS_AND_NEW                    FsGetTextResource("dlg/scaling/this_and_new",L"Scale This and New Map Pieces")
#define FSGUI_DLG_SCALING_THIS_ONLY                       FsGetTextResource("dlg/scaling/this_only",L"Scale This Map Piece Only")
#define FSGUI_DLG_SEARCH_CASE_SENSITIVE                   FsGetTextResource("dlg/search/case_sensitive",L"Case Sensitive")
#define FSGUI_DLG_SEARCH_SELECT                           FsGetTextResource("dlg/search/select",L"Select")
#define FSGUI_DLG_SETUNIT_SET                             FsGetTextResource("dlg/setunit/set",L"Set")
#define FSGUI_DLG_SETUNIT_SET_AND_APPLY_ALL               FsGetTextResource("dlg/setunit/set_and_apply_all",L"Set and Apply to All")
#define FSGUI_DLG_TRIM_APPLY_ALL                          FsGetTextResource("dlg/trim/apply_all",L"All Map Pieces")
#define FSGUI_DLG_TRIM_THIS_MAPPIECE_ONLY                 FsGetTextResource("dlg/trim/this_mappiece_only",L"This Map Piece Only")
#define FSGUI_DLG_TRIM_TRIM_THIS_AND_NEW                  FsGetTextResource("dlg/trim/trim_this_and_new",L"This Map Piece and New Map Pieces")
#define FSGUI_DLG_TRIM_USE_UNIT                           FsGetTextResource("dlg/trim/use_unit",L"Use Unit")
#define FSGUI_DLG_WORLDPROP_NAME                          FsGetTextResource("dlg/worldprop/name",L"Name")
#define FSGUI_ERROR_CANNOT_WRITE_FILE                     FsGetTextResource("error/cannot_write_file",L"Cannot write to file.")
#define FSGUI_ERROR_CRASH_WHILE_CACHE                     FsGetTextResource("error/crash_while_cache",L"The program crashed or forcibly terminated while making a screenshot file cache.\nPlease check if you have specified a correct directory in the config dialog.\nSpecifying root directory of the root drive may cause the program to crash or stop responding.")
#define FSGUI_ERROR_NEED_SELECT_MARKUP                    FsGetTextResource("error/need_select_markup",L"Please select a mark up(s)")
#define FSGUI_MENU_CONFIG                                 FsGetTextResource("menu/config",L"Config")
#define FSGUI_MENU_CONFIG_CONFIG                          FsGetTextResource("menu/config/config",L"Config")
#define FSGUI_MENU_EDIT                                   FsGetTextResource("menu/edit",L"Edit")
#define FSGUI_MENU_EDIT_CLEAR_UI_MODE                     FsGetTextResource("menu/edit/clear_ui_mode",L"Clear UI Mode")
#define FSGUI_MENU_EDIT_COPY                              FsGetTextResource("menu/edit/copy",L"Copy")
#define FSGUI_MENU_EDIT_DELETE                            FsGetTextResource("menu/edit/delete",L"Delete")
#define FSGUI_MENU_EDIT_INSERT                            FsGetTextResource("menu/edit/insert",L"Insert")
#define FSGUI_MENU_EDIT_INSERT_COMMENT                    FsGetTextResource("menu/edit/insert/comment",L"Comment")
#define FSGUI_MENU_EDIT_PASTE                             FsGetTextResource("menu/edit/paste",L"Paste")
#define FSGUI_MENU_EDIT_REDO                              FsGetTextResource("menu/edit/redo",L"Redo\nCtrl+Y")
#define FSGUI_MENU_EDIT_SCALING                           FsGetTextResource("menu/edit/scaling",L"Scaling")
#define FSGUI_MENU_EDIT_SEARCH                            FsGetTextResource("menu/edit/search",L"Search")
#define FSGUI_MENU_EDIT_SEARCH_EXACT_MATCH                FsGetTextResource("menu/edit/search/exact_match",L"Exact Match")
#define FSGUI_MENU_EDIT_SEARCH_PARTIAL_MATCH              FsGetTextResource("menu/edit/search/partial_match",L"Partial")
#define FSGUI_MENU_EDIT_TRIM                              FsGetTextResource("menu/edit/trim",L"Trim")
#define FSGUI_MENU_EDIT_UNDO                              FsGetTextResource("menu/edit/undo",L"Undo\nCtrl+Z")
#define FSGUI_MENU_FIELD                                  FsGetTextResource("menu/field",L"Field")
#define FSGUI_MENU_FIELD_CHANGE_ORDER                     FsGetTextResource("menu/field/change_order",L"Change Order")
#define FSGUI_MENU_FIELD_NEWFIELD                         FsGetTextResource("menu/field/newfield",L"Add New Field")
#define FSGUI_MENU_FIELD_NEXTFIELD                        FsGetTextResource("menu/field/nextfield",L"Next Field\tCtrl+Tab")
#define FSGUI_MENU_FIELD_PREVFIELD                        FsGetTextResource("menu/field/prevfield",L"Previous Field\tCtrl+Shift+Tab")
#define FSGUI_MENU_FILE                                   FsGetTextResource("menu/file",L"File")
#define FSGUI_MENU_FILE_EXIT                              FsGetTextResource("menu/file/exit",L"Exit")
#define FSGUI_MENU_FILE_EXPORT_AS_BITMAP                  FsGetTextResource("menu/file/export_as_bitmap",L"Export As Bitmap")
#define FSGUI_MENU_FILE_EXPORT_AS_BITMAP_ALL_FIELD        FsGetTextResource("menu/file/export_as_bitmap_all_field",L"Export All Fields as PNG")
#define FSGUI_MENU_FILE_INSERT_IMAGE                      FsGetTextResource("menu/file/insert_image",L"Insert Image")
#define FSGUI_MENU_FILE_OPEN                              FsGetTextResource("menu/file/open",L"Open")
#define FSGUI_MENU_FILE_RECENT                            FsGetTextResource("menu/file/recent",L"Recent Files...")
#define FSGUI_MENU_FILE_SAVE                              FsGetTextResource("menu/file/save",L"Save")
#define FSGUI_MENU_FILE_SAVEAS                            FsGetTextResource("menu/file/saveas",L"Save As")
#define FSGUI_MENU_LOOKAT                                 FsGetTextResource("menu/lookat",L"Look At...")
#define FSGUI_MENU_LOOKAT_SELECTION                       FsGetTextResource("menu/lookat/selection",L"Selection\tHOME")
#define FSGUI_MENU_MAPPIECE                               FsGetTextResource("menu/mappiece",L"Map Piece")
#define FSGUI_MENU_MAPPIECE_INSERT_NEWSCRNSHOT            FsGetTextResource("menu/mappiece/insert/newscrnshot",L"Insert New Screen Shot")
#define FSGUI_MENU_MAPPIECE_MOVE_TO_BACK                  FsGetTextResource("menu/mappiece/move_to_back",L"Move to Back")
#define FSGUI_MENU_MAPPIECE_MOVE_TO_FRONT                 FsGetTextResource("menu/mappiece/move_to_front",L"Move to Front")
#define FSGUI_MENU_MAPPIECE_REAPPLYUNITALL                FsGetTextResource("menu/mappiece/reapplyunitall",L"Reapply Unit to All Map Pieces")
#define FSGUI_MENU_MAPPIECE_TRIM_SELECTION_WITH_LARGE_RECTANGLE FsGetTextResource("menu/mappiece/trim_selection_with_large_rectangle",L"Trim Selected Map Pieces with a Large Rectangle")
#define FSGUI_MENU_MAPPIECE_UNTRIM                        FsGetTextResource("menu/mappiece/untrim",L"Untrim")
#define FSGUI_MENU_MAPPIECE_UPDATE_SCRNSHOT_CACHE         FsGetTextResource("menu/mappiece/update_scrnshot_cache",L"Update Screenshot Cache")
#define FSGUI_MENU_MARKUP                                 FsGetTextResource("menu/markup",L"Mark Up")
#define FSGUI_MENU_MARKUP_ANCHOR_BY_PICKING               FsGetTextResource("menu/markup/anchor_by_picking",L"Anchor by Picking")
#define FSGUI_MENU_MARKUP_ANCHOR_SELECTED                 FsGetTextResource("menu/markup/anchor_selected",L"Anchor Selected Mark Ups")
#define FSGUI_MENU_MARKUP_OPEN_DRAWING_DIALOG             FsGetTextResource("menu/markup/open_drawing_dialog",L"Open Drawing Dialog")
#define FSGUI_MENU_MARKUP_REAPPLY_ANCHOR                  FsGetTextResource("menu/markup/reapply_anchor",L"Reapply Anchors")
#define FSGUI_MENU_MARKUP_SNAP                            FsGetTextResource("menu/markup/snap",L"Snap to Unit")
#define FSGUI_MENU_MARKUP_UNANCHOR_BY_PICKING             FsGetTextResource("menu/markup/unanchor_by_picking",L"Unanchor by Picking")
#define FSGUI_MENU_MARKUP_UNANCHOR_SELECTED               FsGetTextResource("menu/markup/unanchor_selected",L"Unanchor Selected Mark Ups.")
#define FSGUI_MENU_SELECT                                 FsGetTextResource("menu/select",L"Select")
#define FSGUI_MENU_SELECT_SELECTALL                       FsGetTextResource("menu/select/selectall",L"Select All")
#define FSGUI_MENU_SELECT_UNSELECTALL                     FsGetTextResource("menu/select/unselectall",L"Unselect All\tT")
#define FSGUI_MENU_VIEW                                   FsGetTextResource("menu/view",L"View")

#define FSGUI_MENU_VIEW_DRAW_MARKUP                       FsGetTextResource("menu/view/draw_markup",L"Show Mark Ups")
#define FSGUI_MENU_VIEW_FIT                               FsGetTextResource("menu/view/fit",L"Fit")
#define FSGUI_MENU_VIEW_MOVE_POSITION_REFERENCE_MARKER    FsGetTextResource("menu/view/move_position_reference_marker",L"Move Position Reference Marker")
#define FSGUI_MENU_VIEW_RESTORE_VIEW_PER_FIELD            FsGetTextResource("menu/view/restore_view_per_field",L"Restore Viewpoint when Switching Field")
#define FSGUI_MENU_VIEW_SHOW_POSITION_REFERENCE_MARKER    FsGetTextResource("menu/view/show_position_reference_marker",L"Show Position Reference Marker")
#define FSGUI_MENU_WORLD                                  FsGetTextResource("menu/world",L"World")
#define FSGUI_MENU_WORLD_SETUNIT                          FsGetTextResource("menu/world/setunit",L"Set Unit")
/* } */
#endif
