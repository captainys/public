/* ////////////////////////////////////////////////////////////

File Name: repairmenu.cpp
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
#include "../miscdlg.h"
#include <ysgl.h>
#include <ysglbuffer.h>

#include <ysshellextedit_stitchingutil.h>
#include <ysshellext_repairutil.h>


void GeblGuiEditorBase::RepairMenu_FreeStitching(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellExtEdit_FreeStitching(shl);

		needRemakeDrawingBuffer|=(
		    NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_FACEGROUP);
		SetNeedRedraw(YSTRUE);
	}
}


void GeblGuiEditorBase::RepairMenu_LoopStitching(FsGuiPopUpMenuItem *)
{
	if(NULL!=Slhd())
	{
		auto &shl=*Slhd();

		YsShellExtEdit_LoopStitching(shl);

		needRemakeDrawingBuffer|=(
		    NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_CONSTEDGE|NEED_REMAKE_DRAWING_SELECTED_POLYGON|NEED_REMAKE_DRAWING_SELECTED_FACEGROUP);
		SetNeedRedraw(YSTRUE);
	}
}



void GeblGuiEditorBase::RepairMenu_RemoveIdenticalPolygon(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		YsShellExt_IdenticalPolygonRemover remover;
		remover.MakeDuplicatePolygonList(Slhd()->Conv());
		remover.DeleteDuplicatePolygon(*Slhd());
		needRemakeDrawingBuffer|=(NEED_REMAKE_DRAWING_POLYGON|NEED_REMAKE_DRAWING_SELECTED_POLYGON);
		SetNeedRedraw(YSTRUE);
	}
}
