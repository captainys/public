/* ////////////////////////////////////////////////////////////

File Name: ysgebl_gui_extension.cpp
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

#include <ysgebl_gui_editor_base.h>

#include "ysgebl_gui_extension.h"



GeblGuiExtensionBase *PolyCreCreateGuiExtension(class GeblGuiEditorBase &canvas)
{
	return new GeblGuiExtension(canvas);
}


/*! Must be implemented in the specific derivative of Polygon Crest. */
void PolyCreDeleteGuiExtension(GeblGuiExtensionBase *ptr)
{
	delete ptr;
}


/*! This function must return the custom font size.  Return 0 to use the default size. 
    This function is outside GeblGuiExtensionBase since this function is needed before
    GeblGuiEditorBase is created. */
int PolyCreGetCustomFontHeight(void)
{
	return 0;
}




GeblGuiExtension::GeblGuiExtension(class GeblGuiEditorBase &canvas) : GeblGuiExtensionBase(canvas)
{
}

/* virtual */ GeblGuiExtension::~GeblGuiExtension()
{
}




class FsGui3DViewControlDialogBase *GeblGuiExtension::CreateCustomViewControlDialog(class FsGui3DViewControl &)
{
	return NULL; // Return NULL if the default view-control dialog is adequate.
}


void GeblGuiExtension::CreateModelessDialog(void)
{
}


void GeblGuiExtension::DeleteModelessDialog(void)
{
}


void GeblGuiExtension::AddMenu(class GeblGuiEditorMainMenu *mainMenu)
{
	auto extMenu=mainMenu->AddTextItem(0,FSKEY_X,L"Extension")->GetSubMenu();
	extMenu->AddTextItem(0,FSKEY_S,L"Sample")->BindCallBack(&GeblGuiExtension::SampleMenuCallBack,this);
}



void GeblGuiExtension::OnClearUIMode(void)
{
	// If dialogs from the extension is open, close them here.
}



void GeblGuiExtension::FinalSetUp(void)
{
}


void GeblGuiExtension::SampleMenuCallBack(FsGuiPopUpMenuItem *)
{
	auto &canvas=*(this->canvasPtr);

	if(NULL!=canvas.Slhd())
	{
		YsShellExtEdit &shl=*(YsShellExtEdit *)canvas.Slhd();

		YsWString msg(L"Extension Menu Sample\n");

		YsString str;
		YsWString wStr;

		str.Printf("Number of vertices=%d\n",(int)shl.GetNumVertex());
		wStr.SetUTF8String(str);
		msg.Append(wStr);

		str.Printf("Number of polygons=%d\n",(int)shl.GetNumPolygon());
		wStr.SetUTF8String(str);
		msg.Append(wStr);

		msg.Append(L"You can write your own function in gui_extension/ysgebl_gui_extension.cpp.\n");

		canvas.MessageDialog(L"Extension Sample",msg);
	}
}
