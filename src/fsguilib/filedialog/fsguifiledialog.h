/* ////////////////////////////////////////////////////////////

File Name: fsguifiledialog.h
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

#ifndef FSGUIFILEDIALOG_IS_INCLUDED
#define FSGUIFILEDIALOG_IS_INCLUDED
/* { */

#include <fsgui.h>



/*! This class provides a functionality of file dialog.
    To enable platform-specific file dialog, this class hooks Show function and calls a platform-specific file dialog function. 
	
	FsGuiCanvas is required to use this class.  It might work without FsGuiCanvas in Windows and MacOSX.  However the behavior of the class is not guaranteed in other environment and in the future versions.  
  */
class FsGuiFileDialog : public FsGuiDialog
{
private:
	typedef FsGuiFileDialog THISCLASS;

	mutable YSBOOL busy;
	class FsGuiFileDialog_PlatformSpecificData *param;
	static void CloseModalCallBackFunc(class FsGuiControlBase *,class FsGuiDialog *,int);
	void CloseModalCallBackFunc(FsGuiDialog *closedModalDialog,int returnCode);

public:
	enum MODE
	{
		MODE_OPEN,
		MODE_SAVE
	};

	/*! Input: Set YSTRUE if the file dialog to allow multiple selection. */
	YSBOOL multiSelect;

	/*! Input; Set file dialog mode. */
	MODE mode;

	/*! Input: File extensions. */
	YsArray <YsWString> fileExtensionArray;

	/*! Input: Default file name. */
	YsWString defaultFileName;

	/*! Input: Title. */
	YsWString title;

	/*! Input/Output: This will be set to YSTRUE when the user made a selection.  Can be used in the polling-based application. */
	YSBOOL selectionMade;

	/*! Output: Set to YSOK if the user clicks "OK", or YSERR otherwise. */
	YSRESULT res;

	/*! Output: Selected files. */
	YsArray <YsWString> selectedFileArray;

	/*! Default constructor. */
	FsGuiFileDialog();

	/*! Default destructor. */
	~FsGuiFileDialog();

	/*! Initializes the variables. */
	void Initialize(void);

	/*! This function will call platform-specific file dialog function, and let the user select a file(s).  When the files are selected, this function will call CloseModalDialog to give control back to the parent dialog or canvas. */
	virtual void Show(const FsGuiDialog *excludeFromDrawing=NULL) const;

private:
	virtual void OnAttach(void);
	void AllocatePlatformSpecificData(void);
	void DeletePlatformSpecificData(void);
};

/* } */
#endif
