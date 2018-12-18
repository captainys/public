/* ////////////////////////////////////////////////////////////

File Name: fsguipopupmenu.h
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

#ifndef FSGUIPOPUPMENU_IS_INCLUDED
#define FSGUIPOPUPMENU_IS_INCLUDED
/* { */

#include <functional>

#include "fsguibase.h"

class FsGuiPopUpMenuItem : private FsGuiObject
{
friend class FsGuiPopUpMenu;

protected:
	int x0,y0,wid,hei;
	YSBOOL checked,isText;
	int id,fsKey;

	int msgWid,msgHei;
	int rightMsgWid,rightMsgHei;

	YsWString wStr;

	mutable FSGUI_DRAWINGTYPE drawingType;
	mutable FsGuiBitmapType bmp;

	YsWString rightAlignedWStr;
	FsGuiBitmapType rightAlignedBmp;



	class FsGuiPopUpMenu *owner;
	class FsGuiPopUpMenu *subMenu;
	void *selectionCallBackParam;
	void (*selectionCallBack)(void *param,class FsGuiPopUpMenu *menu,FsGuiPopUpMenuItem *selectedItem);

	std::function <void ()> stdCallBack;

public:
	FsGuiPopUpMenuItem();
	~FsGuiPopUpMenuItem();

	void Initialize(void);

	/*! Adds a sub-menu to the menu item, and returns a pointer to the sub-menu.
	    If the menu item already has a sub-menu, it returns a pointer to the sub-menu
	    already owned by this menu item without creating a new one.
	*/
	class FsGuiPopUpMenu *AddSubMenu(void);

	/*! Adds a sub-menu at the specific location, and returns a pointer to the sub-menu.
	    If the menu item already has a sub-menu, it returns a pointer to the sub-menu
	    already owned by this menu item without creating a new one.
	*/
	class FsGuiPopUpMenu *AddSubMenu(int x0,int y0);

	/*! Returns a pointer to the sub-menu already owned by this menu item.
	    If this menu item does not own a sub-menu, it returns nullptr.
	*/
	class FsGuiPopUpMenu *GetSubMenu(void);
	const class FsGuiPopUpMenu *GetSubMenu(void) const;

	/*! Returns a pointer to the text message of this menu item.
	*/
	const wchar_t *GetString(void) const;

	/*! Turns on or off the check mark.  If the given pointer is nullptr, it does nothing.
	    Useful when there are versions of the GUI that some may have a specific menu and others may not.
	*/
	static void SetCheck(FsGuiPopUpMenuItem *item,YSBOOL checked);

	/*! Turn on or off the check mark left of the menu item.
	*/
	void SetCheck(YSBOOL checked);

	/*! Returns the state of the check mark left ot the menu item.  If the given pointer is nullptr, it always returns YSFALSE.
	*/
	static const YSBOOL GetCheck(const FsGuiPopUpMenuItem *itm);

	/*! Returns the state of the check mark left ot the menu item.
	*/
	const YSBOOL GetCheck(void) const;

	/*! Invert the on/off state of the check mark.
	*/
	void InvertCheck(void);

	const int GetId(void) const;

	// Set up a call back to an arbitrary function.
	template <typename callBackType>
	void SetCallBack(callBackType func)
	{
		stdCallBack=func;
	}

	/*! Bindes a call-back function that is called when the menu is selected.
	    The call back function must be a member function of objType, and 
	    must take FsGuiPopUpMenuItem pointer as the only parameter.
	*/
	template <typename objType>
	void BindCallBack(void (objType::*func)(FsGuiPopUpMenuItem *),objType *obj)
	{
		stdCallBack=std::bind(func,obj,this);
	}
};

class FsGuiPopUpMenu : public FsGuiControlBase
{
friend class FsGuiPopUpMenuItem;

public:
	using FsGuiControlBase::SetParent;
	using FsGuiControlBase::GetModalDialog;
	using FsGuiControlBase::MakeIdent;
	using FsGuiControlBase::FindIdent;
	using FsGuiControlBase::VirtualClick;

protected:
	YSBOOL pullDown;

	YsListAllocator <FsGuiPopUpMenuItem> itemAlloc;
	YsListContainer <FsGuiPopUpMenuItem> itemList;
	FsGuiPopUpMenuItem *openSubMenu,*mouseOn;

	FsGuiPopUpMenuItem *owner;

	int x0,y0,wid,hei;

	int lastX,lastY;

	int pMx,pMy;
	YSBOOL pLb,pMb,pRb;
	FsGuiPopUpMenuItem *lastItem;

	YsArray <YsVec2> prevTouch;

	mutable YSBOOL needRedraw;

public:
	YsColor bgCol,fgCol;
	YsColor activeBgCol,activeFgCol;
	YsColor frameCol;



	FsGuiPopUpMenu();
	~FsGuiPopUpMenu();

	void SetIsPullDownMenu(YSBOOL isPulLDown);
	YSBOOL IsPullDownMenu(void) const;

	virtual void SetNeedRedraw(YSBOOL needRedraw);
	virtual YSBOOL NeedRedraw(void) const;

	void FitSubMenuToWindow(FsGuiPopUpMenu *subMenu);

	/*! Returns the first menu item that matches the specified identifier number (depth-first). */
	const FsGuiPopUpMenuItem *FindMenuItem(int ident) const;
private:
	const FsGuiPopUpMenuItem *FindMenuItem(const FsGuiPopUpMenu *menuPtr,int ident) const;

public:
	/*! Returns the first menu item that matches the specified identifier number (depth-first). */
	FsGuiPopUpMenuItem *FindMenuItem(int ident);
private:
	FsGuiPopUpMenuItem *FindMenuItem(FsGuiPopUpMenu *menuPtr,int ident);

public:
	/*! This function really initializes the object. */
	void Initialize(void);

	/*! This function clears menu items, but does not change the location of the pop-up menu. */
	void CleanUp(void);

	/*! This function re-locates menu items so that the pull-down menu fits within the given width. 
	    Returns YSERR and does nothing if the menu is not a pull-down menu. */
	YSRESULT FitPullDownMenu(int wid);

	/*! This function re-locates sub-menu to the given location.  It recursively moves all sub-menus in the tree. */
	void MoveSubMenu(FsGuiPopUpMenu *subMenu,int x0,int y0);
private:
	void MoveSubMenuByOffset(FsGuiPopUpMenu *subMenu,int dx,int dy);

public:
	FsGuiPopUpMenuItem *GetLastSelectedItem(void);

public:
	/*! Adds a menu item.  The first parameters are menu identifier (an integer) and a short-cut key code.
	    When you don't need a menu identifier (in most cases you don't), give 0 as the menuId.
	    If you don't assign a short-cut key, give 0 or FSKEY_NULL as fsKey.
	    You can assign a call-back function here, but it is recommended to use BindCallBack function rather than assigning as the last parameters.
	*/
	FsGuiPopUpMenuItem *AddTextItem(int menuId,int fsKey,const char label[],void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)=NULL,void *contextPtr=NULL);
	FsGuiPopUpMenuItem *AddTextItem(int menuId,int fsKey,const wchar_t wLabel[],void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)=NULL,void *contextPtr=NULL);
	FsGuiPopUpMenuItem *AddBmpItem(int menuId,int fsKey,const YsBitmap &bmp,const wchar_t wLabel[],void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)=NULL,void *contextPtr=NULL);
protected:
	void SetUpMenuItem(
	    FsGuiPopUpMenuItem *item,int id,int fsKey,const wchar_t label[],int msgWid,int msgHei,int rightMsgWid,int rightMsgHei);


public:
	FsGuiPopUpMenuItem *FindItemInRange(int mx,int my) const;

	/*! Returns YSTRUE if (mx,my) is on the open sub menu.  This function checks recursively all of the open sub menus. */
	YSBOOL IsMouseOnOpenSubMenu(int mx,int my) const;

	FsGuiPopUpMenu *GetTopMostMenu(void);

	int GetWidth(void) const;
	int GetHeight(void) const;

	/*! This function sets the width of the menu in pixels. 
	    Size of the pull-down menu may need to be reset to the width of the canvas or the window.
	    This function can be used for that purpose. */
	void SetWidth(int wid);

	/*! This function returns the background color. */
	const YsColor &GetBackgroundColor(void) const;

	/*! This function changes the background color. */
	void SetBackgroundColor(const YsColor &newBackgroundColor);

	YSBOOL IsMouseOnMenu(int mx,int my) const;
	YSRESULT SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[]);
	void SetMouseOnItem(int mx,int my);
	void OpenSubMenu(FsGuiPopUpMenuItem *itm);
	FsGuiPopUpMenuItem *GetOpenSubMenu(void);
	const FsGuiPopUpMenuItem *GetOpenSubMenu(void) const;
	void CloseSubMenu(void);
	void CloseAllSubMenu(void);
	YSRESULT MouseButtonPressEvent(FsGuiPopUpMenuItem *&onItem,int mx,int my);
	YSRESULT MouseButtonReleaseEvent(FsGuiPopUpMenuItem *&onItem,int mx,int my);
	YSRESULT KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);

	/*! Process a virtual click of the GUI item that has the given identifier.
	*/
	virtual YSRESULT VirtualClick(const YsString &ident);
private:
	void ProcessClick(FsGuiPopUpMenuItem *item);

public:
	void Show(const class FsGuiDialog *excludeFromDrawing=NULL) const;
protected:
	void ShowItem(const FsGuiPopUpMenuItem *item,int rectX0,int rectY0,int rectX1,int rectY1) const;

public:
	virtual void OnOpenSubMenu(FsGuiPopUpMenuItem *item);
	virtual void OnCloseSubMenu(FsGuiPopUpMenuItem *item);
	virtual void OnSelectMenuItem(FsGuiPopUpMenuItem *item);
private:
	void InvokeCallBackIfAssigned(FsGuiPopUpMenuItem *item);
};


/* } */
#endif
