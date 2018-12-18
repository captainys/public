/* ////////////////////////////////////////////////////////////

File Name: fssimplefiledialog.h
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

#ifndef FSSIMPLEFILEDIALOG_IS_INCLUDED
#define FSSIMPLEFILEDIALOG_IS_INCLUDED
/* { */

#include <functional>
#include <fssimplewindow.h>
#include <ysclass.h>



/*! This class provides a functionality of file dialog.
    To enable platform-specific file dialog, this class hooks Show function and calls a platform-specific file dialog function. 
	
	This requires at least FsSimpleWindow and YsClass library.

	Linux implementation also links yssystemfontlibrary.
*/
class FsSimpleFileDialog
{
private:
	typedef FsSimpleFileDialog THISCLASS;
	std::function <void(THISCLASS *,int returnCode)> closeModalCallBack;
	YSBOOL busy;

	void Start(void);  // Platform-specific implementation

	class PlatformSpecificData;
	PlatformSpecificData *param;
	void AllocatePlatformSpecificData(void);
	void DeletePlatformSpecificData(void);
public:
	enum MODE
	{
		MODE_OPEN,
		MODE_SAVE
	};

	// [Public data members]

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

	/*! Output: Selected files. */
	YsArray <YsWString> selectedFileArray;


	// [Public member functions]

	/*! Default constructor. */
	FsSimpleFileDialog();

	/*! Default destructor. */
	~FsSimpleFileDialog();

	/*! Initializes the variables. */
	void Initialize(void);

	/*! Shows the dialog and let the user select files.
	    It might be asynchronous.  The file may not have been selected yet when this function returns.
	    The file selection must be caught by OnClose or by a function set by BindCloseModalCallback.
	*/
	void Show(void);

	/*! Binds a function that catches close-modal (file-selected or cancelled) event.
	*/
	template <typename objType>
	void BindCloseModalCallBack(void (objType::*func)(FsSimpleFileDialog *,int),objType *obj)
	{
		closeModalCallBack=std::bind(func,obj,std::placeholders::_1,std::placeholders::_2);
	}

	/*! Overload this function ,or 
	*/
	virtual void OnClose(int returnCode);
};

/* } */
#endif
