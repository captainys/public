/* ////////////////////////////////////////////////////////////

File Name: edit_align.cpp
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

#include <ysshellext_alignmentutil.h>

#include "../ysgebl_gui_editor_base.h"
#include "../textresource.h"



void GeblGuiEditorBase::Edit_Align_MinimumX(FsGuiPopUpMenuItem *)
{
	Edit_Align(0,YsShellExt_AlignmentUtil::ALIGN_MINIMUM);
}
void GeblGuiEditorBase::Edit_Align_CenterX(FsGuiPopUpMenuItem *)
{
	Edit_Align(0,YsShellExt_AlignmentUtil::ALIGN_CENTER);
}
void GeblGuiEditorBase::Edit_Align_MaximumX(FsGuiPopUpMenuItem *)
{
	Edit_Align(0,YsShellExt_AlignmentUtil::ALIGN_MAXIMUM);
}
void GeblGuiEditorBase::Edit_Align_MinimumY(FsGuiPopUpMenuItem *)
{
	Edit_Align(1,YsShellExt_AlignmentUtil::ALIGN_MINIMUM);
}
void GeblGuiEditorBase::Edit_Align_CenterY(FsGuiPopUpMenuItem *)
{
	Edit_Align(1,YsShellExt_AlignmentUtil::ALIGN_CENTER);
}
void GeblGuiEditorBase::Edit_Align_MaximumY(FsGuiPopUpMenuItem *)
{
	Edit_Align(1,YsShellExt_AlignmentUtil::ALIGN_MAXIMUM);
}
void GeblGuiEditorBase::Edit_Align_MinimumZ(FsGuiPopUpMenuItem *)
{
	Edit_Align(2,YsShellExt_AlignmentUtil::ALIGN_MINIMUM);
}
void GeblGuiEditorBase::Edit_Align_CenterZ(FsGuiPopUpMenuItem *)
{
	Edit_Align(2,YsShellExt_AlignmentUtil::ALIGN_CENTER);
}
void GeblGuiEditorBase::Edit_Align_MaximumZ(FsGuiPopUpMenuItem *)
{
	Edit_Align(2,YsShellExt_AlignmentUtil::ALIGN_MAXIMUM);
}

void GeblGuiEditorBase::Edit_Align(int axis,YsShellExt_AlignmentUtil::ALIGNTYPE alignType)
{
	if(nullptr!=Slhd())
	{
		auto &shl=*Slhd();
		YsShellExt_AlignmentUtil alignUtil;
		alignUtil.AddVertex(shl.Conv(),shl.GetSelectedVertex());
		alignUtil.AddPolygon(shl.Conv(),shl.GetSelectedPolygon());
		alignUtil.AddConstEdge(shl.Conv(),shl.GetSelectedConstEdge());
		alignUtil.AddFaceGroup(shl.Conv(),shl.GetSelectedFaceGroup());
		alignUtil.Align(shl,axis,alignType);

		needRemakeDrawingBuffer|=NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}
