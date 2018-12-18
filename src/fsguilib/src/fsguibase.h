/* ////////////////////////////////////////////////////////////

File Name: fsguibase.h
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

#ifndef FSGUIBASE_IS_INCLUDED
#define FSGUIBASE_IS_INCLUDED
/* { */



#include <ysclass.h>
#include <ysbitmap.h>
#include <ysbitmaparray.h>


class FsGuiBitmapType : public YsBitmapArray
{
public:
	YSBOOL IsEmpty(void) const;
	YSBOOL IsLoaded(void) const;
	int GetWidth(void) const;
	int GetHeight(void) const;
	void MakeSmallEmptyBitmap(void);
	void SetBitmap(const YsBitmap &bmpIn);
};



class FsGuiMouseButtonSet
{
public:
	YSBOOL lb,mb,rb;
	FsGuiMouseButtonSet(){}
	FsGuiMouseButtonSet(YSBOOL l,YSBOOL m,YSBOOL r)
	{
		lb=l;
		mb=m;
		rb=r;
	}
};



////////////////////////////////////////////////////////////

class FsGuiObject
{
public:
	enum SCHEME
	{
		PLAIN,
		MODERN
	};

public:
	enum FSGUI_DRAWINGTYPE
	{
		FSGUI_DRAWINGTYPE_UNKNOWN,
		FSGUI_DRAWINGTYPE_NORMAL,
		FSGUI_DRAWINGTYPE_ACTIVE,
		FSGUI_DRAWINGTYPE_INACTIVE
	};

	static SCHEME scheme;

	static YsColor defBgCol,defFgCol;
	static YsColor defActiveBgCol,defActiveFgCol;
	static YsColor defFrameCol;

	static YsColor defDialogBgCol;

	static YsColor defTabBgCol;
	static YsColor defTabClosedBgCol;
	static YsColor defTabClosedFgCol;

	// used in MODERN scheme
	static double defRoundRadius;

	// for ListBox and DropList
	static YsColor defListBgCol,defListFgCol;
	static YsColor defListActiveBgCol,defListActiveFgCol;

	static class YsFontRenderer *defUnicodeRenderer;
	static class YsFontRenderer *defAsciiRenderer;

	static int defHSpaceUnit,defVSpaceUnit,defHAnnotation,defHScrollBar;

	static int defBitmapWidthLimit,defBitmapHeightLimit;

	static YSRESULT GetUnicodeCharacterBitmapSize(int &wid,int &hei);  // Based on the size of L"X"
	static YSRESULT RenderUnicodeString(YsBitmap &bmp,const char str[],const YsColor &fgCol,const YsColor &bgCol);
	static YSRESULT RenderUnicodeString(YsBitmap &bmp,const wchar_t wStr[],const YsColor &fgCol,const YsColor &bgCol);
	static YSRESULT GetTightUnicodeRenderSize(int &wid,int &hei,const wchar_t wStr[]);

	static YSRESULT GetAsciiCharacterBitmapSize(int &wid,int &hei);  // Based on the size of "X"
	static YSRESULT GetAsciiStringBitmapSize(int &wid,int &hei,const char *str);
	static YSRESULT RenderAsciiString(YsBitmap &bmp,const char str[],const YsColor &fgCol,const YsColor &bgCol);
	static YSRESULT RenderAsciiString(YsBitmap &bmp,const wchar_t str[],const YsColor &fgCol,const YsColor &bgCol);
	static YSRESULT GetTightAsciiRenderSize(int &wid,int &hei,const char str[]);

	class TouchMovement
	{
	public:
		YsVec2 from,to;
	};
	class TouchState
	{
	public:
		YsArray <TouchMovement> movement;
		YsArray <YsVec2> newTouch;
		YsArray <YsVec2> endedTouch;

		YSSIZE_T GetNumCurrentTouch(void) const
		{
			return movement.GetN()+newTouch.GetN();
		}
	};
	static TouchState MatchTouch(const YsArray <YsVec2> &newTouch,const YsArray <YsVec2> &prevTouch);
	static YsArray <YsVec2> ConvertTouch(YSSIZE_T nTouch,const class FsVec2i touch[]);

	/*! Force this class to be a polymorphic class.
	*/
	virtual void MakeItPolymorphic(void){};
};

////////////////////////////////////////////////////////////

class FsGuiFontRenderer
{
public:
	virtual YSRESULT RenderString(YsBitmap &bmp,const wchar_t wStr[],const YsColor &fgCol,const YsColor &bgCol) const=0;
	YSRESULT RenderString(YsBitmap &bmp,const char str[],const YsColor &fgCol,const YsColor &bgCol) const;
	virtual YSRESULT GetTightRenderSize(int &wid,int &hei,const wchar_t wStr[]) const=0;
};

class FsGuiAsciiFixedFontRenderer : public FsGuiFontRenderer
{
public:
	virtual int GetFontWidth(void) const=0;
	virtual int GetFontHeight(void) const=0;
	virtual YSRESULT RenderString(YsBitmap &bmp,const wchar_t wStr[],const YsColor &fgCol,const YsColor &bgCol) const=0;
	virtual YSRESULT GetTightRenderSize(int &wid,int &hei,const wchar_t wStr[]) const=0;
};

////////////////////////////////////////////////////////////

/*! This class is a base class for FsGuiCanvas, FsGuiPopUpmenu, and FsGuiDialog.
    */
class FsGuiControlBase : public FsGuiObject
{
private:
	FsGuiControlBase *parent;
	class FsGuiDialog *modalDlg;

	int identSource;
	YsAVLTree <YsString,int,YsStringComparer<YsString> > stringToIdent;

	YsString identString;

protected:
	YsArray <FsGuiControlBase *> scheduledForDeletion;

public:
	/*! Default constructor. */
	FsGuiControlBase();

	/*! Default destructor.  This needs to be virtual becasue some sub-classes may be deleted from the FsGuiControlBase pointer. */ 
	virtual ~FsGuiControlBase();

	/*! Clears ident to ID table. */
	void ClearStringToIdentTable(void);

	/*! Sets the identifier string. */
	void SetIdent(const YsString &str);

	/*! Gets the identifier string. */
	const YsString &GetIdent(void) const;

	/*! This function sets a pointer to the parent control-base object. */
	void SetParent(FsGuiControlBase *parent);

	/*! This function returns a pointer to the parent control-base object. */
	FsGuiControlBase *GetParent(void) const;

	/*! This function attaches a pointer to the modal dialog. Giving NULL to modalDlg will detach the dialog. */
	void AttachModalDialog(class FsGuiDialog *modalDlg);

	/*! This function appends a modal dialog on top of the chaing of the currently-attached modal dialogs.
	    If no modal dialog is attached before this function, this function works same as AttachModalDialog.
	*/
	void AppendModalDialog(class FsGuiDialog *modalDlg);

	/*! This function detaches the modal dialog.  Same effect as AttachModalDialog(NULL);. */
	void DetachModalDialog(void);

	/*! This function returns a pointer to the modal dialog attached to this control-base object. */
	class FsGuiDialog *GetModalDialog(void) const;

	/*! This function is called when a modal dialog that belongs to this object has been closed. */
    virtual void OnModalDialogClosed(int dialogIdent,class FsGuiDialog *closedModalDialog,int modalDialogEndCode);

	/*! Override this function to implement behavior of the Save button of the View-control dialog. 
	    (A tunnel for 3D-enabled applications.) */
	virtual YSRESULT OnSaveView(const class FsGui3DViewControlDialog *dlg);

	/*! Override this function to implement behavior of the Loae button of the View-control dialog.
	    (A tunnel for 3D-enabled applications.) */
	virtual YSRESULT OnLoadView(class FsGui3DViewControlDialog *dlg);

	/*! The framework (main loop) must call this function regularly. */
	virtual void Interval(void);

	/*! This function must return YSTRUE when a content of the object needs to be re-drawn. */
	virtual YSBOOL NeedRedraw(void) const=0;
	// Memo to myself: The condition for NeedRedraw may also depend on the child objects.  Therefore, it needs to be implemented per class.

	/*! This function sets need-redraw flag. */
	virtual void SetNeedRedraw(YSBOOL needRedraw)=0;

	/*! This function delets FsGuiControlBase sub-class objects that are scheduled for deletion. */
	void PerformScheduledDeletion(void);

	/*! Assign an integer identifier to the string, and returns it.
	    Every menu item and dialog item has an assigned ID.
	    In GUI debugging script, use the string to specify an item,
	    and convert it to the ID number by FindIdent.
	    It is analog to MAKEINTRESOURCE macro of Win32.
	    If the item name is already used, it returns -1.
	    Otherwise a positive number.
	*/
	int MakeIdent(const char itemName[]);

	/*! Alias for MakeIdent. */
	inline int MkId(const char itemName[])
	{
		return MakeIdent(itemName);
	}

	/*! Find an integer identifier created by MakeIdent from the item name.
	*/
	int FindIdent(const char itemName[]) const;

	/*! Process a command.
	    If the command is processed, it returns YSOK, or YSERR otherwise.
	*/
	virtual YSRESULT ProcessCommand(const YsString &str);

	/*! Process a virtual click of the GUI item that has the given identifier.
	*/
	virtual YSRESULT VirtualClick(const YsString &ident);

	/*! Virtually click a check box or a radio button if it is not checked.
	*/
	virtual YSRESULT VirtualCheck(const YsString &ident);

	/*! Virtually click a check box if it is checked.
	*/
	virtual YSRESULT VirtualUncheck(const YsString &ident);

	/*! Process a virtual selection of the GUI item.
	    The item must be list box or drop list.
	*/
	virtual YSRESULT VirtualSelect(const YsString &ident,const YsString &str);
	virtual YSRESULT VirtualSelect(const YsString &ident,int n);
};


/* } */
#endif
