/* ////////////////////////////////////////////////////////////

File Name: fsguidialogutil.h
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

#ifndef FSGUIDIALOGUTIL_IS_INCLUDED
#define FSGUIDIALOGUTIL_IS_INCLUDED
/* { */

#include <ysclass.h>
#include "fsguidialog.h"
#include "fsguiclipboard.h"

/*! Deprecated and left for backward compatibility.  Use FsGuiMessageDialog instead. */
class FsGuiMessageBoxDialog : public FsGuiDialog
{
private:
	FsGuiStatic *label;
	FsGuiDialogItem *yesBtn,*noBtn,*btn;
	FsGuiButton *copyBtn;
	int returnCodeYes,returnCodeNo;

public:
	void Make(
	    const wchar_t title[],const wchar_t msg[],const wchar_t yesBtn[],const wchar_t noBtn[],
	    int returnCodeYes=(int)YSOK,int returnCodeNo=(int)YSERR,YSBOOL okBtnBottom=YSFALSE);
	void Make(
	    const wchar_t title[],const wchar_t msg[],const wchar_t yesBtn[],const wchar_t noBtn[],const wchar_t copyBtn[],
	    int returnCodeYes=(int)YSOK,int returnCodeNo=(int)YSERR,YSBOOL okBtnBottom=YSFALSE);
	virtual void OnKeyDown(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual void OnButtonClick(FsGuiButton *btn);
};


////////////////////////////////////////////////////////////

template <class PAYLOADTYPE>
class FsGuiMessageBoxDialogWithPayload : public FsGuiMessageBoxDialog
{
public:
	PAYLOADTYPE payload;
};

////////////////////////////////////////////////////////////

template <const int N>
class FsGuiInputNumberDialogTemplate : public FsGuiDialog
{
public:
	FsGuiTextBox *numTxt[N];
	FsGuiButton *okBtn,*cancelBtn;
	int returnCodeYes,returnCodeNo;

	void OnButtonClickTemplate(FsGuiButton *btn);
};

/*! This dialog is for taking a number as input.
*/
class FsGuiInputNumberDialog : public FsGuiInputNumberDialogTemplate <1>
{
public:
	/*! Set up text-box and buttons on the dialog.
	*/
	void Make(
	    const double defNumber,const int belowDecimal,
	    const wchar_t title[],const wchar_t msg[],const wchar_t label[],
	    const wchar_t yesBtn[],const wchar_t noBtn[],int returnCodeYes=(int)YSOK,int returnCodeNo=(int)YSERR);

	/*! Get number entered in the text box.
	*/
	const double GetNumber(void) const;

	/*! Default behavior of OnButtonClick is to close the modal dialog.
	    Return code will be (int)YSOK, if ok button is clicked, or (int)YSERR if cancel button is clicked.
	*/
	virtual void OnButtonClick(FsGuiButton *btn);
};

class FsGuiInputTwoNumberDialog : public FsGuiInputNumberDialogTemplate <2>
{
public:
	void Make(
	    const double dflt[2],const int belowDecimal,
	    const wchar_t msg[],const wchar_t title[],const wchar_t label1[],const wchar_t label2[],
	    const wchar_t yesBtn[],const wchar_t noBtn[],int returnCodeYes,int returnCodeNo);
	const double GetNumber(int idx) const;

	virtual void OnButtonClick(FsGuiButton *btn);
};

/* } */
#endif
