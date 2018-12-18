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
#include <stdlib.h>

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
	commandCounter=0;
}



void GeblGuiExtension::OnClearUIMode(void)
{
	// If dialogs from the extension is open, close them here.
}


void GeblGuiExtension::FinalSetUp(void)
{
}


static const char * const testCmd[]=
{
	"CLICK $mainMenu edit/create/primitive",
	"SELECTN createPrimitiveDialog primitiveType 1",
	"KEYIN SPACE",
	"CLICK $mainMenu util/constEdge/clear",
	"CLICK $mainMenu util/constEdge/constAll",
	"CLICK $mainMenu util/faceGroup/fromConstEdge",
	nullptr
};

/* virtual */ void GeblGuiExtension::OnInterval(void)
{
	if(nullptr!=testCmd[commandCounter])
	{
		canvasPtr->ProcessCommand(testCmd[commandCounter]);
		++commandCounter;
	}
	else
	{
		if(NULL!=canvasPtr->Slhd())
		{
			auto &shl=*(canvasPtr->Slhd());

			int returnCode=0;

			if(0==shl.GetNumConstEdge())
			{
				printf("No const edge has been created.\n");
				returnCode=1;
			}
			if(shl.GetNumFaceGroup()<=1)
			{
				printf("Too few face grouops have been created.\n");
				returnCode=1;
			}

			exit(returnCode);
		}
	}
}
