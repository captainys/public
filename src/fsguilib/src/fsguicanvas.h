/* ////////////////////////////////////////////////////////////

File Name: fsguicanvas.h
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

#ifndef FSGUICANVAS_IS_INCLUDED
#define FSGUICANVAS_IS_INCLUDED
/* { */

#include <functional>
#include <fssimplewindow.h>

#include "fsguibase.h"

class FsGuiPopUpMenuItem;

////////////////////////////////////////////////////////////

/*! This class makes a GUI canvas.  GUI canvas can have up to a menu and a main dialog.

	A GUI canvas routes events coming from the operating system.

    If a modal-dialog chain is attached to this canvas, all events are directed to 
    the bottom-most dialog of the modal-dialog chain of the canvas.

	Else, if a modal-dialog chain is attached to the main menu, all events are directed to 
	the bottom-most dialog of the modal-dialog chain of the main menu.

	Else, if a modal-dialog chain is attached to the main dialog, all events are directed to 
	the bottom-most dialog of the modal-dialog chain of the main dialog.

    The modal dialog is responsible for closing it by CloseModalDialog function.
  */
class FsGuiCanvas : public FsGuiControlBase
{
private:
	class ShortCutKey
	{
	public:
		int fsKey;
		YSBOOL shift,ctrl,alt;
		std::function <void (void)> callback;
	};

	class FsGuiPopUpMenu *mainMenu;
	class FsGuiStatusBar *topStatusBar,*bottomStatusBar;
	YsArray <class FsGuiDialog *> dlgArray;
	YsArray <ShortCutKey> shortCutArray;
	class FsGuiDialog *activeDlg;

	int prevMx,prevMy;  // Cache previous mouse cursor position
	YSBOOL prevLb,prevMb,prevRb; // Cache previous mouse button state
	int prevWid,prevHei; // Cache previous window size
	YSBOOL needRedraw;

	YsArray <YsVec2> prevTouch;
	FsGuiObject::TouchState prevTouchState;

	std::function <YSRESULT (FsGuiMouseButtonSet,YsVec2i)> LButtonDownCallBack,LButtonUpCallBack,MButtonDownCallBack,MButtonUpCallBack,RButtonDownCallBack,RButtonUpCallBack;
	std::function <YSRESULT (FsGuiMouseButtonSet,YsVec2i,YsVec2i)> MouseMoveCallBack;
	std::function <YSRESULT (const TouchState &touchState)> TouchStateChangeCallBack;

public:
	/*! Default constructor */
	FsGuiCanvas();

	/*! Default destructor */
	~FsGuiCanvas();

	/*! Initializes the object */
	void Initialize(void);

	/*! This function sets a pointer to the main menu. */
	void SetMainMenu(class FsGuiPopUpMenu *mainMenu);

	/*! This function removes the main menu.  Same as SetMainMenu(NULL); */
	void RemoveMainMenu(void);

	/*! This function sets a status bar at the top of the window. */
	void SetTopStatusBar(class FsGuiStatusBar *topBar);

	/*! This function returns a pointer to the top status bar.  NULL will be returned if no top status bar is set. */
	class FsGuiStatusBar *GetTopStatusBar(void) const;

	/*! This function sets a status bar at the top of the window. */
	void SetBottomStatusBar(class FsGuiStatusBar *bottomBar);

	/*! This function returns a pointer to the bottom status bar.    NULL will be returned if no top status bar is set. */
	class FsGuiStatusBar *GetBottomStatusBar(void) const;

	/*! This function returns a pointer to the main menu.  If no main menu is 
	    set, NULL will be returned. */
	class FsGuiPopUpMenu *GetMainMenu(void) const;

	/*! This function adds a pointer to a modeless dialog. 
	    Returns YSERR if the incoming dialog is already in the dialog array. */
	YSRESULT AddDialog(class FsGuiDialog *dlg);

	/*! This function removes a pointer to a modeless dialog. 
	    Returns YSERR if the dialog is not in the dialog array. */
	YSRESULT RemoveDialog(class FsGuiDialog *dlg);

	/*! This function removes all modeless dialogs. */
	void RemoveDialogAll(void);

	/*! This function removes (closes) all modeless dialogs that are not permanent. */
	void RemoveAllNonPermanentDialog(void);

	/*! This function arranges the dialogs according to the arrangement preference
	    specified in each dialog. */
	void ArrangeDialog(void);

	/*! This function kills all focus in the dialogs. */
	void KillDialogFocusAll(void);

	/*! This function returns the number of top-level dialogs that belongs to this canvas. */
	YSSIZE_T GetNumDialog(void) const;

	/*! This function returns the top-level dialog of the given index. */
	class FsGuiDialog *GetDialog(YSSIZE_T index) const;

	/*! This function returns YSTRUE if the dialog "dlg" is used by this canvas as a modal or modeless dialog. */
	YSBOOL IsDialogUsed(const FsGuiDialog *dlg) const;

	/*! This function returns the active modal dialog.  
	    If it doesn't exist, it returns NULL. */
	class FsGuiDialog *GetActiveModalDialog(void) const;

	/*! This function returns the active modeless dialog.  
	    If it doesn't exist, it returns NULL. */
	class FsGuiDialog *GetActiveDialog(void) const;

	/*! This function sets the active modeless dialog.  If newActiveDlg does not belong 
	    to this canvas, this function returns YSERR.  If  newActiveDlg is NULL,
	    no dialog in this canvas will be active. */
	YSRESULT SetActiveDialog(const class FsGuiDialog *newActiveDlg);

	/*! Control structure (typically a main loop or an interval function) 
	    must call this function to allow this object to non-user and programatic events. */
	void Interval(void);

	/*! Control structure (typically a main loop or an interval function) 
	    must use this function to regularly report the mouse state to the canvas. */
	YSRESULT SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Control structure must use this function to regularly report touch state if available. */
	YSRESULT SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[]);

	/*! Runs a command.  For example,
	      CLICK <Destination> <Identifier>
	      CLICK $mainMenu file/new
	      CLICK $activeDialog cancel
	      CLICK $activeModalDialog ok
	      CLICK coordDialog x
	      SELECT $activeDialog fieldList AOMORI
	      CLOSEMODAL

        $mainMenu is the current main menu.
        $activeDialog is the active modeless dialog.
        $activeModalDialog is the active modal dialog.
        If there is no $ sign, it is the dialog name.
        Identifier must be the string used for FsGuiControlBase::MakeIdent.
	*/
	virtual YSRESULT ProcessCommand(const YsString &str);

private:
	FsGuiControlBase *FindTarget(const char label[]) const;

	// On menu selection, mouse button state must be forgotten by the dialogs so that a button will not be
	// accidentally pressed.
	void ForgetPreviousMouseButtonState(void);

public:
	/*! Control structure (typically a main loop or an interval function) 
	    must use this function to report a key stroke when a key is pressed. */
	YSRESULT KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);

	/*! This function updates the window size.  Does nothing if the size stays the same. */
	YSRESULT SetWindowSize(int newWid,int newHei,YSBOOL autoArrangeDialog);

	/*! Control structure (typically a main loop or an interval function) 
	    must use this function to report a character is generated by a key 
	    stroke. */
	YSRESULT CharIn(int c);

	/*! This function draws all the menus and dialogs on the canvas. */
	void Show(void) const;

	/*! This function returns YSTRUE if the canvas needs to be re-drawn. */
	virtual YSBOOL NeedRedraw(void) const;

	/*! This function sets need-redraw flag. */
	virtual void SetNeedRedraw(YSBOOL needRedraw);

	/*! Override this function to process iteratively. */
	virtual void OnInterval(void);

	/*! Override this function to write a behavior for key stroke.  
	    Must return YSOK if this function processed the key stroke and the key event must not be sent to other objects.  
	    Return YSERR otherwise. */
	virtual YSRESULT OnKeyDown(int fskey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);

	/*! Override this function to write a behavior for when a key stroke generated an ASCII character. 
	    Must return YSOK if this function processed the character and the character must not be sent to other objects.
	    Return YSERR otherwise. */
	virtual YSRESULT OnKeyChar(int c);

	/*! Override this function to write a behavior for left button down. 
	    Must return YSOK if the event is processed in this function and must not be sent to other objects. 
	    Return YSERR otherwise.  */
	virtual YSRESULT OnLButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Override this function to write a behavior for middle button down.
	    Must return YSOK if the event is processed in this function and must not be sent to other objects. 
	    Return YSERR otherwise.  */
	virtual YSRESULT OnMButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Override this function to write a behavior for right button down. 
	    Must return YSOK if the event is processed in this function and must not be sent to other objects. 
	    Return YSERR otherwise.  */
	virtual YSRESULT OnRButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Override this function to write a behavior for mouse move. 
	    Must return YSOK if the event is processed in this function and must not be sent to other objects. 
	    Return YSERR otherwise.  */
	virtual YSRESULT OnMouseMove(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my,int prevMx,int prevMy);

	/*! Override this function to write a behavior for touch state change.
	    Return YSOK if the touch state change is processed in the function.  Otherwise, return YSERR. */
	virtual YSRESULT OnTouchStateChange(const TouchState &touchState);

	/*! Override this function to write a behavior for left button up. 
	    Must return YSOK if the event is processed in this function and must not be sent to other objects. 
	    Return YSERR otherwise.  */
	virtual YSRESULT OnLButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Override this function to write a behavior for middle button up. 
	    Must return YSOK if the event is processed in this function and must not be sent to other objects. 
	    Return YSERR otherwise.  */
	virtual YSRESULT OnMButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Override this function to write a behavior for right button up. 
	    Must return YSOK if the event is processed in this function and must not be sent to other objects. 
	    Return YSERR otherwise.  */
	virtual YSRESULT OnRButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	/*! Assigns a function for left-button down event. */
	template <class objType>
	void BindLButtonDownCallBack(YSRESULT (objType::*func)(FsGuiMouseButtonSet,YsVec2i),objType *receiver)
	{
		LButtonDownCallBack=std::bind(func,receiver,std::placeholders::_1,std::placeholders::_2);
	}

	/*! Assigns a function for left-button up event. */
	template <class objType>
	void BindLButtonUpCallBack(YSRESULT (objType::*func)(FsGuiMouseButtonSet,YsVec2i),objType *receiver)
	{
		LButtonUpCallBack=std::bind(func,receiver,std::placeholders::_1,std::placeholders::_2);
	}

	/*! Assigns a function for middle-button down event. */
	template <class objType>
	void BindMButtonDownCallBack(YSRESULT (objType::*func)(FsGuiMouseButtonSet,YsVec2i),objType *receiver)
	{
		MButtonDownCallBack=std::bind(func,receiver,std::placeholders::_1,std::placeholders::_2);
	}

	/*! Assigns a function for middle-button up event. */
	template <class objType>
	void BindMButtonUpCallBack(YSRESULT (objType::*func)(FsGuiMouseButtonSet,YsVec2i),objType *receiver)
	{
		MButtonUpCallBack=std::bind(func,receiver,std::placeholders::_1,std::placeholders::_2);
	}

	/*! Assigns a function for right-button down event. */
	template <class objType>
	void BindRButtonDownCallBack(YSRESULT (objType::*func)(FsGuiMouseButtonSet,YsVec2i),objType *receiver)
	{
		RButtonDownCallBack=std::bind(func,receiver,std::placeholders::_1,std::placeholders::_2);
	}

	/*! Assigns a function for right-button up event. */
	template <class objType>
	void BindRButtonUpCallBack(YSRESULT (objType::*func)(FsGuiMouseButtonSet,YsVec2i),objType *receiver)
	{
		RButtonUpCallBack=std::bind(func,receiver,std::placeholders::_1,std::placeholders::_2);
	}

	/*! Assigns a function for mouse-move event. */
	template <class objType>
	void BindMouseMoveCallBack(YSRESULT (objType::*func)(FsGuiMouseButtonSet,YsVec2i,YsVec2i),objType *receiver)
	{
		MouseMoveCallBack=std::bind(func,receiver,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
	}

	/*! Assigns a function for touch state change event. */
	template <class objType>
	void BindTouchStateChangeCallBack(YSRESULT (objType::*func)(const FsGuiObject::TouchState &),objType *receiver)
	{
		TouchStateChangeCallBack=std::bind(func,receiver,std::placeholders::_1);
	}



	/*! Erases a function for left-button down event. */
	void ClearLButtonDownCallBack(void);

	/*! Erases a function for left-button up event. */
	void ClearLButtonUpCallBack(void);

	/*! Erases a function for middle-button down event. */
	void ClearMButtonDownCallBack(void);

	/*! Erases a function for middle-button up event. */
	void ClearMButtonUpCallBack(void);

	/*! Erases a function for right-button down event. */
	void ClearRButtonDownCallBack(void);

	/*! Erases a function for right-button up event. */
	void ClearRButtonUpCallBack(void);

	/*! Erases a function for mouse-move event. */
	void ClearMouseMoveCallBack(void);

	/*! Erases a function for touch state change event. */
	void ClearTouchStateChangeCallBack(void);


	/*! Returns a modeless dialog that matches the class T.  If more than one of such modeless dialog is present, it returns the one that is found first. */
	template <class T>
	T *FindTypedModelessDialog(void) const;

private:
	const ShortCutKey *FindShortCut(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt) const;
	ShortCutKey *FindShortCut(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	ShortCutKey *CreateShortCut(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);

private:
	void PrintShortCutKeyWarning(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
public:
	/*! Binds a short-cut key call-back function.
	    Although the function prototype is in a form of menu call-back function,
	    this short-cut key is not associated with a menu item.
	    The menu-item pointer given to the function will be nullptr if the function is invoked from a key.
	*/
	template <typename objType>
	void BindKeyCallBack(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt,void (objType::*func)(FsGuiPopUpMenuItem *),objType *obj)
	{
		auto sck=FindShortCut(fsKey,shift,ctrl,alt);
		if(nullptr==sck)
		{
			auto newShortCut=CreateShortCut(fsKey,shift,ctrl,alt);
			newShortCut->callback=std::bind(func,obj,nullptr);
		}
		else
		{
			// printf("%s %d\n",__FUNCTION__,__LINE__);
			// PrintShortCutKeyWarning(fsKey,shift,ctrl,alt);
			sck->fsKey=fsKey;
			sck->shift=shift;
			sck->ctrl=ctrl;
			sck->alt=alt;
		}
	}

	void ClearKeyCallBack(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
};

template <class T>
T *FsGuiCanvas::FindTypedModelessDialog(void) const
{
	for(auto d : dlgArray)
	{
		auto ptr=dynamic_cast <T *>(d);
		if(NULL!=ptr)
		{
			return ptr;
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////

/* } */
#endif
