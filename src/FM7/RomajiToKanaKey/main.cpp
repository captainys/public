#include <fssimplewindow.h>
#include <ysgl.h>
#include <ysglsldrawfontbitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

#include "fm77avkeymap.h"
#include "fm77avrkana.h"
#include "gui.h"



class MainDialog
{
private:
	CheapGUI gui;
	FM77AVReverseKeyMap revKeyMap;

	std::string typingStr;
	std::vector <std::string> typingWordStr;

	CheapGUI::Text *titleTxt;
	CheapGUI::Text *typingText;
	CheapGUI::Text *typingWordText;

	enum {
		NUM_WHAT_TO_TYPE=32
	};
	CheapGUI::Text *whatToType[NUM_WHAT_TO_TYPE];

	CheapGUI::CheckBox *us101Btn;
	CheapGUI::CheckBox *jisBtn;
	std::vector <CheapGUI::CheckBox *> modeBtn;

	CheapGUI::PushButton *clearBtn,*exitBtn;

public:
	bool quit;

public:
	void Initialize(void);
	void SetUpCheapGUI(void);
	void ProcessUserInput(void);
	void RunOneStep(void);
	void UpdateGUI(void);
	void Draw(void) const;
	void ResetGUI(void);

	void Clear(void);
	void Quit(void);

	bool NeedRedraw(void) const;

	void UpdateText(void);
};

void MainDialog::Initialize(void)
{
	quit=false;

	typingStr.clear();
	typingWordStr.clear();
	revKeyMap.MakeJISKeyMap();
}

void MainDialog::SetUpCheapGUI(void)
{
	int guiY=8;

	titleTxt=gui.AddText(8,guiY,400,32,"FM-7 Romaji to Kana Key Converter");
	guiY+=32;

	gui.AddText(8,guiY,400,32,"CaptainYS (http://www.ysflight.com)");
	guiY+=32;

	typingText=gui.AddText(8,guiY,400,32,"");
	guiY+=32;

	typingWordText=gui.AddText(8,guiY,400,32,"");
	guiY+=34;

	{
		auto guiY0=guiY;
		auto guiX=8;
		for(int i=0; i<NUM_WHAT_TO_TYPE; ++i)
		{
			if(i==8 || i==16 || 24==i)
			{
				guiX+=160;
				guiY=guiY0;
			}
			whatToType[i]=gui.AddText(guiX,guiY,640,32,"");
			guiY+=32;
		}
	}


	us101Btn=gui.AddCheckBox(8,guiY,248,32,"US Keyboard");
	jisBtn=gui.AddCheckBox(320,guiY,248,32,"JA Keyboard");
	jisBtn->SetCheck(true);
	guiY+=34;

	modeBtn.push_back(us101Btn);
	modeBtn.push_back(jisBtn);
	gui.SetRadioButtonGroup(modeBtn);

	clearBtn=gui.AddPushButton(8,guiY,248,32,"CLEAR(ENTER)");

	exitBtn=gui.AddPushButton(320,guiY,248,32,"EXIT(ESC)");
	guiY+=34;

	gui.needRedraw=true;
}

void MainDialog::ProcessUserInput(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key)
	{
		Quit();
	}
	if(FSKEY_ENTER==key)
	{
		Clear();
	}
	if(FSKEY_BS==key)
	{
		if(0==typingStr.size() && 0<typingWordStr.size())
		{
			typingStr=typingWordStr.back();
			typingStr.pop_back();
			typingWordStr.pop_back();
		}
		else if(0<typingStr.size())
		{
			typingStr.pop_back();
		}
		typingText->SetText(typingStr.c_str());
		UpdateText();
	}
	int lb,mb,rb,mx,my;
	auto mouseEvt=FsGetMouseEvent(lb,mb,rb,mx,my);
	if(FSMOUSEEVENT_LBUTTONUP==mouseEvt)
	{
		gui.NotifyLButtonUp(mx,my);
	}
	if(exitBtn==gui.PeekLastClicked())
	{
		Quit();
	}
	else if(jisBtn==gui.PeekLastClicked())
	{
		revKeyMap.MakeJISKeyMap();
		UpdateText();
	}
	else if(us101Btn==gui.PeekLastClicked())
	{
		revKeyMap.MakeUS101KeyMap();
		UpdateText();
	}
	else if(clearBtn==gui.PeekLastClicked())
	{
		Clear();
	}
	gui.GetLastClicked();

	auto c=FsInkeyChar();
	bool shift=FsGetKeyState(FSKEY_SHIFT)!=0;
	bool ctrl=FsGetKeyState(FSKEY_CTRL)!=0;

	if(0x20<c && c<127)
	{
		typingStr.push_back(c);
		UpdateText();
	}
}

void MainDialog::RunOneStep(void)
{
}

void MainDialog::UpdateGUI(void)
{
}

void MainDialog::Draw(void) const
{
	gui.Draw();
}

void MainDialog::Quit(void)
{
	this->quit=true;
}

void MainDialog::Clear(void)
{
	typingWordStr.clear();
	typingWordText->SetText("");
	typingStr="";
	typingText->SetText("");
	for(int i=0; i<NUM_WHAT_TO_TYPE; ++i)
	{
		whatToType[i]->SetText("");
	}
}

bool MainDialog::NeedRedraw(void) const
{
	return gui.needRedraw;
}

void MainDialog::UpdateText(void)
{
	auto avkey=FM77AVFindKeyCombinationForRomaji(typingStr.c_str());
	auto carryOver=false;

	if(nullptr!=avkey.inputStr && 0!=avkey.inputStr[0])
	{
		auto lastChar=typingStr.back();
		typingWordStr.push_back(typingStr);
		typingStr.clear();

		if(true==avkey.carryOver)
		{
			typingStr.push_back(lastChar);
		}
	}

	std::string allStr;
	int n=0;
	for(auto &s : typingWordStr)
	{
		allStr+=s;
		auto avkey=FM77AVFindKeyCombinationForRomaji(s.c_str());
		if(true==avkey.carryOver)
		{
			allStr.pop_back();
		}
		allStr.push_back(' ');

		for(auto c : avkey.comb)
		{
			auto keyLabel=revKeyMap.GetKeyLabel(c.keyCode);
			if(nullptr!=keyLabel && 0!=keyLabel[0] && n<NUM_WHAT_TO_TYPE)
			{
				std::string msg;
				if(true==c.shift)
				{
					msg+="Shift+";
				}
				msg+=keyLabel;
				whatToType[n]->SetText(msg.c_str());
				++n;
			}
		}
	}
	for(int i=n; i<NUM_WHAT_TO_TYPE; ++i)
	{
		whatToType[i]->SetText("");
	}

	typingWordText->SetText(allStr.c_str());
	typingText->SetText(typingStr.c_str());
}



////////////////////////////////////////////////////////////

int main(void)
{
	FsOpenWindow(0,0,640,480,1);
	FsChangeToProgramDir();

	YsGLSLCreateSharedBitmapFontRenderer();
	YsGLSLCreateSharedRenderer();
	{
		auto renderer=YsGLSLSharedBitmapFontRenderer();
		YsGLSLUseBitmapFontRenderer(renderer);
		YsGLSLBitmapFontRendererRequestFontSize(renderer,24,20);
		YsGLSLEndUseBitmapFontRenderer(renderer);
	}

	MainDialog app;
	app.Initialize();
	app.SetUpCheapGUI();

	while(true!=app.quit)
	{
		FsPollDevice();
		app.ProcessUserInput();
		app.RunOneStep();
		app.UpdateGUI();

		if(true==app.NeedRedraw() || 0!=FsCheckWindowExposure())
		{
			int wid,hei;
			FsGetWindowSize(wid,hei);

			{
				auto renderer=YsGLSLSharedBitmapFontRenderer();
				YsGLSLUseBitmapFontRenderer(renderer);
				YsGLSLSetBitmapFontRendererViewportSize(renderer,wid,hei);
				YsGLSLEndUseBitmapFontRenderer(renderer);
			}

			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glViewport(0,0,wid,hei);
			app.Draw();
			FsSwapBuffers();
		}

		FsSleep(1); // Give some CPU time for COM driver.
	}

	YsGLSLDeleteSharedBitmapFontRenderer();

	return 0;
}

