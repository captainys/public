/* ////////////////////////////////////////////////////////////

File Name: fsguidialog.h
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

#ifndef FSGUIDIALOG_IS_INCLUDED
#define FSGUIDIALOG_IS_INCLUDED
/* { */

#include <functional>
#include <fssimplewindow.h>

#include "fsguibase.h"

typedef enum
{
	FSGUI_STATIC,
	FSGUI_BUTTON,
	FSGUI_TEXTBOX,
	FSGUI_LISTBOX,
	FSGUI_NUMBERBOX,
	FSGUI_DROPLIST,
	FSGUI_COLORPALETTE,
	FSGUI_SLIDER,
	FSGUI_TREECONTROL,
	FSGUI_CUSTOM,
} FSGUIITEMTYPE;

class FsGuiDialogItem : protected FsGuiObject
{
protected:
	YSBOOL enabled,isVisible;
	YSBOOL isTabStop;
	YSBOOL isText;
	YSBOOL newLine;
	YSBOOL useDefaultFrame;  // If YSFALSE, entirely custom-bmp.

	// Drawing cache >>
	mutable FSGUI_DRAWINGTYPE prevDrawingType;
	// Drawing cache <<

	~FsGuiDialogItem(void){}

	void DrawVerticalScrollBar(int x1,int y1,int x2,int y2,const YsColor &bgCol,const YsColor &fgCol) const;
	void DrawVerticalScrollBarSlider(int x1,int y1,int x2,int y2,int totalHeight,int showTop,int showHeight,const YsColor &bgCol,const YsColor &fgCol) const;
	void GetScrollBarBugPosition(int &bugX1,int &bugX2,int barX1,int barX2,int totalLength,int showTop,int showLength) const;
	int GetShowTopFromPosition(int barX1,int barX2,int mouseX,int totalLength,int showLength) const;

public:
	FSGUIITEMTYPE itemType;
	int id,x0,y0,wid,hei;
	int fsKey;

	YsWString wLabel;
	mutable FsGuiBitmapType bmp;  // <- It is essentially a cache.
protected:
	mutable FsGuiBitmapType negativeBmp;

public:
	class FsGuiDialog *owner;

	FsGuiDialogItem();
	void Initialize(FsGuiDialog *owner);

	/*! Returns an identification number assigned to this item. */
	int GetIdent(void) const;

	FSGUIITEMTYPE GetItemType(void) const;
	const YsWString &GetLabel(void) const;

	YSBOOL IsEnabled(void) const;
	YSBOOL IsTabStop(void) const;
	virtual YSBOOL IsInRange(int mx,int my) const;
	YSBOOL IsVisible(void) const;

	void Enable(void);
	void Disable(void);
	void SetEnabled(YSBOOL enabled);

	void SetUseDefaultFrame(YSBOOL useDefaultFrame);

	void SetNewLineWhenCreated(YSBOOL newLine);

	void Show(void); // YSTRUE -> isVisible
	void Hide(void); // YSFALSE -> isVisible

	void SetNegativeBitmap(const YsBitmap &negBmp);

	void DrawRect(YSBOOL focus,const YsColor &col) const;
	void DrawRightSideArrow(int x0,int y0,int x1,int y1,YSBOOL fill1,YSBOOL fill2,const YsColor &fillCol,const YsColor &bgCol,const YsColor &fgCol) const;
	YSBOOL IsInRightSideArrow(YSBOOL &up,YSBOOL &down,int mx,int my,int x0,int y0,int x1,int y1) const;

	/*! Returns the center.
	*/
	YsVec2i GetCenter(void) const;

	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual void DrawPopUp(const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual YSRESULT CharIn(int key);
	virtual void LButtonDown(int mx,int my);
	virtual void LButtonUp(int mx,int my);
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	virtual YSBOOL Highlight(int mx,int my);
	virtual YSBOOL NeedRedrawActiveItem(void) const;
};

class FsGuiStatic : public FsGuiDialogItem
{
private:
	YSBOOL frame,fill;
	YsColor fgCol,bgCol;

public:
	FsGuiStatic();
	void Initialize(FsGuiDialog *owner);
	void SetText(const char txt[]);
	void SetText(const wchar_t label[]);
private:
	void RenderOwnText(void);

public:
	void GetText(YsString &str) const;
	void GetText(YsWString &str) const;

	YsString GetText(void) const;
	YsWString GetWText(void) const;

	void SetFgColor(const YsColor &fgColIn);

	void SetDrawFrame(YSBOOL frame);
	void SetFill(YSBOOL fill);
	YSBOOL GetDrawFrame(void) const;
	YSBOOL GetFill(void) const;

	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
};

enum FSGUIBUTTONTYPE
{
	FSGUI_PUSHBUTTON,
	FSGUI_CHECKBOX,
	FSGUI_RADIOBUTTON
};

enum FSGUITEXTTYPE
{
	FSGUI_STRING,
	FSGUI_INTEGER,
	FSGUI_REALNUMBER,
	FSGUI_ASCIISTRING
};

class FsGuiButton : public FsGuiDialogItem
{
friend class FsGuiDialog;

protected:
	FSGUIBUTTONTYPE btnType;
	YSBOOL checked;
	YsArray <FsGuiButton *> btnGroup;

public:
	FsGuiButton();
	void Initialize(FsGuiDialog *owner);
	void SetCheck(YSBOOL checked);

	YSBOOL GetCheck(void) const;
	void CalculateButtonSizeFromMessageSize(int &btnWid,int &btnHei,int msgWid,int msgHei) const;

	void SetText(const char msg[]);
	void SetText(const wchar_t msg[]);

	void Click(void);

	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual void LButtonUp(int mx,int my);
};

class FsGuiTextBox : public FsGuiDialogItem
{
friend class FsGuiDialog;

public:
	enum LAYOUT
	{
		HORIZONTAL,
		VERTICAL
	};

private:
	YsWString txt;
	FsGuiBitmapType txtBmp;

	FSGUITEXTTYPE txtType;
	int lngLimit,nShow;
	YSBOOL change;
	YSBOOL selectAllWhenGetFocus;

	int msgX0,msgY0,msgWid,msgHei;
	int inputX0,inputY0,inputWid,inputHei;
	int typeX0,typeY0,typeWid,typeHei;

	mutable int lastCursorState;

	YSSIZE_T showTop;
	YSSIZE_T cursorPos;
	YSSIZE_T selBegin;  // -1:No selection   >=0:selBegin to cursorPos are selected.

	int cursorPixOffset,nextCursorPixOffset,selTopPixOffset,selEndPixOffset;

	YSBOOL eatEnterKey;

public:
	FsGuiTextBox();
	void Initialize(FsGuiDialog *owner);
	void SetTextType(FSGUITEXTTYPE txtType);
	void SetText(const char str[]);
	void SetText(const wchar_t str[]);
	void SetInteger(int number);
	void SetRealNumber(const double &number,int decimal);
	void SetLengthLimit(int lngLimit);
	void SelectAll(void);
	void UpdateShowTop(void);

	/*! If 'sw' is YSTRUE, this entire text in this text box will be selected when it gets focus. */
	void SetSelectAllWhenGetFocus(YSBOOL sw);

	/*! Set if this text box eats enter key stroke. */
	void SetEatEnterKey(YSBOOL eatEnterKey);

	/*! Check if this text box eats enter key. */
	YSBOOL CheckEatEnterKey(void) const;

	const YsString GetString(void) const;
	const YsWString &GetWString(void) const;

	void GetText(YsString &str) const;
	void GetText(YsWString &str) const;
	int GetInteger(void) const;
	double GetRealNumber(void) const;

	int GetSelectionRange(int &selTop,int &length) const;
	void GetSelectedText(YsString &str) const;
	void GetSelectedText(YsWString &str) const;
	YSSIZE_T GetPreviousWordPos(YSSIZE_T from) const;
	YSSIZE_T GetNextWordPos(YSSIZE_T next) const;
	void GetInputRect(int &x0,int &y0,int &x1,int &y1) const;
	void GetTypeRect(int &x0,int &y0,int &x1,int &y1) const;
	int GetShowLength(void) const;

	YSBOOL CheckAndClearTextChange(void);
	YSRESULT DeleteSelected(void);


	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual YSRESULT CharIn(int key);
	virtual YSBOOL NeedRedrawActiveItem(void) const;

private:
	void UpdatePixOffset(void);
	int GetPixOffset(YSSIZE_T pos) const;
	void RemakeBitmap(void);
	void JustGotFocus(void);
};

enum 
{
	FSGUI_SELECTED=1,
	FSGUI_DIRECTORY=2   // For file selector
};

class FsGuiListBoxItem
{
protected:
	YsWString str;
	FsGuiBitmapType bmp;
	YsColor fgCol,bgCol;

public:
	unsigned int flags;
	int intAttrib;

	FsGuiListBoxItem();
	void Initialize(void);
	void Select(void);
	void Unselect(void);
	void SetSelection(YSBOOL sel);
	void InvertSelection(void);

	int GetWidth(void) const;
	int GetHeight(void) const;
	void GetString(YsString &str) const;
	void GetString(YsWString &str) const;
	void SetString(const char str[]);
	void SetString(const wchar_t str[]);
	void SetFgColor(const YsColor &fgColorIn);
	void RemakeBitmap(void);
	const FsGuiBitmapType &GetBitmap(void) const;

	wchar_t GetFirstLetter(void) const;

	YSBOOL IsSelected(void) const;
	YSBOOL IsDirectory(void) const;
};

class FsGuiListBox : public FsGuiDialogItem
{
friend class FsGuiDialog;
friend class FsGuiDropList;

protected:
	YsArray <FsGuiListBoxItem> item;
	int showHeightPix,nShowWidth,showTopPix;
	int totalHeightPix,totalWidthPix;
	YSBOOL multi,tight;
	unsigned int nextRollClk,lastClickClk,lastChangeClk;
	YSBOOL selectionChange,doubleClick;
	YSBOOL captureScrollBar;

	int msgWid,msgHei;

public:
	FsGuiListBox();
	void Initialize(FsGuiDialog *owner);
	void SetMultiSelect(YSBOOL enableMultiSelect);
	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	void DrawPopUpFrame(const YsColor &col) const;
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual YSRESULT CharIn(int key);
	virtual void LButtonDown(int mx,int my);
	virtual void LButtonUp(int mx,int my);
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	void GetChoiceRect(int &x1,int &y1,int &x2,int &y2) const;
	void GetScrollBarRect(int &x1,int &y1,int &x2,int &y2) const;
	void GetScrollBarSliderRect(int &x1,int &y1,int &x2,int &y2) const;
	int GetChoiceByCoord(int mx,int my) const;
	YSBOOL IsScrollBarVisible(void) const;

	void ClearChoice(void);
	void SetChoice(YSSIZE_T nStr,const char *const str[]);
	void SetChoice(YSSIZE_T nStr,const YsString str[]);
	void SetChoice(YSSIZE_T nStr,const wchar_t *const str[]);
	void SetChoice(YSSIZE_T nStr,const YsWString str[]);
	void SetChoice(const YsConstArrayMask <YsString> &choiceArray);
	void SetChoice(const YsConstArrayMask <YsWString> &choiceArray);
private:
	template <class CHARTYPE>
	void SetChoiceTemplate(YSSIZE_T nStr,const CHARTYPE *const str[]);

public:
	int AddString(const char str[],YSBOOL select);
	int AddString(const wchar_t str[],YSBOOL select);
	YSRESULT SetString(int id,const char str[]);
	YSRESULT SetString(int id,const wchar_t str[]);
	YSRESULT DeleteString(int id);
	YSRESULT SetDirectoryFlag(int id,YSBOOL flag);
	YSRESULT SetIntAttrib(int id,int intAttrib);
	YSRESULT SetStringFgColor(int id,const YsColor &col);

	void Select(YSSIZE_T id);

	/*! Returns the index to the visible item at the top of the list box. */
	int GetShowTop(void) const;
	void SetShowTop(YSSIZE_T showTop);
	void SelectByString(const char str[],YSBOOL selectFirstIfNoMatch=YSFALSE);
	int GetSelection(void) const;
	YSBOOL IsSelected(int id) const;
	YSRESULT GetSelectedString(YsString &str) const;
	YSRESULT GetSelectedString(YsWString &str) const;
	YsString GetSelectedString(void) const;
	YsWString GetSelectedWString(void) const;
	int GetIntAttrib(int id) const;

	YSBOOL IsDirectory(int id) const;

	int GetNumChoice(void) const;
	YSRESULT GetString(YsString &str,int id) const;
	YSRESULT GetString(YsWString &str,int id) const;

	YSBOOL CheckAndClearSelectionChange(void);
	YSBOOL CheckAndClearDoubleClick(void);

	unsigned int GetLastChangeClock(void) const;

protected:
	int GetVerticalLocation(YSSIZE_T sel) const;
	int GetVerticalLocationAutoBound(YSSIZE_T sel) const;
	void AdjustShowTopFromNewSelection(YSSIZE_T newSel);
	int GetCurrentShowTop(void) const;
	int GetCurrentShowBottom(void) const;
	void ScrollUp(void);
	void ScrollDown(void);
	int GetAverageNShow(void) const;
};

class FsGuiDropList : public FsGuiDialogItem
{
friend class FsGuiDialog;

protected:
	int sel,nWidth;
	YSBOOL popUp;
	FsGuiListBox lbx;
	FsGuiBitmapType bmp;
	YSBOOL selectionChange;

public:
	FsGuiDropList();

	void ClearChoice(void);
	void SetChoice(YSSIZE_T nStr,const char *const str[]);
	void SetChoice(YSSIZE_T nStr,const YsString str[]);
	void SetChoice(YSSIZE_T nStr,const wchar_t *const str[]);
	void SetChoice(YSSIZE_T nStr,const YsWString str[]);
	void SetChoice(const YsConstArrayMask <YsString> &choiceArray);
	void SetChoice(const YsConstArrayMask <YsWString> &choiceArray);
	int AddString(const char str[],YSBOOL select);
	int AddString(const wchar_t str[],YSBOOL select);
	YSRESULT SetString(int id,const char str[]);
	YSRESULT SetString(int id,const wchar_t str[]);
	YSRESULT DeleteString(int id);

	virtual YSBOOL IsInRange(int mx,int my) const;

	void Select(int id);
	void SelectByString(const char str[],YSBOOL selectFirstIfNoMatch=YSFALSE);
	int GetSelection(void) const;
	YSRESULT GetSelectedString(YsString &str) const;
	YSRESULT GetSelectedString(YsWString &str) const;
	YsString GetSelectedString(void) const;
	YsWString GetSelectedWString(void) const;
	YSRESULT SetIntAttrib(int id,int intAttrib);
	YSRESULT GetString(YsString &str,int idx) const;
	YSRESULT GetString(YsWString &str,int idx) const;

	YSBOOL CheckAndClearSelectionChange(void);
	int GetIntAttrib(int id) const;
	int GetNumChoice(void) const;

	void Initialize(FsGuiDialog *owner);
	virtual void DrawPopUp(const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual void LButtonDown(int mx,int my);
	virtual void LButtonUp(int mx,int my);
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual YSRESULT CharIn(int key);
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

protected:
	void RemakeBitmap(void);
};

class FsGuiNumberBox : public FsGuiDialogItem
{
friend class FsGuiDialog;

protected:
	int num,min,max,step;
	unsigned int nextRollClk;
	YSBOOL increased,decreased;
	int labelWidth;

	int msgWid,msgHei,digitWid,digitHei;

	mutable YSBOOL mouseOnIncreaseButton;
	mutable YSBOOL mouseOnDecreaseButton;

public:
	FsGuiNumberBox();
	void Initialize(FsGuiDialog *owner);
	void SetNumberAndRange(int num,int min,int max,int step);
	void SetNumber(int num);

	int GetNumber(void) const;
	void GetIncreaseButtonArea(int &x0,int &y0,int &x1,int &y1) const;
	void GetDecreaseButtonArea(int &x0,int &y0,int &x1,int &y1) const;

	YSBOOL CheckAndClearNumberChange(void);

	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual YSRESULT CharIn(int key);
	virtual void LButtonDown(int mx,int my);
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
};

class FsGuiColorPalette : public FsGuiDialogItem
{
friend class FsGuiDialog;

public:
	enum
	{
		NUM_PALETTE=16
	};

protected:
	int col[3],step;
	int palette[NUM_PALETTE][3];
	YSBOOL arrow[3][2],hasComponentButton,hasColorButton;
	unsigned int nextRollClk;
	int cursor;

private:
	class FsGuiColorDialog *colDlg;

public:
	FsGuiColorPalette();
	~FsGuiColorPalette();
	void Initialize(FsGuiDialog *owner);
	void SetColor(const YsColor &col);
	void SetPalette(int id,const YsColor &col);

	void InvokeOnColorPaletteChange(void);

	const YsColor GetColor(void) const;
	const YsColor &GetColor(YsColor &col) const;
	const YsColor &GetPalette(YsColor &col,int id) const;

	YSBOOL HasColorButton(void) const;

	void GetColorRect(int &x0,int &y0,int &x1,int &y1)const;
	void GetRedRect(int &x0,int &y0,int &x1,int &y1)const;
	void GetGreenRect(int &x0,int &y0,int &x1,int &y1)const;
	void GetBlueRect(int &x0,int &y0,int &x1,int &y1)const;
	void GetComponentRect(int &x0,int &y0,int &x1,int &y1,int c)const;
	void GetPaletteRect(int &x0,int &y0,int &x1,int &y1)const;

	YSRESULT LoadFile(const char fn[]);
	YSRESULT SaveFile(const char fn[]) const;

	YSRESULT LoadFile(YsTextInputStream &inStream);
	YSRESULT SaveFile(YsTextOutputStream &outStream) const;

	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual void LButtonDown(int mx,int my);
	virtual void LButtonUp(int mx,int my);
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
};

class FsGuiSlider : public FsGuiDialogItem
{
protected:
	double min,max;
	double position; // 0.0 to 1.0
	double step;

	int cursorSize;  // in pixels

	YSBOOL dragging,horizontal;
	int dragMouseOffsetX,dragMouseOffsetY;

public:
	FsGuiSlider();
	void Initialize(FsGuiDialog *owner);

	void SetHorizontal(YSBOOL hor);

	const double &GetPosition(void) const;  // 0.0 to 1.0
	const double GetScaledValue(void) const; // min to max
	void GetSliderRect(int &x0,int &y0,int &x1,int &y1)const;
	int GetCursorSize(void) const;
	void GetCursorMovingRange(int &min,int &max) const;
	void GetCursorCenter(int &x,int &y) const;
	void GetCursorRect(int &x0,int &y0,int &x1,int &y1) const;
	double CalculatePositionFromScreenCoord(int x,int y) const;

	void SetMinMax(const double &min,const double &max);  // Remark: Min can be larger than max.
	void SetPosition(const double &pos);
	void SetPositionByScaledValue(const double value);
	void Increment(void);
	void Decrement(void);
	void SetStep(const double &step);

	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual void LButtonDown(int mx,int my);
	virtual void LButtonUp(int mx,int my);
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
};

class FsGuiTabControl : public FsGuiDialogItem
{
// Remark:
//   Height of the tab control is the height of the label area.
//   It doesn't include the field in which the items are placed.

friend class FsGuiDialog;  // Need to use AddTab, AddBmpTab

	class FsGuiTab
	{
	public:
		YSBOOL enabled,isVisible,isString;
		YsWString wLabel;

		mutable FsGuiBitmapType bmp; // <- Is essentially a cache.


		int labelWid,labelHei;
		int lastX,lastY;

		YsArray <FsGuiDialogItem *> tabItem;
		YsArray <class FsGuiGroupBox *> tabGrpBox;

		void Initialize(void);
	};

protected:
	YsArray <FsGuiTab> tabList;
	int currentTab;
	int mouseOnTab;

	int fieldHei;  // Height the item field + the label area.


	int AddTab(const char str[]);
	int AddTab(const wchar_t str[]);
	int AddBmpTab(const YsBitmap &tabBmp,const wchar_t altStr[]);

protected:
	void ExpandTab(void);
	void UpdateTab(void);

public:
	FsGuiTabControl();
	void Initialize(FsGuiDialog *owner);

	/*! Returns the index to the current tab. */
	int GetCurrentTab(void) const;

	void SelectCurrentTab(int tabId);
	void SelectFirstTab(void);
	void SelectNextTab(void);
	void SelectPrevTab(void);

	void EnableTab(int tabId);
	void DisableTab(int tabId);

	/*! Refresh show/hide status of the items that belong to the tab.
	    Tab items of hidden tabs can be made visible by calling Show/Hide functions individually.
	    However, such show/hide function calls may make the state invalid.
	    This function will refresh show/hide status based on the current selection of the tab. */
	void Refresh(void);

	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual void LButtonDown(int mx,int my);
	virtual void LButtonUp(int mx,int my);
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
};

////////////////////////////////////////////////////////////

/*! This class is a node of the FsGuiTreeControl. */
class FsGuiTreeControlNode
{
friend class FsGuiTreeControl;

public:
	enum
	{
		FLAG_OPEN=1,
		FLAG_SELECTED=2
	};

private:
	FsGuiTreeControlNode *parent;
	YsArray <FsGuiTreeControlNode *> *child;
	unsigned int flags;
	mutable int x,y;
	YsWString label;
	FsGuiBitmapType bitmap;
	mutable int intAttrib;

public:
	/*! Default constructor */
	FsGuiTreeControlNode();

	/*! Default destructor */
	~FsGuiTreeControlNode();

private:
	void Initialize(void);  // Only called from FsGuiTreeControl
	void SetSelect(YSBOOL sel);  // Only called from FsGuiTreeControl
	void Select(void);  // Only called from FsGuiTreeControl
	void Unselect(void);  // Only called from FsGuiTreeControl

public:
	/*! Returns whether the node is selected. */
	YSBOOL IsSelected(void) const;

	/*! Returns number of immediate child nodes. */
	int GetNChild(void) const;

	/*! Returns a pointer to the nth child node.  The index of the first child node is zero. 
	    It returns NULL if index is out of range. */
	const FsGuiTreeControlNode *GetChild(YSSIZE_T index) const;

	/*! Returns a pointer to the parent node. */
	const FsGuiTreeControlNode *GetParent(void) const;

	/*! Returns YSTRUE if the node is open, or YSFALSE otherwise. */
	YSBOOL IsOpen(void) const;

	/*! Returns YSTRUE if the parent node is open, or YSFALSE otherwise. */
	YSBOOL IsParentOpen(void) const;

	/*! Returns the label of the node in wStr. */
	void GetLabel(YsWString &wStr) const;

	/*! Sets an integer attribute.  It can be used for utility purpose (eg. hash key for connecting the node with other data structures.) */
	void SetIntAttrib(int attrib) const;

	/*! Gets an integer attribute. */
	int GetIntAttrib(void) const;

private:
	void SetOpen(YSBOOL sw); // Only called from FsGuiTreeControl
	void SetLabel(const wchar_t label[]); // Only called from FsGuiTreeControl
	void Render(void); // Only called from FsGuiTreeControl
	FsGuiTreeControlNode *AddChild(FsGuiTreeControlNode *child); // Only called from FsGuiTreeControl
	FsGuiTreeControlNode *GetChild(YSSIZE_T index); // Only called from FsGuiTreeControl
	void SetPosition(int x,int y) const; // Only called from FsGuiTreeControl
	const FsGuiBitmapType &GetBitmap(void) const;
	int LowerY(void) const;
	int Width(void) const;
	int Height(void) const;
	YSBOOL IsDescendantOf(const FsGuiTreeControlNode *ofThis) const;
};

class FsGuiTreeControl : public FsGuiDialogItem
{
private:
	class NodeAndIndex
	{
	public:
		const FsGuiTreeControlNode *node;
		int childIndex;
	};

	FsGuiTreeControlNode rootNode;
	YsSegmentedArray <FsGuiTreeControlNode,4> nodeAlloc;

	enum
	{
		FLAG_MULTISELECT=1,
		FLAG_CANDRAGNODE=2,
		FLAG_SHOWROOT=4
	};
	unsigned int flags;

	enum
	{
		STATE_NONE=0,
		STATE_CAPTURESCROLLBAR=1,
		STATE_CAPTUREDRAGNODE=2
	};

	int xScroll,yScroll;
	int vSpace;
	unsigned int nextRollClk;
	unsigned int state;
	const FsGuiTreeControlNode *cursorPos;
	const FsGuiTreeControlNode *dragTo;

	int mx0,my0,myOffset;  // For scroll bar
	int lastMx,lastMy;     // For drawing dragging image

	mutable YSBOOL needReRender;
	mutable YSBOOL needRePosition;

	mutable int contentWid,contentHei;
	mutable int boxWid;

public:
	/*! Default constructor */
	FsGuiTreeControl();

	/*! Default destructor */
	~FsGuiTreeControl();

	/*! Initializes the tree control */
	void Initialize(FsGuiDialog *owner);

	/*! Cleans up the tree control. */
	void CleanUp(void);

private:
	void LocalInitialize(void);
	void GetVerticalScrollBarRect(int &x1,int &y1,int &x2,int &y2) const;
	void GetVerticalScrollBarSliderRect(int &x1,int &y1,int &x2,int &y2) const;
	void GetNodeRect(int &x1,int &y1,int &x2,int &y2,const FsGuiTreeControlNode *node) const;

	void SetCaptureScrollBar(YSBOOL sw);
	YSBOOL CaptureScrollBar(void) const;
	void SetCaptureDragNode(YSBOOL sw,int mx,int my);
	YSBOOL CaptureDragNode(void) const;

	YSBOOL IsCursorWithinNode(const FsGuiTreeControlNode *node,int x,int y) const;
	void TransferNode(FsGuiTreeControlNode *thisNode,FsGuiTreeControlNode *newParent);

public:
	/*! Returns a pointer to the root node. */
	const FsGuiTreeControlNode *GetRoot(void) const;

	/*! Adds a child node and returns a pointer to the new child node. */
	const FsGuiTreeControlNode *AddChild(const FsGuiTreeControlNode *parentNode);

	/*! Adds a child node of a text and returns a pointer to the new child node. */
	const FsGuiTreeControlNode *AddTextChild(const FsGuiTreeControlNode *parentNode,const wchar_t str[]);

	/*! Adds a child node of a text and returns a pointer to the new child node. */
	const FsGuiTreeControlNode *AddTextChild(const FsGuiTreeControlNode *parentNode,const char str[]);

	/*! Set label of the child node. */
	void SetNodeLabel(const FsGuiTreeControlNode *node,const wchar_t label[]);

	/*! Set label of the child node. */
	void SetNodeLabel(const FsGuiTreeControlNode *node,const char label[]);

	/*! Open node. */
	void OpenNode(const FsGuiTreeControlNode *node);

	/*! Close node. */
	void CloseNode(const FsGuiTreeControlNode *node);

	/*! Set scroll in Y direction in pixels. */
	void SetYScroll(int yScroll);

	/*! Returns a pointer to the node shown at the window coordinate (x,y). */
	const FsGuiTreeControlNode *FindNodeAtWindowCoord(int x,int y) const;

	/*! Turn on/off multi-select mode. */
	void SetMultiSelect(YSBOOL sw);
private:
	YSBOOL MultiSelectMode(void) const;

public:
	/*! Turn on/off can-drag status. If can-drag status is on, the user can drag 
	    nodes around. */
	void SetCanDragNode(YSBOOL sw);
private:
	YSBOOL CanDragNode(void) const;

public:
	/*! Turn on/off show-root. This affect the result from GetAllVisibleNode.  */
	void SetShowRoot(YSBOOL sw);
private:
	YSBOOL ShowRoot(void) const;

public:
	/*! Returns a pointer to the node that is shown at the top of the tree control. */
	const FsGuiTreeControlNode *GetShowTopNode(void) const;
private:
	const FsGuiTreeControlNode *FindShowTopNode(const FsGuiTreeControlNode *node) const;

public:
	/*! Returns a pointer to the node that is visible in the second row of the tree control. */
	const FsGuiTreeControlNode *GetSecondVisibleNode(void) const;
private:
	const FsGuiTreeControlNode *FindSecondVisibleNode(const FsGuiTreeControlNode *current,const FsGuiTreeControlNode *nextOf,const FsGuiTreeControlNode *&prev) const;

public:
	/*! Returns a pointer to the node that is one row above the top edge of the tree control box. */
	const FsGuiTreeControlNode *GetLastHiddenNode(void) const;
private:
	const FsGuiTreeControlNode *FindLastHiddenNode(const FsGuiTreeControlNode *current,const FsGuiTreeControlNode *showTop,const FsGuiTreeControlNode *&prev) const;


public:
	/*! Scrolls up by one row. */
	void ScrollUp(void);

	/*! Scrolls up by one row. */
	void ScrollDown(void);

	/*! Unselect all nodes. */
	void UnselectAll(void);

	/*! Returns all visible nodes.  Visible node may not be in the tree-control box, but
	    will become visible when the user scrolls the scroll bar.  Nodes are sorted in the
	    order in which they are drawn in the tree-control box.  */
	template <const int N>
	void GetAllVisibleNode(YsArray <const FsGuiTreeControlNode *,N> &nodeArray) const;

	/*! Returns all selected nodes. */
	template <const int N>
	void GetAllSelectedNode(YsArray <const FsGuiTreeControlNode *,N> &nodeArray) const;

	/*! Returns a selected node.  If multi-selection mode is turned on, it returns one
	    of the selected nodes.  If no node is selected, it returns NULL. */
	const FsGuiTreeControlNode *GetSelection(void) const;

public:
	/*! Reaction for events. */
	virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	/*! Reaction for events. */
	virtual YSRESULT CharIn(int key);
	/*! Reaction for events. */
	virtual void LButtonDown(int mx,int my);
	/*! Reaction for events. */
	virtual void LButtonUp(int mx,int my);
	/*! Reaction for events. */
	virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

private:
	void RePosition(void) const;
	void RePosition(int &x,int &y,const FsGuiTreeControlNode *node,int fontWid,int fontHei) const;

public:
	/*! Draws the tree control */
	virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
private:
	void DrawHighlightRecursive(const YsColor &col) const;
	void DrawHighlightOneNode(const FsGuiTreeControlNode *node,const YsColor &col) const;
	void DrawRecursive(const FsGuiTreeControlNode *node,const YsColor &bgCol,const YsColor &fgCol) const;
	void DrawOneNode(const FsGuiTreeControlNode *node,const YsColor &bgCol,const YsColor &fgCol) const;
};

template <const int N>
void FsGuiTreeControl::GetAllVisibleNode(YsArray <const FsGuiTreeControlNode *,N> &nodeArray) const
{
	nodeArray.Clear();
	YsArray <NodeAndIndex> stack;

	if(YSTRUE==ShowRoot())
	{
		nodeArray.Append(&rootNode);
	}

	stack.Increment();
	stack.GetEnd().node=&rootNode;
	stack.GetEnd().childIndex=0;
	while(0<stack.GetN())
	{
		if(stack.GetEnd().node->GetNChild()<=stack.GetEnd().childIndex)
		{
			stack.DeleteLast();
		}
		else
		{
			const int childIndex=stack.GetEnd().childIndex++;
			const FsGuiTreeControlNode *child=stack.GetEnd().node->GetChild(childIndex);
			nodeArray.Append(child);
			if(YSTRUE==child->IsOpen() && 0<child->GetNChild())
			{
				stack.Increment();
				stack.GetEnd().node=child;
				stack.GetEnd().childIndex=0;
			}
		}
	}
}

template <const int N>
void FsGuiTreeControl::GetAllSelectedNode(YsArray <const FsGuiTreeControlNode *,N> &nodeArray) const
{
	nodeArray.Clear();

	if(YSTRUE==rootNode.IsSelected())
	{
		nodeArray.Append(&rootNode);
	}

	YsArray <NodeAndIndex> stack;

	stack.Increment();
	stack.GetEnd().node=&rootNode;
	stack.GetEnd().childIndex=0;
	while(0<stack.GetN())
	{
		if(stack.GetEnd().node->GetNChild()<=stack.GetEnd().childIndex)
		{
			stack.DeleteLast();
		}
		else
		{
			const int childIndex=stack.GetEnd().childIndex++;
			const FsGuiTreeControlNode *child=stack.GetEnd().node->GetChild(childIndex);
			if(YSTRUE==child->IsSelected())
			{
				nodeArray.Append(child);
			}
			if(0<child->GetNChild())
			{
				stack.Increment();
				stack.GetEnd().node=child;
				stack.GetEnd().childIndex=0;
			}
		}
	}
}

////////////////////////////////////////////////////////////

class FsGuiGroupBox
// Note to myself: It's not a GUI item.  Group Boxes must be drawn before other GUI items to stay background.
//                 Group box must be set up after Fit() function is called.
{
public:
	int n,x0,y0,x1,y1;
	YSBOOL visible;

	FsGuiGroupBox();
	void Initialize(void);
	void AddGuiItem(const FsGuiDialogItem *itm);

	void Show(void);
	void Hide(void);
	YSBOOL IsVisible(void) const;
};

////////////////////////////////////////////////////////////

enum FSDIALOGARRANGETYPE
{
	FSDIALOG_NOARRANGE,
	FSDIALOG_ARRANGE_TOP_LEFT,
	FSDIALOG_ARRANGE_TOP_RIGHT,
	FSDIALOG_ARRANGE_BOTTOM_LEFT,
	FSDIALOG_ARRANGE_BOTTOM_RIGHT,
	FSDIALOG_ARRANGE_CENTER_CENTER,
	FSDIALOG_ARRANGE_TOP_CENTER,
	FSDIALOG_ARRANGE_BOTTOM_CENTER,
};

class FsGuiDialog : public FsGuiControlBase
{
public:
	/*! This function creates a self-destructive dialog.
	    A self-destructive modal dialog will be deleted by the delete operator when the modal-dialog is closed.
	    The deletion may be performed whenever the parent object's PeformScheduledDeletion function is called.
	    PerformScheduledDeletion will be called in the destructor latest.

		Currently, PerformScheduledDeletion is called at the beginning of KeyIn, SetMouseState, and Interval.
	*/
	template <class T>
	static T *CreateSelfDestructiveDialog(void)
	{
		auto *dlgPtr=new T;
		auto *basePtr=dynamic_cast <FsGuiDialog *>(dlgPtr);
		if(NULL==basePtr)
		{
			delete dlgPtr;
			return NULL;
		}
		else
		{
			basePtr->isSelfDestructiveDialog=YSTRUE;
			return dlgPtr;
		}
	}

	YSBOOL IsSelfDestructiveDialog(void) const
	{
		return isSelfDestructiveDialog;
	}


public:
	using FsGuiControlBase::GetParent;
	using FsGuiControlBase::SetParent;
	using FsGuiControlBase::GetModalDialog;
	using FsGuiControlBase::AttachModalDialog;

private:
	FSDIALOGARRANGETYPE arrangeType;
	YSBOOL isSelfDestructiveDialog;
	YSBOOL isPermanent;

protected:
	mutable YSBOOL needRedraw;
	YSBOOL endModal;
	YSRESULT result;

	void (*closeModalCallBackFunc)(FsGuiControlBase *context,FsGuiDialog *closedModalDialog,int returnCode);
	std::function <void (int)> stdCloseModalCallBack;

	YsListAllocator <FsGuiButton> btnAlloc;
	YsListContainer <FsGuiButton> btnList;

	YsListAllocator <FsGuiTextBox> txtAlloc;
	YsListContainer <FsGuiTextBox> txtList;

	YsListAllocator <FsGuiListBox> lstAlloc;
	YsListContainer <FsGuiListBox> lstList;

	YsListAllocator <FsGuiDropList> drpAlloc;
	YsListContainer <FsGuiDropList> drpList;

	YsListAllocator <FsGuiStatic> sttAlloc;
	YsListContainer <FsGuiStatic> sttList;

	YsListAllocator <FsGuiNumberBox> nbxAlloc;
	YsListContainer <FsGuiNumberBox> nbxList;

	YsListAllocator <FsGuiColorPalette> cplAlloc;
	YsListContainer <FsGuiColorPalette> cplList;

	YsListAllocator <FsGuiSlider> sldAlloc;
	YsListContainer <FsGuiSlider> sldList;

	YsSegmentedArray <FsGuiTreeControl,4> treeAlloc;

	YsListAllocator <FsGuiTabControl> tabAlloc;
	YsListContainer <FsGuiTabControl> tabList;


	// Note to myself: Group Boxes are not GUI items.
	YsListAllocator <FsGuiGroupBox> grpBoxAlloc;
	YsListContainer <FsGuiGroupBox> grpBoxList;

	int dlgX0,dlgY0,dlgWid,dlgHei;
	int itemX0,itemY0;

	FsGuiTabControl *makingTab;
	int makingTabId;
	int lastX,lastY;

	YSBOOL enabled;
	YSBOOL autoNewColumn;

	YsWString txtMessage;
	FsGuiBitmapType bmpMessage;
	int msgWid,msgHei;

	int pMx,pMy;
	YSBOOL pLb,pMb,pRb;

	YsArray <YsVec2> prevTouch;

	YsColor dialogBgCol;
	YsColor tabBgCol;
private:
	YsColor bgCol,fgCol;
protected:
	YsColor activeBgCol,activeFgCol;
	YsColor frameCol;

	YsArray <FsGuiDialogItem *> itemList;

	FsGuiDialogItem *focus,*mouseOver,*clicked;
	FsGuiDialogItem *popUp;

	FsGuiTabControl *primaryTab;  // <- This tab receives Ctrl+TAB, Ctrl+Shift+TAB

	FsGuiDialog *parentDlg,*foregroundDlg; // <- To achieve modal dialog in iOS

	int modalDialogIdent;

public:


	// FsGuiBitmapType bmpMessage;



	FsGuiDialog();
	virtual ~FsGuiDialog();

	/*! Initializes the dialog.  It also clears the close-modal call back function.
	*/
	void Initialize(void);

	/*! Removes all items on the dialog, and move the next-item location to top-left.
	    It also removes internal references to the items (not to crash).
	    This function retains other states as is.
	*/
	void RemoveAllItem(void);

	void SetCloseModalCallBack(void (*closeModalCallBackFunc)(FsGuiControlBase *,FsGuiDialog *,int));

	// Set up a call back to void Foo::CallBack(FsGuiDialog *closedModalDialog,int returnValue)
	template <typename objType>
	void BindCloseModalCallBack(void (objType::*func)(FsGuiDialog *,int),objType *obj)
	{
		stdCloseModalCallBack=std::bind(func,obj,this,std::placeholders::_1);
	}

	void UnbindCloseModalCallBack(void);


	/*! Set permanent-dialog flag.  A permanent dialog is a dialog that stays regardless of the mode of the application.
	    A non-permanent dialog can be closed by calling RemoveAllNonPermanentDialog() function.  
	    A permanent dialog can still be closed by explicitly calling RemoveDialog function.
	    This flag has no effect on modal dialogs. */
	void SetIsPermanent(YSBOOL f);

	/*! Returns */
	YSBOOL IsPermanent(void) const;

	void SetTopLeftCorner(int x,int y);
	void SetSize(int w,int h);
	void SetTransparency(YSBOOL trsp);
	void SetBackgroundAlpha(const double alpha);
	void SetBackgroundColor(const YsColor &col);
	void AlignRowMiddle(const YsArray <FsGuiDialogItem *> &dlgItemRow);
	void AlignColumnRight(const YsArray <FsGuiDialogItem *> &dlgItemColumn);
	void AlignLeftMiddle(const YsArray <YsArray <FsGuiDialogItem *> > &dlgItemMatrix);
	void Fit(void);
	void Move(int newX0,int newY0);
	void SetTextMessage(const char msg[]);
	void SetTextMessage(const wchar_t msg[]);
	void StepToNextColumn(void);
	virtual void Show(const FsGuiDialog *excludeFromDrawing=NULL) const;
	void SetFocus(FsGuiDialogItem *focus);
	FsGuiDialogItem *GetFocus(void) const;

	int GetLeftX(void) const;
	int GetTopY(void) const;
	int GetWidth(void) const;
	int GetHeight(void) const;
	YSBOOL GetTransparency(void) const;


	/*! This function returns a pointer to a FsGuiDialogItem where (mx,my) is lying on regardless of the item is visible or hidden. */
	FsGuiDialogItem *FindItem(int mx,int my);

	/*! This function returns a pointer to a FsGuiDialogItem where (mx,my) is lying excluding the hidden items. */
	FsGuiDialogItem *FindVisibleItem(int mx,int my);

	/*! Returns the active modal dialog, which is the most-foreground modal dialog. */
	FsGuiDialog *GetActiveModalDialog(void) const;

	YSBOOL IsMouseOnDialog(int mx,int my) const;
	YSRESULT SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[]);
	void ForgetPreviousMouseButtonState(void);
	void SetPopUp(FsGuiDialogItem *popUpItem);
	FsGuiDialogItem *LButtonDown(int mx,int my);
	FsGuiDialogItem *LButtonUp(int mx,int my);
	YSRESULT KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	YSRESULT CharIn(int c);
	FsGuiDialogItem *GetClickedItem(void);

	/*! Process a virtual click of the GUI item that has the given identifier.
	*/
	virtual YSRESULT VirtualClick(const YsString &ident);

	/*! Virtually click a check box or a radio button if it is not checked.
	*/
	virtual YSRESULT VirtualCheck(const YsString &ident);

	/*! Virtually click a check box or a radio button if it is not checked.
	*/
	virtual YSRESULT VirtualUncheck(const YsString &ident);

private:
	void ProcessClick(FsGuiDialogItem *item);

public:
	virtual YSRESULT VirtualSelect(const YsString &ident,const YsString &str);
	virtual YSRESULT VirtualSelect(const YsString &ident,int n);


public:
	virtual void SetNeedRedraw(YSBOOL needRedraw);
	virtual YSBOOL NeedRedraw(void) const;

	/*! Returns YSTRUE if the key stroke may be eaten by this dialog. */
	YSBOOL MayEatThisKeyStroke(int fskey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt) const;

	YSRESULT CloseModalDialog(int returnCode);

	void HideAll(void);
	void ShowAll(void);

	// Deprecated >>
	void StartModalDialog(void);
	YSBOOL GetEndModal(void) const;
	YSRESULT GetResult(void) const;
	// Deprecated <<

	/*! Called as soon as the dialog is attached to a FsGuiControlBase by AttachModalDialog or AddDialog. */
	virtual void OnAttach(void);

	/*! Called as soon as the dialog is detached from a FsGuiControlBase. */
	virtual void OnDetach(void);

	virtual void OnKeyDown(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnListBoxSelChange(FsGuiListBox *lbx,int prevSel);
	virtual void OnListBoxScroll(FsGuiListBox *lbx,int prevShowTop);
	virtual void OnListBoxDoubleClick(FsGuiListBox *lbx,int clickedOn);

	/*! Implement this function to handle selection-change event of a drop list. 
	*/
	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);
	virtual void OnNumberBoxChange(FsGuiNumberBox *nbx,int prevNum);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
	virtual void OnTextBoxSpecialKey(FsGuiTextBox *txt,int fskey); // For processing enter and esc
	virtual void OnMouseMove(
	    YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my,
	    YSBOOL plb,YSBOOL pmb,YSBOOL prb,int pmx,int pmy,
	    FsGuiDialogItem *mouseOver);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);
	virtual void OnSelectTab(FsGuiTabControl *tab,int newTab);

	virtual void OnTreeControlSelChange(FsGuiTreeControl *tree);
	virtual void OnTreeControlScroll(FsGuiTreeControl *tree);
	virtual void OnTreeControlNodeAboutToMove(FsGuiTreeControl *tree,int nNode,const FsGuiTreeControlNode * const node[]);
	virtual void OnTreeControlNodeMoved(FsGuiTreeControl *tree,int nNode,const FsGuiTreeControlNode * const node[]);

	/*! This function is called when a color palette changed. */
	virtual void OnColorPaletteChange(FsGuiColorPalette *plt);

	/*! This function adds a radio-button group.
	    Note: The pointers to the buttons need to be non-constant.  These button may change state depending on the user input. */
	void SetRadioButtonGroup(YSSIZE_T nBtn,FsGuiButton *btn[]);
	void SetRadioButtonGroup(const YsArrayMask <FsGuiButton *> btn);

	void InsertVerticalSpace(int height);

	// Static
public:
	FsGuiStatic *AddStaticText(int id,int fsKey,const char label[],int nWidth,int nLine,YSBOOL newLine);  // This reserves size for nWidth, nLine
	FsGuiStatic *AddStaticText(int id,int fsKey,const char label[],YSBOOL newLine);

	FsGuiStatic *AddStaticText(int id,int fsKey,const wchar_t label[],int nWidth,int nLine,YSBOOL newLine);  // This reserves size for nWidth, nLine
	FsGuiStatic *AddStaticText(int id,int fsKey,const wchar_t label[],YSBOOL newLine);

	FsGuiStatic *AddStaticBmp(int id,int fsKey,const YsBitmap &bmpIn,YSBOOL newLine);
protected:
	void SetUpStatic(FsGuiStatic *item,int msgWid,int msgHei,int id,int fsKey,YSBOOL newLine);

	// Button
public:
	FsGuiButton *AddTextButton(int id,int fsKey,FSGUIBUTTONTYPE btnType,const char label[],YSBOOL newLine);
	FsGuiButton *AddTextButton(int id,int fsKey,FSGUIBUTTONTYPE btnType,const wchar_t label[],YSBOOL newLine);
	FsGuiButton *AddBmpButton(int id,int fsKey,FSGUIBUTTONTYPE btnType,const YsBitmap &bmpIn,const wchar_t label[],YSBOOL newLine);
protected:
	void SetUpButton(FsGuiButton *item,int msgWid,int msgHei,FSGUIBUTTONTYPE btnType,int id,int fsKey,const wchar_t label[],YSBOOL newLine);

public:
	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddTextBox(int id,int fsKey,const char label[],int nShow,YSBOOL newLine);
	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddTextBox(int id,int fsKey,const wchar_t label[],int nShow,YSBOOL newLine);
	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddBmpTextBox(int id,int fsKey,const YsBitmap &bmpIn,const char label[],int nShow,YSBOOL newLine);

	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddTextBox(int id,int fsKey,const char label[],const char defTxt[],int nShow,YSBOOL newLine);
	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddTextBox(int id,int fsKey,const wchar_t label[],const char defTxt[],int nShow,YSBOOL newLine);
	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddBmpTextBox(int id,int fsKey,const YsBitmap &bmpIn,const char label[],const char defTxt[],int nShow,YSBOOL newLine);

	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddTextBox(int id,int fsKey,const char label[],const wchar_t defTxt[],int nShow,YSBOOL newLine);
	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddTextBox(int id,int fsKey,const wchar_t label[],const wchar_t defTxt[],int nShow,YSBOOL newLine);
	/*! Left for the backward compatibility.  Use the functions that takes layout as a parameter. */
	FsGuiTextBox *AddBmpTextBox(int id,int fsKey,const YsBitmap &bmpIn,const char label[],const wchar_t defTxt[],int nShow,YSBOOL newLine);


	/*! layout can be FsGuiTextBox::HORIZONTAL or FsGuiTextBox::VERTICAL
	*/
	FsGuiTextBox *AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const char label[],int nShow,YSBOOL newLine);
	FsGuiTextBox *AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const wchar_t label[],int nShow,YSBOOL newLine);
	FsGuiTextBox *AddBmpTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const YsBitmap &bmpIn,const char label[],int nShow,YSBOOL newLine);

	FsGuiTextBox *AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const char label[],const char defTxt[],int nShow,YSBOOL newLine);
	FsGuiTextBox *AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const wchar_t label[],const char defTxt[],int nShow,YSBOOL newLine);
	FsGuiTextBox *AddBmpTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const YsBitmap &bmpIn,const char label[],const char defTxt[],int nShow,YSBOOL newLine);

	FsGuiTextBox *AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const char label[],const wchar_t defTxt[],int nShow,YSBOOL newLine);
	FsGuiTextBox *AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const wchar_t label[],const wchar_t defTxt[],int nShow,YSBOOL newLine);
	FsGuiTextBox *AddBmpTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const YsBitmap &bmpIn,const char label[],const wchar_t defTxt[],int nShow,YSBOOL newLine);
protected:
	void SetUpTextBox(FsGuiTextBox *item,int msgWid,int msgHei,const wchar_t defTxt[],int nShow,FsGuiTextBox::LAYOUT layout,YSBOOL newLine);

public:
	FsGuiListBox *AddEmptyListBox(
	   int id,int fsKey,const char label[],int nShow,int nWidth,YSBOOL newLine);
	FsGuiListBox *AddEmptyListBox(
	   int id,int fsKey,const wchar_t label[],int nShow,int nWidth,YSBOOL newLine);
	FsGuiListBox *AddEmptyBmpListBox(
	   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],int nShow,int nWidth,YSBOOL newLine);

	// Three versions AddListBox (ASCII), AddListBox (UNICODE), and AddBmpListBox, and for each of them choice can be in ASCII or UNICODE
	FsGuiListBox *AddListBox(
	   int id,int fsKey,const char label[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,YSBOOL newLine);
	FsGuiListBox *AddListBox(
	   int id,int fsKey,const wchar_t label[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,YSBOOL newLine);
	FsGuiListBox *AddBmpListBox(
	   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,YSBOOL newLine);

	FsGuiListBox *AddListBox(
	   int id,int fsKey,const char label[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,YSBOOL newLine);
	FsGuiListBox *AddListBox(
	   int id,int fsKey,const wchar_t label[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,YSBOOL newLine);
	FsGuiListBox *AddBmpListBox(
	   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,YSBOOL newLine);
private:
	template <class CHARTYPE>
	FsGuiListBox *AddListBoxTemplate(
	   int id,int fsKey,const char label[],YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine);
	template <class CHARTYPE>
	FsGuiListBox *AddListBoxTemplate(
	   int id,int fsKey,const wchar_t label[],YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine);
	template <class CHARTYPE>
	FsGuiListBox *AddBmpListBoxTemplate(
	   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine);
	template <class CHARTYPE>
	void SetUpListBox(FsGuiListBox *item,int msgWid,int msgHei,YSSIZE_T nChoce,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine);

public:
	FsGuiNumberBox *AddNumberBox(
	   int id,int fsKey,const char label[],int labelWidth,int num,int min,int max,int step,YSBOOL newLine);
	FsGuiNumberBox *AddNumberBox(
	   int id,int fsKey,const wchar_t label[],int labelWidth,int num,int min,int max,int step,YSBOOL newLine);
	FsGuiNumberBox *AddBmpNumberBox(
	   int id,int fsKey,const YsBitmap &bmpIn,const char label[],int labelWidth,int num,int min,int max,int step,YSBOOL newLine);
protected:
	void SetUpNumberBox(FsGuiNumberBox *item,int msgWid,int msgHei,int labelWidth,int num,int min,int max,int step,YSBOOL newLine);

public:
	/*! This function adds an empty drop list. */
	FsGuiDropList *AddEmptyDropList(
	   int id,int fsKey,const char label[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine);

	/*! This function adds a drop list with choices. */
	FsGuiDropList *AddDropList(
	   int id,int fsKey,const char label[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine);

	/*! This function adds a drop list with choices. */
	FsGuiDropList *AddDropList(
	   int id,int fsKey,const char label[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine);

	/*! This function adds a drop list with choices. */
	FsGuiDropList *AddDropList(
	   int id,int fsKey,const wchar_t label[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine);
private:
	template <class CHARTYPE>
	FsGuiDropList *AddDropListTemplate(
	   int id,int fsKey,const YsWString &label,YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine);

public:
	FsGuiColorPalette *AddColorPalette(
	   int id,int fsKey,const char label[],int r,int g,int b,int step,YSBOOL colorBtn,YSBOOL newLine);

	FsGuiColorPalette *AddCompactColorPalette(int id,int fsKey,const char label[],int r,int g,int b,YSBOOL newLine);

	FsGuiSlider *AddHorizontalSlider(int id,int fsKey,int nWidth,const double &min,const double &max,YSBOOL newLine);
	FsGuiSlider *AddVerticalSlider(int id,int fsKey,int nHeight,const double &min,const double &max,YSBOOL newLine);

	FsGuiTreeControl *AddTreeControl(int id,int fsKey,int nShow,int nWidth,YSBOOL newLine);

public:
	/*! Adds a custom control.  The caller is responsible for allocating and de-allocating the custom control. */
	void AddCustomControl(int id,int fsKey,FsGuiDialogItem *item,int widInPixel,int heiInPixel,YSBOOL newLine);


	/*! Adds a tab control in the dialog and returns a pointer. */
	FsGuiTabControl *AddTabControl(int id,int fsKey,YSBOOL newLine);

	/*! Adds a tab in the tab control (tabCtrl).  str is the label of the tab. */
	int AddTab(FsGuiTabControl *tabCtrl,const char str[]);

	/*! Adds a tab in the tab control (tabCtrl).  str is the label of the tab. */
	int AddTab(FsGuiTabControl *tabCtrl,const wchar_t str[]);

	/*! Expands tabs so that tabs take entire width of the tab control. */
	void ExpandTab(FsGuiTabControl *tabCtrl);

	/*! Adds a tab in the tab control (tabCtrl).  bmpIn will be shown as the label of the tab.
	    str is a text label and is optional.  When the text label is not necessary, just give L"" to str.  */
	int AddBmpTab(FsGuiTabControl *tabCtrl,const YsBitmap &bmpIn,const wchar_t str[]);

public:
	/*! Starts adding dialog items in the tab.  Any dialog items added after this function before EndAddTabItem will be a member of the tab. */
	void BeginAddTabItem(FsGuiTabControl *tabCtrl,int tabId);

	/*! Ends adding dialog items in the tab. */
	void EndAddTabItem(void);



	FsGuiGroupBox *AddGroupBox(void);
protected:
	void ClickEquivalent(FsGuiDialogItem *item);
	void PlaceNewItem(FsGuiDialogItem *item,int itemWid,int itemHei,YSBOOL newLine);
	void SetIdKeyLabel(FsGuiDialogItem *item,int id,int fsKey,const wchar_t label[]);
	void SetIdKeyLabel(FsGuiDialogItem *item,int id,int fsKey,const char label[]);

public:
	void SetArrangeType(FSDIALOGARRANGETYPE arrangeType);
	FSDIALOGARRANGETYPE GetArrangeType(void) const;
};

/* } */
#endif
