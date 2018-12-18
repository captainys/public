/* ////////////////////////////////////////////////////////////

File Name: fssimplefiledialog_linux.cpp
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

#include <time.h>

#include <ysclass.h>
#include <ysport.h>
#include <fssimplewindow.h>
#include <yssystemfont.h>

#include "../fssimplefiledialog.h"

class FsSimpleDialog
{
public:
	class Widget
	{
	public:
		int x0,y0,wid,hei;
		FsSimpleDialog *owner;

		void DrawBackground(void) const;
		void DrawFrame(void) const;
		void DrawActiveFrame(void) const;
		void SetRect(int x0,int y0,int wid,int hei);
		YSBOOL IsInside(int x,int y) const;

		virtual void Draw(const Widget *activeWidget) const=0;
		virtual void KeyIn(int keyCode){}
		virtual void CharIn(int charCoe){}
		virtual void LButtonDown(int mx,int my){};
		virtual void MouseMove(int lb,int mb,int rb,int mx,int my){};
		virtual void LButtonUp(int mx,int my){};
	};
	class StaticText : public Widget
	{
	public:
		YsWString str;
		virtual void Draw(const Widget *activeWidget) const;
	};
	class TextInput : public Widget
	{
	public:
		YsWString str;
		virtual void Draw(const Widget *activeWidget) const;
		virtual void KeyIn(int keyCode);
		virtual void CharIn(int charCoe);
	};
	class Button : public Widget
	{
	public:
		YsWString str;
		virtual void Draw(const Widget *activeWidget) const;
	};
	class ListBox : public Widget
	{
	public:
		class Item
		{
		public:
			YsWString str;
			YSBOOL isDir,selected;
		};

		int showTop;
		YsArray <Item> item;
		int lastClickTime;
		YSSIZE_T lastSel;

		ListBox();
		void CleanUp(void);
		void AddItem(const wchar_t str[],YSBOOL isDir);
		YSSIZE_T GetNShow(void) const;
		YSSIZE_T PickedIndex(int mx,int my) const;
		YsWString GetSelectedString(void) const;
		YSBOOL SelectionIsDirectory(void) const;
		virtual void Draw(const Widget *activeWidget) const;
		virtual void KeyIn(int keyCode);
		virtual void CharIn(int charCode);
		virtual void LButtonDown(int mx,int my);
		virtual void LButtonUp(int mx,int my);
	};

	YsSystemFontCache fontCache;
	int fontHeight;

	YsSegmentedArray <StaticText,4> staticTextSource;
	YsSegmentedArray <TextInput,4> textInputSource;
	YsSegmentedArray <Button,4> buttonSource;
	YsSegmentedArray <ListBox,4> listBoxSource;
	YsArray <Widget *> widgetArray;
	Widget *activeWidget;



	FsSimpleDialog();
	~FsSimpleDialog();
	void CleanUp(void);

	void SetActive(Widget *widgetPtr);

	TextInput *AddTextInput(void);
	StaticText *AddStaticText(void);
	Button *AddButton(void);
	ListBox *AddListBox(void);

	void Draw(void);

	virtual void KeyIn(int keyCode);
	virtual void CharIn(int charCoe);
	virtual void LButtonDown(int mx,int my);
	virtual void MouseMove(int lb,int mb,int rb,int mx,int my);
	virtual void LButtonUp(int mx,int my);

	virtual void OnButtonClick(Button *btn){};
	virtual void OnListBoxSelChange(ListBox *lbx){};
	virtual void OnListBoxDoubleClick(ListBox *lbx){};
};

////////////////////////////////////////////////////////////

void FsSimpleDialog::Widget::DrawBackground(void) const
{
	glColor3ub(0,0,0);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2i(x0    ,y0);
	glVertex2i(x0+wid,y0);
	glVertex2i(x0+wid,y0+hei);
	glVertex2i(x0    ,y0+hei);
	glEnd();

}
void FsSimpleDialog::Widget::DrawFrame(void) const
{
	glLineWidth(1);

	glColor3ub(255,255,255);
	glBegin(GL_LINE_LOOP);
	glVertex2i(x0    ,y0);
	glVertex2i(x0+wid,y0);
	glVertex2i(x0+wid,y0+hei);
	glVertex2i(x0    ,y0+hei);
	glEnd();
}
void FsSimpleDialog::Widget::DrawActiveFrame(void) const
{
	glLineWidth(3);
	glColor3ub(255,255,255);
	glBegin(GL_LINE_LOOP);
	glVertex2i(x0    ,y0);
	glVertex2i(x0+wid,y0);
	glVertex2i(x0+wid,y0+hei);
	glVertex2i(x0    ,y0+hei);
	glEnd();
}
void FsSimpleDialog::Widget::SetRect(int x0,int y0,int wid,int hei)
{
	this->x0=x0;
	this->y0=y0;
	this->wid=wid;
	this->hei=hei;
}
YSBOOL FsSimpleDialog::Widget::IsInside(int x,int y) const
{
	if(x0<=x && x<x0+wid && y0<=y && y<y0+hei)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

////////////////////////////////////////////////////////////

/* virtual */ void FsSimpleDialog::StaticText::Draw(const Widget *) const
{
	YsSystemFontTextBitmap bmp;
	const unsigned char fgCol[4]={255,255,255,255};
	const unsigned char bgCol[4]={0,0,0,0};
	if(YSOK==owner->fontCache.MakeRGBABitmap(bmp,str,fgCol,bgCol,YSTRUE))
	{
		const int y0=this->y0+this->hei-(this->hei-bmp.Height())/2;
		DrawBackground();
		glColor4ubv(fgCol);
		glRasterPos2i(this->x0+8,y0);
		glDrawPixels(bmp.Width(),bmp.Height(),GL_RGBA,GL_UNSIGNED_BYTE,bmp.Bitmap());
	}
}

////////////////////////////////////////////////////////////

/* virtual */ void FsSimpleDialog::TextInput::Draw(const Widget *activeWidget) const
{
	DrawBackground();
	DrawFrame();

	auto show=str;
	if(this==activeWidget)
	{
		DrawActiveFrame();
		if(0==time(nullptr)%2)
		{
			show.Append(L"_");
		}
		else
		{
			show.Append(L"|");
		}
	}

	YsSystemFontTextBitmap bmp;
	const unsigned char fgCol[4]={255,255,255,255};
	const unsigned char bgCol[4]={0,0,0,0};
	if(YSOK==owner->fontCache.MakeRGBABitmap(bmp,show,fgCol,bgCol,YSTRUE))
	{
		const int y0=this->y0+this->hei-(this->hei-bmp.Height())/2;
		glRasterPos2i(this->x0+8,y0);
		glDrawPixels(bmp.Width(),bmp.Height(),GL_RGBA,GL_UNSIGNED_BYTE,bmp.Bitmap());
	}
}
/* virtual */ void FsSimpleDialog::TextInput::KeyIn(int keyCode)
{
	if(0<str.Strlen() && FSKEY_BS==keyCode)
	{
		str.BackSpace();
	}
}
/* virtual */ void FsSimpleDialog::TextInput::CharIn(int charCode)
{
	if(32<=charCode)
	{
		str.Append(charCode);
	}
}

////////////////////////////////////////////////////////////

/* virtual */ void FsSimpleDialog::Button::Draw(const Widget *activeWidget) const
{
	DrawBackground();
	DrawFrame();
	if(this==activeWidget)
	{
		DrawActiveFrame();
	}

	YsSystemFontTextBitmap bmp;
	const unsigned char fgCol[4]={255,255,255,255};
	const unsigned char bgCol[4]={0,0,0,0};
	if(YSOK==owner->fontCache.MakeRGBABitmap(bmp,str,fgCol,bgCol,YSTRUE))
	{
		const int y0=this->y0+this->hei-(this->hei-bmp.Height())/2;
		glRasterPos2i(this->x0+8,y0);
		glDrawPixels(bmp.Width(),bmp.Height(),GL_RGBA,GL_UNSIGNED_BYTE,bmp.Bitmap());
	}
}

////////////////////////////////////////////////////////////

FsSimpleDialog::ListBox::ListBox()
{
	showTop=0;
}
void FsSimpleDialog::ListBox::CleanUp(void)
{
	item.CleanUp();
}
void FsSimpleDialog::ListBox::AddItem(const wchar_t str[],YSBOOL isDir)
{
	item.Increment();
	item.Last().str=str;
	item.Last().isDir=isDir;
	item.Last().selected=YSFALSE;
}

YSSIZE_T FsSimpleDialog::ListBox::GetNShow(void) const
{
	return (hei-8)/owner->fontHeight;
}

YSSIZE_T FsSimpleDialog::ListBox::PickedIndex(int,int my) const
{
	return (my-y0-4)/owner->fontHeight;
}
YsWString FsSimpleDialog::ListBox::GetSelectedString(void) const
{
	for(auto &i : item)
	{
		if(YSTRUE==i.selected)
		{
			return i.str;
		}
	}
	return L"";
}

YSBOOL FsSimpleDialog::ListBox::SelectionIsDirectory(void) const
{
	for(auto &i : item)
	{
		if(YSTRUE==i.selected)
		{
			return i.isDir;
		}
	}
	return YSFALSE;
}

/* virtual */ void FsSimpleDialog::ListBox::Draw(const Widget *activeWidget) const
{
	DrawBackground();
	DrawFrame();
	if(this==activeWidget)
	{
		DrawActiveFrame();
	}

	auto nShow=GetNShow();
	int y0=this->y0+4+owner->fontHeight;

	for(auto idx=showTop; idx<item.GetN() && idx<showTop+nShow; ++idx)
	{
		YsSystemFontTextBitmap bmp;
		unsigned char fgCol[4]={255,255,255,255};
		unsigned char bgCol[4]={0,0,0,0};

		auto str=item[idx].str;
		if(YSTRUE==item[idx].isDir)
		{
			str=L"<<";
			str.Append(item[idx].str);
			str.Append(L">>");
		}
		if(YSTRUE==item[idx].selected)
		{
			fgCol[0]=0;
			fgCol[1]=0;
			fgCol[2]=0;
			fgCol[3]=255;
			bgCol[0]=255;
			bgCol[1]=255;
			bgCol[2]=255;
			bgCol[3]=255;
		}
		if(YSOK==owner->fontCache.MakeRGBABitmap(bmp,str,fgCol,bgCol,YSTRUE))
		{
			if(YSTRUE==item[idx].selected)
			{
				glColor3ub(255,255,255);
				glBegin(GL_TRIANGLE_FAN);
				glVertex2i(this->x0+4            ,y0);
				glVertex2i(this->x0+4+bmp.Width(),y0);
				glVertex2i(this->x0+4+bmp.Width(),y0-bmp.Height());
				glVertex2i(this->x0+4            ,y0-bmp.Height());
				glEnd();
			}

			glRasterPos2i(this->x0+4,y0);
			glDrawPixels(bmp.Width(),bmp.Height(),GL_RGBA,GL_UNSIGNED_BYTE,bmp.Bitmap());
		}
		y0+=owner->fontHeight;
	}
}
/* virtual */ void FsSimpleDialog::ListBox::KeyIn(int keyCode)
{
	int diff=0;
	if(FSKEY_DOWN==keyCode)
	{
		diff=1;
	}
	else if(FSKEY_UP==keyCode)
	{
		diff=-1;
	}
	else if(FSKEY_WHEELDOWN==keyCode || FSKEY_PAGEDOWN==keyCode)
	{
		diff=8;
	}
	else if(FSKEY_WHEELUP==keyCode || FSKEY_PAGEUP==keyCode)
	{
		diff=-8;
	}
	if(0!=FsGetKeyState(FSKEY_SHIFT))
	{
		diff*=8;
	}
	showTop+=diff;

	if(FSKEY_HOME==keyCode)
	{
		showTop=0;
	}
	else if(FSKEY_END==keyCode)
	{
		showTop=item.GetN()-GetNShow()+1;
	}

	if(showTop<0)
	{
		showTop=0;
	}
	else if(item.GetN()<showTop-GetNShow()+1)
	{
		showTop=item.GetN()-GetNShow()+1;
	}
}
/* virtual */ void FsSimpleDialog::ListBox::CharIn(int charCode)
{
}

/* virtual */ void FsSimpleDialog::ListBox::LButtonDown(int mx,int my)
{
	auto idx=PickedIndex(mx,my);
	if(FsSubSecondTimer()-lastClickTime<1000 && lastSel==idx)
	{
		for(auto &i : item)  // For future expansion for multi-selection
		{
			i.selected=YSFALSE;
		}
		item[idx].selected=YSTRUE;
		owner->OnListBoxDoubleClick(this);
		lastClickTime=FsSubSecondTimer()-1000000;
	}
	else if(0<=idx && idx<item.GetN())
	{
		lastClickTime=FsSubSecondTimer();
		lastSel=idx;
		if(item[idx].selected!=YSTRUE)
		{
			for(auto &i : item)
			{
				i.selected=YSFALSE;
			}
			item[idx].selected=YSTRUE;
			owner->OnListBoxSelChange(this);
		}
	}
}
/* virtual */ void FsSimpleDialog::ListBox::LButtonUp(int mx,int my)
{
}

////////////////////////////////////////////////////////////

FsSimpleDialog::FsSimpleDialog()
{
	activeWidget=nullptr;

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	fontCache.RequestDefaultFontWithHeight(1+winHei/60);

	int fontWid,fontHei;
	fontCache.GetTightBitmapSize(fontWid,fontHei,L"X");

	this->fontHeight=fontHei;
}
FsSimpleDialog::~FsSimpleDialog()
{
	CleanUp();
}
void FsSimpleDialog::CleanUp(void)
{
	textInputSource.CleanUp();
	staticTextSource.CleanUp();
	widgetArray.CleanUp();
}

void FsSimpleDialog::SetActive(Widget *widgetPtr)
{
	activeWidget=widgetPtr;
}

FsSimpleDialog::TextInput *FsSimpleDialog::AddTextInput(void)
{
	textInputSource.Increment();
	widgetArray.Add(&textInputSource.Last());
	textInputSource.Last().owner=this;
	return &textInputSource.Last();
}
FsSimpleDialog::StaticText *FsSimpleDialog::AddStaticText(void)
{
	staticTextSource.Increment();
	widgetArray.Add(&staticTextSource.Last());
	staticTextSource.Last().owner=this;
	return &staticTextSource.Last();
}
FsSimpleDialog::Button *FsSimpleDialog::AddButton(void)
{
	buttonSource.Increment();
	widgetArray.Add(&buttonSource.Last());
	buttonSource.Last().owner=this;
	return &buttonSource.Last();
}
FsSimpleDialog::ListBox *FsSimpleDialog::AddListBox(void)
{
	listBoxSource.Increment();
	widgetArray.Add(&listBoxSource.Last());
	listBoxSource.Last().owner=this;
	return &listBoxSource.Last();
}

void FsSimpleDialog::Draw(void)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	glViewport(0,0,wid,hei);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT);
	for(auto widgetPtr : widgetArray)
	{
		widgetPtr->Draw(activeWidget);
	}
	FsSwapBuffers();
}
/* virtual */ void FsSimpleDialog::KeyIn(int keyCode)
{
	if(FSKEY_TAB==keyCode && 0<widgetArray.GetN())
	{
		if(nullptr==activeWidget)
		{
			activeWidget=widgetArray[0];
		}
		else
		{
			if(0==FsGetKeyState(FSKEY_SHIFT))
			{
				for(auto idx : widgetArray.AllIndex())
				{
					if(widgetArray[idx]==activeWidget)
					{
						activeWidget=widgetArray.GetCyclic(idx+1);
						break;
					}
				}
			}
			else
			{
				for(auto idx : widgetArray.AllIndex())
				{
					if(widgetArray[idx]==activeWidget)
					{
						activeWidget=widgetArray.GetCyclic(idx-1);
						break;
					}
				}
			}
		}
	}
	if(FSKEY_SPACE==keyCode || FSKEY_ENTER==keyCode)
	{
		auto btnPtr=dynamic_cast<Button *>(activeWidget);
		if(nullptr!=btnPtr)
		{
			OnButtonClick(btnPtr);
		}
	}

	if(nullptr!=activeWidget)
	{
		activeWidget->KeyIn(keyCode);
	}
}
/* virtual */ void FsSimpleDialog::CharIn(int charCode)
{
	if(nullptr!=activeWidget)
	{
		activeWidget->CharIn(charCode);
	}
}
/* virtual */ void FsSimpleDialog::LButtonDown(int mx,int my)
{
	for(auto widgetPtr : widgetArray)
	{
		if(YSTRUE==widgetPtr->IsInside(mx,my))
		{
			widgetPtr->LButtonDown(mx,my);
			activeWidget=widgetPtr;
		}
	}
}
/* virtual */ void FsSimpleDialog::MouseMove(int lb,int mb,int rb,int mx,int my)
{
	if(nullptr!=activeWidget)
	{
		for(auto widgetPtr : widgetArray)
		{
			if(YSTRUE==widgetPtr->IsInside(mx,my))
			{
				widgetPtr->MouseMove(lb,mb,rb,mx,my);
			}
		}
	}
}
/* virtual */ void FsSimpleDialog::LButtonUp(int mx,int my)
{
	if(nullptr!=activeWidget)
	{
		for(auto widgetPtr : widgetArray)
		{
			if(YSTRUE==widgetPtr->IsInside(mx,my))
			{
				widgetPtr->LButtonUp(mx,my);

				auto btnPtr=dynamic_cast<Button *>(widgetPtr);
				if(nullptr!=btnPtr)
				{
					OnButtonClick(btnPtr);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////

class FsSimpleFileDialog_Linux : public FsSimpleDialog
{
public:
	YsWString currentDirectory;
	YsWString currentFile;

	StaticText *titleStatic;
	TextInput *directoryTxt;
	TextInput *fileTxt;
	ListBox *lbx;
	Button *okBtn,*cancelBtn;

	YSBOOL close;
	YSRESULT res;

	void Make(FsSimpleFileDialog *owner);
	void PopulateFileList(void);
	virtual void KeyIn(int keyCode);
	virtual void CharIn(int charCoe);
	virtual void LButtonDown(int mx,int my);
	virtual void MouseMove(int lb,int mb,int rb,int mx,int my);
	virtual void LButtonUp(int mx,int my);
	virtual void OnButtonClick(Button *btn);
	virtual void OnListBoxSelChange(ListBox *lbx);
	virtual void OnListBoxDoubleClick(ListBox *lbx);
};

void FsSimpleFileDialog_Linux::Make(FsSimpleFileDialog *owner)
{
	close=YSFALSE;
	res=YSOK;

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	int y0=0,h=winHei/20;
	titleStatic=AddStaticText();
	titleStatic->str=owner->title;
	titleStatic->SetRect(0,y0,winWid,h);
	y0+=h;

	directoryTxt=AddTextInput();
	directoryTxt->SetRect(0,y0,winWid,h);
	y0+=h;

	lbx=AddListBox();
	lbx->SetRect(0,y0,winWid,winHei*7/10);
	y0+=lbx->hei;

	fileTxt=AddTextInput();
	fileTxt->SetRect(0,y0,winWid,h);
	y0+=h;

	okBtn=AddButton();
	okBtn->str=L"OK";
	okBtn->SetRect(0,y0,winWid/2,h);

	cancelBtn=AddButton();
	cancelBtn->str=L"Cancel";
	cancelBtn->SetRect(winWid/2,y0,winWid,h);

	SetActive(fileTxt);



	YsWString pth,fil;
	owner->defaultFileName.SeparatePathFile(pth,fil);
	if(0<pth.Strlen())
	{
		currentDirectory=pth;
	}
	else
	{
		currentDirectory=YsFileIO::Getcwd();
	}
	directoryTxt->str=currentDirectory;
	fileTxt->str=fil;

	PopulateFileList();
}

void FsSimpleFileDialog_Linux::PopulateFileList(void)
{
	lbx->CleanUp();
	YsFileList flist;
	flist.FindFileList(currentDirectory,L"",L"");
	for(YSSIZE_T idx=0; idx<flist.GetN(); ++idx)
	{
		lbx->AddItem(flist.GetBaseNameW(idx),flist.IsDirectory(idx));
	}
}

/* virtual */ void FsSimpleFileDialog_Linux::KeyIn(int keyCode)
{
	FsSimpleDialog::KeyIn(keyCode);
}
/* virtual */ void FsSimpleFileDialog_Linux::CharIn(int charCode)
{
	FsSimpleDialog::CharIn(charCode);
}
/* virtual */ void FsSimpleFileDialog_Linux::LButtonDown(int mx,int my)
{
	FsSimpleDialog::LButtonDown(mx,my);
}
/* virtual */ void FsSimpleFileDialog_Linux::MouseMove(int lb,int mb,int rb,int mx,int my)
{
	FsSimpleDialog::MouseMove(lb,mb,rb,mx,my);
}
/* virtual */ void FsSimpleFileDialog_Linux::LButtonUp(int mx,int my)
{
	FsSimpleDialog::LButtonUp(mx,my);
}
/* virtual */ void FsSimpleFileDialog_Linux::OnButtonClick(Button *btn)
{
	if(btn==okBtn)
	{
		close=YSTRUE;
		res=YSOK;
	}
	else if(btn==cancelBtn)
	{
		close=YSTRUE;
		res=YSERR;
	}
}
/* virtual */ void FsSimpleFileDialog_Linux::OnListBoxSelChange(ListBox *lbx)
{
	if(lbx==this->lbx && YSTRUE!=lbx->SelectionIsDirectory())
	{
		fileTxt->str=lbx->GetSelectedString();
	}
}
/* virtual */ void FsSimpleFileDialog_Linux::OnListBoxDoubleClick(ListBox *lbx)
{
	if(lbx==this->lbx)
	{
		auto selStr=lbx->GetSelectedString();
		if(YSTRUE==lbx->SelectionIsDirectory())
		{
			if(0==selStr.Strcmp(L"."))
			{
			}
			else if(0==selStr.Strcmp(L".."))
			{
				if(':'==currentDirectory.LastChar() || currentDirectory.DoesEndWith(L"//") || currentDirectory.DoesEndWith(L"\\\\"))
				{
					return;
				}

				YsWString pth,fil;
				if('/'==currentDirectory.LastChar() || '\\'==currentDirectory.LastChar())
				{
					currentDirectory.BackSpace();
				}
				currentDirectory.SeparatePathFile(pth,fil);
				currentDirectory=pth;
				directoryTxt->str=currentDirectory;
				PopulateFileList();
			}
			else
			{
				currentDirectory.MakeFullPathName(currentDirectory,selStr);
				directoryTxt->str=currentDirectory;
				PopulateFileList();
			}
		}
		else
		{
			OnButtonClick(okBtn);
		}
	}
}

////////////////////////////////////////////////////////////


void FsSimpleFileDialog::Start(void)
{
	if(YSTRUE==busy)
	{
		return;
	}
	busy=YSTRUE;


	FsSimpleFileDialog_Linux fdlg;
	YSRESULT res=YSERR;

	fdlg.Make(this);

	YSBOOL close=YSFALSE;
	while(YSTRUE!=close)
	{
		FsPollDevice();

		for(;;)
		{
			auto key=FsInkey();
			if(FSKEY_ESC==key)
			{
				close=YSTRUE;
				res=YSERR;
				break;
			}
			fdlg.KeyIn(key);
			if(FSKEY_NULL==key)
			{
				break;
			}
		}
		for(;;)
		{
			auto c=FsInkeyChar();
			if(0==c)
			{
				break;
			}
			fdlg.CharIn(c);
		}
		int lb,mb,rb,mx,my;
		auto evt=FsGetMouseEvent(lb,mb,rb,mx,my);
		if(evt==FSMOUSEEVENT_LBUTTONDOWN)
		{
			fdlg.LButtonDown(mx,my);
		}
		else if(evt==FSMOUSEEVENT_MOVE)
		{
			fdlg.MouseMove(lb,mb,rb,mx,my);
		}
		else if(evt==FSMOUSEEVENT_LBUTTONUP)
		{
			fdlg.LButtonUp(mx,my);
		}
		if(YSTRUE==fdlg.close)
		{
			res=fdlg.res;
			break;
		}


		fdlg.Draw();
	}

	selectedFileArray.CleanUp();
	if(YSOK==res)
	{
		YsWString ful;
		ful.MakeFullPathName(fdlg.directoryTxt->str,fdlg.fileTxt->str);
		selectedFileArray.Add(ful);
	}

	busy=YSFALSE;

	OnClose((int)res);
	if(nullptr!=closeModalCallBack)
	{
		closeModalCallBack(this,(int)res);
	}
}

void FsSimpleFileDialog::AllocatePlatformSpecificData(void)
{
}
void FsSimpleFileDialog::DeletePlatformSpecificData(void)
{
}
