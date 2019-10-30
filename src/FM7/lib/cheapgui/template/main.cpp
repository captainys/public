#include <string>
#include <vector>


#include <stdio.h>


#include "fssimplewindow.h"
#include "gui.h"



class ApplicationMain
{
private:
	CheapGUI gui;
	bool terminateFlag;
	bool ignoreNextLButtonUp;

	// GUI >>
	CheapGUI::Text *titleTxt;
	CheapGUI::Text *statusText;
	CheapGUI::PushButton *exitBtn;
	// GUI <<
public:
	ApplicationMain();

	bool Terminate(void) const;
	bool RecognizeCommandParameter(int ac,char *av[]);
	void Make(void);
	void ProcessUserInput(void);
	void RunOneStep(void);
	void UpdateGUI(void);
	void Draw(void) const;
};

ApplicationMain::ApplicationMain()
{
	terminateFlag=false;
	ignoreNextLButtonUp=false;
}

bool ApplicationMain::Terminate(void) const
{
	return terminateFlag;
}

bool ApplicationMain::RecognizeCommandParameter(int ac,char *av[])
{
	return true;
}

void ApplicationMain::Make(void)
{
	int guiY=8;

	titleTxt=gui.AddText(8,guiY,400,32,"CheapGUITemplate by CaptainYS (http://www.ysflight.com)");
	guiY+=32;

	statusText=gui.AddText(8,guiY,400,32,"STATUS");
	guiY+=32;

	exitBtn=gui.AddPushButton(8,guiY,300,32,"EXIT(ESC)");
	guiY+=32;
}

void ApplicationMain::ProcessUserInput(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		terminateFlag=true;
	}

	int lb,mb,rb,mx,my;
	auto mouseEvt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(FSMOUSEEVENT_LBUTTONUP==mouseEvt)
	{
		if(true==ignoreNextLButtonUp)
		{
			ignoreNextLButtonUp=false;
		}
		else
		{
			gui.NotifyLButtonUp(mx,my);
		}
	}
	if(FSMOUSEEVENT_LBUTTONDOWN==mouseEvt)
	{
		ignoreNextLButtonUp=false;
	}

	if(exitBtn==gui.PeekLastClicked())
	{
		terminateFlag=true;
	}
	gui.GetLastClicked();
}

void ApplicationMain::RunOneStep(void)
{
	FsPushOnPaintEvent();
}

void ApplicationMain::UpdateGUI(void)
{
}

void ApplicationMain::Draw(void) const
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	gui.Draw();
	FsSwapBuffers();
}

////////////////////////////////////////////////////////////

void Render(void *dat)
{
	ApplicationMain *mainPtr=(ApplicationMain*)(dat);
	mainPtr->Draw();
}

int main(int ac,char *av[])
{
	ApplicationMain main;

	FsOpenWindow(0,0,1024,800,1);
	FsRegisterOnPaintCallBack(Render,&main);

	FsChangeToProgramDir();

	CheapGUI::CreateBitmapFontRenderer();

	if(true!=main.RecognizeCommandParameter(ac,av))
	{
		return 1;
	}
	main.Make();

	while(true!=main.Terminate())
	{
		FsPollDevice();
		main.ProcessUserInput();
		main.RunOneStep();
		main.UpdateGUI();
	}

	return 0;
}
