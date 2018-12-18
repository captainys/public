#include <fsgui.h>
#include <ysport.h>

#include "helpdlg.h"
#include "../textresource.h"
#include <fssimplewindow.h>
#include "../ysgebl_gui_editor_base.h"
#include "../ysgeblversion.h"


#define OPENVECTORBTN YsOpenURL("http://s.shop.vector.co.jp/service/servlet/NCart.Add?ITEM_NO=SR360800")
#define OPENPAYPALBTN YsOpenURL("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=WNHT4QQ753B86&lc=US&item_name=YSFLIGHT%2eCOM%20%2f%20Development%20of%20Free%20Flight%20Simulator%2c%20Tools%2c%20Polygon%20Editor%2c%20etc%2e&item_number=1&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted")


PolyCreAboutDialog::PolyCreAboutDialog()
{
	canvas=NULL;
	okBtn=NULL;
}

PolyCreAboutDialog::~PolyCreAboutDialog()
{
}

YSRESULT PolyCreAboutDialog::Make(class GeblGuiEditorBase *canvasPtr)
{
	if(NULL==okBtn)
	{
		FsGuiDialog::Initialize();

		this->canvas=canvasPtr;

		AddStaticText(0,FSKEY_NULL,L"Polygon Crest",YSTRUE);
		AddStaticText(0,FSKEY_NULL,L"Copyright (C) Soji Yamakawa",YSTRUE);

		YsString str;
		str.Printf("Version %d",PolygonCrestVersion);

		AddStaticText(0,FSKEY_NULL,str,YSTRUE);

		ysflightBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"http://www.ysflight.com",YSTRUE);

		AddStaticText(0,FSKEY_NULL,FSGUI_DLG_ABOUT_DONATELINE1,YSTRUE);
		AddStaticText(0,FSKEY_NULL,FSGUI_DLG_ABOUT_DONATELINE2,YSTRUE);

		YsLocale locale;
		const char *langStr=locale.GetLanguagePart();
		if(NULL!=langStr && 0==strncmp("ja",langStr,2))
		{
			// Japanese
			donateVectorBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_ABOUT_DONATEVECTOR,YSTRUE);
			AddStaticText(0,FSKEY_NULL,FSGUI_DLG_ABOUT_DONATELINE3,YSTRUE);
			donatePayPalBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_ABOUT_DONATEPAYPAL,YSTRUE);
		}
		else
		{
			// Non-Japanese
			donateVectorBtn=NULL;
			donatePayPalBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_ABOUT_DONATEPAYPAL,YSTRUE);
		}


		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSTRUE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(1.0);
		Fit();
	}

	return YSOK;
}

void PolyCreAboutDialog::RemakeDrawingBuffer(void)
{
	// As required
}

void PolyCreAboutDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		CloseModalDialog(YSOK);
	}
	else if(btn==ysflightBtn)
	{
		YsOpenURL("http://www.ysflight.com");
	}
	else if(btn==donatePayPalBtn)
	{
		OPENPAYPALBTN;
	}
	else if(btn==donateVectorBtn)
	{
		OPENVECTORBTN;
	}
}

////////////////////////////////////////////////////////////



void GeblGuiEditorBase::Help_About(FsGuiPopUpMenuItem *)
{
	auto aboutDlg=FsGuiDialog::CreateSelfDestructiveDialog<PolyCreAboutDialog>();

	aboutDlg->Make(this);
	AttachModalDialog(aboutDlg);
	ArrangeDialog();
}


////////////////////////////////////////////////////////////

PolyCreHelpDialog::PolyCreHelpDialog()
{
	okBtn=NULL;
	urlBtn=NULL;
}

PolyCreHelpDialog::~PolyCreHelpDialog()
{
}

YSRESULT PolyCreHelpDialog::Make(class GeblGuiEditorBase *canvasPtr)
{
	if(NULL==okBtn)
	{
		this->canvas=canvasPtr;

		AddStaticText(0,FSKEY_NULL,FSGUI_DLG_HELP_HELPMSG,YSTRUE);
		urlBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_HELP_HELPURL,YSTRUE);

		AddStaticText(0,FSKEY_NULL,L"--------------------------------",YSTRUE);

		ysflightBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"http://www.ysflight.com",YSTRUE);

		AddStaticText(0,FSKEY_NULL,FSGUI_DLG_ABOUT_DONATELINE1,YSTRUE);
		AddStaticText(0,FSKEY_NULL,FSGUI_DLG_ABOUT_DONATELINE2,YSTRUE);

		YsLocale locale;
		const char *langStr=locale.GetLanguagePart();
		if(NULL!=langStr && 0==strncmp("ja",langStr,2))
		{
			// Japanese
			donateVectorBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_ABOUT_DONATEVECTOR,YSTRUE);
			AddStaticText(0,FSKEY_NULL,FSGUI_DLG_ABOUT_DONATELINE3,YSTRUE);
			donatePayPalBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_ABOUT_DONATEPAYPAL,YSTRUE);
		}
		else
		{
			// Non-Japanese
			donateVectorBtn=NULL;
			donatePayPalBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_ABOUT_DONATEPAYPAL,YSTRUE);
		}

		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_CLOSE,YSTRUE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(1.0);
		Fit();
	}
	return YSOK;
}

/* virtual */ void PolyCreHelpDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		CloseModalDialog(YSOK);
	}
	else if(btn==urlBtn)
	{
		YsWString wstr=FSGUI_DLG_HELP_HELPURL;
		YsString str;
		str.EncodeUTF8 <wchar_t> (wstr);
		YsOpenURL(str);
	}
	else if(btn==ysflightBtn)
	{
		YsOpenURL("http://www.ysflight.com");
	}
	else if(btn==donatePayPalBtn)
	{
		OPENPAYPALBTN;
	}
	else if(btn==donateVectorBtn)
	{
		OPENVECTORBTN;
	}
}

void GeblGuiEditorBase::Help_Help(FsGuiPopUpMenuItem *)
{
	auto helpDlg=FsGuiDialog::CreateSelfDestructiveDialog<PolyCreHelpDialog>();
	helpDlg->Make(this);
	AttachModalDialog(helpDlg);
	ArrangeDialog();
}
