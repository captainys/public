/* ////////////////////////////////////////////////////////////

File Name: fsgui3dapp.h
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

#ifndef FSGUI_3DAPP_IS_INCLUDED
#define FSGUI_3DAPP_IS_INCLUDED
/* { */

#include <fsgui3d.h>

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

private:
	/*! For convenience, you can use THISCLASS instead of FsGui3DMainCanvas 
	    in the member functions.
	*/
	typedef FsGui3DMainCanvas THISCLASS;

public:
	// [Core data structure]


	// [GUI support]
	FsGuiRecentFiles recent;
	YsWString lastUsedFileName;
	FsGuiPopUpMenu *fileRecent;


	// [Modeless dialogs]
	//   (1) Add a pointer in the following chunk.
	//   (2) Add an initialization in the constructor of the aplication.
	//   (3) Add deletion in the destructor of the application.
	//   (4) Add RemoveDialog in Edit_ClearUIIMode


	// [Modal dialogs]


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

	/*! Implement this function.  Save data.
	*/
	void Save(YsWString fName,YSBOOL updateRecent);
	/*! Implement this function.  Load data.
	*/
	void Open(YsWString fName,YSBOOL updateRecent);

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

private:
	// [Menu pointers for check marks]


	// [Menu call-backs]
	/*! Sample call-back functions.
	*/
	void File_Open(FsGuiPopUpMenuItem *);
	void File_Open_FileSelected(FsGuiDialog *dlg,int returnCode);

	void File_Save(FsGuiPopUpMenuItem *);

	void File_SaveAs(FsGuiPopUpMenuItem *);
	void File_SaveAs_FileSelected(FsGuiDialog *dlg,int returnCode);
	void File_SaveAs_OverwriteConfirmed(FsGuiDialog *dlg,int returnCode);

	void File_Exit(FsGuiPopUpMenuItem *);
	void File_Exit_ConfirmExitCallBack(FsGuiDialog *,int);
	void File_Exit_ReallyExit(void);

	void File_Recent(FsGuiPopUpMenuItem *itm);

	/*! Customize this function.
	    This function must return a file name where recent file list is saved.
	*/
	YsWString GetRecentFileListFileName(void) const;

	void RefreshRecentlyUsedFileList(void);

	void AddRecentlyUsedFile(YsWString fName);
};

/* } */
#endif
