#include <string.h>

#include "gui.h"



CheapGUI::CheapGUI()
{
	needRedraw=false;
	lastClick=nullptr;
}
CheapGUI::Text *CheapGUI::AddText(int x0,int y0,int wid,int hei,const char label[])
{
	std::shared_ptr <Widget> widgetPtr(new Text);
	widgetList.push_back(widgetPtr);
	auto btnPtr=dynamic_cast <Text *>(widgetPtr.get());

	btnPtr->label=label;
	btnPtr->x0=x0;
	btnPtr->y0=y0;
	btnPtr->wid=wid;
	btnPtr->hei=hei;
	btnPtr->guiPtr=this;

	return btnPtr;
}
CheapGUI::PushButton *CheapGUI::AddPushButton(int x0,int y0,int wid,int hei,const char label[])
{
	std::shared_ptr <Widget> widgetPtr(new PushButton);
	widgetList.push_back(widgetPtr);
	auto btnPtr=dynamic_cast <PushButton *>(widgetPtr.get());

	btnPtr->label=label;
	btnPtr->x0=x0;
	btnPtr->y0=y0;
	btnPtr->wid=wid;
	btnPtr->hei=hei;
	btnPtr->guiPtr=this;

	return btnPtr;
}
CheapGUI::CheckBox *CheapGUI::AddCheckBox(int x0,int y0,int wid,int hei,const char label[])
{
	std::shared_ptr <Widget> widgetPtr(new CheckBox);
	widgetList.push_back(widgetPtr);
	auto btnPtr=dynamic_cast <CheckBox *>(widgetPtr.get());

	btnPtr->label=label;
	btnPtr->x0=x0;
	btnPtr->y0=y0;
	btnPtr->wid=wid;
	btnPtr->hei=hei;
	btnPtr->guiPtr=this;

	return btnPtr;
}
void CheapGUI::SetRadioButtonGroup(const std::vector <CheckBox *> &grp)
{
	for(auto btnPtr : grp)
	{
		btnPtr->SetRadioButtonGroup(grp);
	}
}
void CheapGUI::Draw(void) const
{
	for(auto w : widgetList)
	{
		if(true!=w->hide)
		{
			w->Draw();
		}
	}
	needRedraw=false;
}

void CheapGUI::NotifyMousePosition(int mx,int my)
{
}

void CheapGUI::NotifyLButtonUp(int mx,int my)
{
	for(auto w : widgetList)
	{
		if(true!=w->hide &&
		   w->x0<=mx && mx<=w->x0+w->wid && w->y0<=my && my<=w->y0+w->hei)
		{
			w->Clicked();
			lastClick=w.get();
		}
	}
}
CheapGUI::Widget *CheapGUI::GetLastClicked(void)
{
	auto lc=lastClick;
	lastClick=nullptr;
	return lc;
}
const CheapGUI::Widget *CheapGUI::PeekLastClicked(void) const
{
	return lastClick;
}

CheapGUI::Widget *CheapGUI::PeekLastClicked(void)
{
	return lastClick;
}


////////////////////////////////////////////////////////////

CheapGUI::Widget::Widget()
{
	r=0;
	g=0;
	b=255;
	a=255;
	hide=false;
}

void CheapGUI::Widget::Show(void)
{
	if(true==hide)
	{
		hide=false;
		guiPtr->needRedraw=true;
	}
}
void CheapGUI::Widget::Hide(void)
{
	if(true!=hide)
	{
		hide=true;
		guiPtr->needRedraw=true;
	}
}

void CheapGUI::Widget::SetColor(int r,int g,int b,int a)
{
	this->r=r;
	this->g=g;
	this->b=b;
	this->a=a;
}

////////////////////////////////////////////////////////////

void CheapGUI::Text::SetText(const char str[])
{
	label=str;
	guiPtr->needRedraw=true;
}
////////////////////////////////////////////////////////////

CheapGUI::Button::Button()
{
	heldDown=false;
}

////////////////////////////////////////////////////////////

CheapGUI::CheckBox::CheckBox()
{
	checked=false;
}

void CheapGUI::CheckBox::SetCheck(bool c)
{
	if(c!=checked)
	{
		checked=c;
		guiPtr->needRedraw=true;
	}
}

bool CheapGUI::CheckBox::GetCheck(void) const
{
	return checked;
}

void CheapGUI::CheckBox::SetRadioButtonGroup(const std::vector <CheckBox *> &grp)
{
	this->radioBtnGrp=grp;
}

/* virtual */ void CheapGUI::CheckBox::Clicked(void)
{
	if(0==radioBtnGrp.size())
	{
		if(true==checked)
		{
			checked=false;
		}
		else
		{
			checked=true;
		}
		guiPtr->needRedraw=true;
	}
	else
	{
		if(true!=checked)
		{
			for(auto btnPtr : radioBtnGrp)
			{
				btnPtr->SetCheck(false);
			}
			checked=true;
			guiPtr->needRedraw=true;
		}
	}
}
