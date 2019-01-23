#include <fssimplewindow.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

#include "fm77avkeyboardemu.h"
#include "filedialog.h"
#include "comport.h"
#include "cheaputil.h"
#include "gui.h"
#include "clipboard.h"



class FM77AVKeyboardEmulatorMain
{
private:
	FM77AVKeyboardEmulator fm77avKeyboardEmu;
	CheapGUI gui;
	std::vector <int> availablePort;
	int port;
	bool autoPortScan;

	int prevPort;
	int prevMode;
	int prevIRToyState;
	int prevIRToyError;
	bool prevAutoTyping;
	std::string prevRKana;

	CheapGUI::Text *titleTxt;
	CheapGUI::Text *statusText;
	CheapGUI::Text *errorText;

	std::vector <CheapGUI::CheckBox *> portBtn;

	CheapGUI::CheckBox *translateModeBtn;
	CheapGUI::CheckBox *directModeBtn;
	CheapGUI::CheckBox *rKanaModeBtn;
	std::vector <CheapGUI::CheckBox *> modeBtn;

	CheapGUI::PushButton *autoTypingBtn,*autoTypingBasicBtn,*autoTypeClipboardBtn;

	CheapGUI::PushButton *exitBtn;
	CheapGUI::PushButton *secretBtn;
	CheapGUI::PushButton *capsBtn;
	CheapGUI::PushButton *kanaBtn;
	CheapGUI::CheckBox *autoStopBtn;
	CheapGUI::CheckBox *autoFireBtn;

	CheapGUI::Text *rKanaTxt;
	CheapGUI::CheckBox *pauseBtn;

	CheapGUI::Text *irToyVersionTxt;
	CheapGUI::Text *protocolVersionTxt;

	CheapGUI::Text *zxcTxt;
	CheapGUI::CheckBox *ZXCforLMRSpaceBtn;
	CheapGUI::Text *arrowTxt;
	CheapGUI::CheckBox *arrowForArrowBtn,*arrowFor4DirBtn,*arrowFor8DirBtn;
	std::vector <CheapGUI::Widget *> assignmentWidget;

	CheapGUI::PushButton *saveCOMPortLogBtn;

public:
	bool quit;
	bool ignoreNextLButtonUp;

public:
	void Initialize(void);
	void SetUpCheapGUI(void);
	void ProcessUserInput(void);
	void RunOneStep(void);
	void UpdateGUI(void);
	void Draw(void) const;
	void ResetGUI(void);

	void Quit(void);

	bool NeedRedraw(void) const;

private:
	void UpdateIRToyState(CheapGUI::Text *textPtr,int irToyState,int prevIRToyState);
	void UpdateIRToyErrorCode(CheapGUI::Text *textPtr,int irToyError,int prevIRToyError,bool autoTyping,bool prevAutoTyping);
};

void FM77AVKeyboardEmulatorMain::Initialize(void)
{
	quit=false;
	ignoreNextLButtonUp=false;
	autoPortScan=true;

	availablePort=YsCOMPort::FindAvailablePort();

	port=(0<availablePort.size() ? availablePort.back() : -1);
	if(0<availablePort.size())
	{
		fm77avKeyboardEmu.Connect(port);
	}
	else
	{
		fm77avKeyboardEmu.SetIRToyNotFoundError();
	}

	prevPort=-1;
	prevMode=FM77AVKeyboardEmulator::MODE_NULL;
	prevIRToyState=IRToy_Controller::STATE_NULL;
	prevIRToyError=IRToy_Controller::ERROR_NULL;
	prevAutoTyping=false;
	prevRKana.clear();
}

void FM77AVKeyboardEmulatorMain::SetUpCheapGUI(void)
{
	int guiY=8;

	titleTxt=gui.AddText(8,guiY,400,32,"FM77AV Keyboard Emulator");
	guiY+=32;

	gui.AddText(8,guiY,400,32,"CaptainYS (http://www.ysflight.com)");
	guiY+=32;

	statusText=gui.AddText(8,guiY,400,32,"STATUS");
	guiY+=32;

	errorText=gui.AddText(8,guiY,400,32,"CONDITION:OK");
	errorText->SetColor(0,128,0,255);
	guiY+=34;

	if(1<availablePort.size())
	{
		gui.AddText(8,guiY,64,32,"PORT:");
		int guiX=96;
		for(auto p : availablePort)
		{
			char msg[256];
			sprintf(msg,"%d",p);
			portBtn.push_back(gui.AddCheckBox(guiX,guiY,32,32,msg));
			if(p==this->port)
			{
				portBtn.back()->SetCheck(true);
			}
			guiX+=40;
		}
		gui.SetRadioButtonGroup(portBtn);
		guiY+=34;
	}

	translateModeBtn=gui.AddCheckBox(8,guiY,240,32,"TRANSLATION");
	translateModeBtn->SetCheck(true);
	directModeBtn=gui.AddCheckBox(258,guiY,160,32,"DIRECT");
	rKanaModeBtn=gui.AddCheckBox(428,guiY,140,32,"RKANA");
	guiY+=34;

	modeBtn.push_back(translateModeBtn);
	modeBtn.push_back(directModeBtn);
	modeBtn.push_back(rKanaModeBtn);
	gui.SetRadioButtonGroup(modeBtn);

	exitBtn=gui.AddPushButton(8,guiY,300,32,"EXIT(Shift+ESC)");
	secretBtn=gui.AddPushButton(328,guiY,300,32,"77AV SECRET MSG");
	guiY+=34;

	capsBtn=gui.AddPushButton(8,guiY,128,32,"CAPS");
	kanaBtn=gui.AddPushButton(144,guiY,128,32,"KANA");
	rKanaTxt=gui.AddText(580,guiY,72,32,"");
	guiY+=34;

	pauseBtn=gui.AddCheckBox(8,guiY,112,32,"PAUSE");
	pauseBtn->SetCheck(fm77avKeyboardEmu.GetPause());
	autoStopBtn=gui.AddCheckBox(128,guiY,150,32,"AUTO-STOP");
	autoFireBtn=gui.AddCheckBox(286,guiY,150,32,"AUTO-FIRE");
	guiY+=34;

	autoTypingBtn=gui.AddPushButton(8,guiY,150,32,"AUTO-TYPE");
	autoTypingBasicBtn=gui.AddPushButton(176,guiY,260,32,"AUTO-TYPE(BASIC)");
#ifdef _WIN32
	autoTypeClipboardBtn=gui.AddPushButton(440,guiY,112,32,"PASTE");
#endif
	guiY+=34;

	zxcTxt=gui.AddText(8,guiY,112,32,"ZXC:");
	ZXCforLMRSpaceBtn=gui.AddCheckBox(112,guiY,200,32,"L-M-R SPACE");
	guiY+=34;
	arrowTxt=gui.AddText(8,guiY,112,32,"ARROW:");
	arrowForArrowBtn=gui.AddCheckBox(128,guiY,112,32,"ARROW");
	arrowForArrowBtn->SetCheck(true);
	arrowFor4DirBtn=gui.AddCheckBox(256,guiY,112,32,"2468");
	arrowFor8DirBtn=gui.AddCheckBox(384,guiY,160,32,"12346789");
	guiY+=34;

	assignmentWidget.push_back(zxcTxt);
	assignmentWidget.push_back(ZXCforLMRSpaceBtn);
	assignmentWidget.push_back(arrowTxt);
	assignmentWidget.push_back(arrowForArrowBtn);
	assignmentWidget.push_back(arrowFor4DirBtn);
	assignmentWidget.push_back(arrowFor8DirBtn);
	std::vector <CheapGUI::CheckBox *> arrowBtnGrp;
	arrowBtnGrp.push_back(arrowForArrowBtn);
	arrowBtnGrp.push_back(arrowFor4DirBtn);
	arrowBtnGrp.push_back(arrowFor8DirBtn);
	gui.SetRadioButtonGroup(arrowBtnGrp);

	irToyVersionTxt=gui.AddText(8,guiY,80,32,"");
	protocolVersionTxt=gui.AddText(88,guiY,80,32,"");

	guiY+=34;
	guiY+=34;
	guiY+=34;

	saveCOMPortLogBtn=gui.AddPushButton(8,guiY,440,32,"Save COM Log for Debugging");

	gui.needRedraw=true;
}

void FM77AVKeyboardEmulatorMain::ProcessUserInput(void)
{
	auto key=FsInkey();
	if(FSKEY_ESC==key && (0!=FsGetKeyState(FSKEY_SHIFT) || 0!=FsGetKeyState(FSKEY_CTRL)))
	{
		Quit();
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
		Quit();
	}
	if(secretBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.SendFM77AVSecretMessage();
	}
	if(capsBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.SendStroke(AVKEY_CAPS,false,false,false);
	}
	if(kanaBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.SendStroke(AVKEY_KANA,false,false,false);
	}
	if(pauseBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.SetPause(pauseBtn->GetCheck());
		if(true==fm77avKeyboardEmu.GetPause())
		{
			titleTxt->SetText("FM77AV Keyboard Emulator (PAUSED)");
		}
		else
		{
			titleTxt->SetText("FM77AV Keyboard Emulator");
		}
	}
	if(autoTypingBtn==gui.PeekLastClicked() && fm77avKeyboardEmu.GetIRToyState()==IRToy_Controller::STATE_GOWILD)
	{
		FileDialogOption opt;
		auto fName=SelectFile(opt);
		fm77avKeyboardEmu.StartAutoTyping(fName.c_str(),0);
		ignoreNextLButtonUp=true;
	}
	if(autoTypingBasicBtn==gui.PeekLastClicked() && fm77avKeyboardEmu.GetIRToyState()==IRToy_Controller::STATE_GOWILD)
	{
		FileDialogOption opt;
		auto fName=SelectFile(opt);
		fm77avKeyboardEmu.StartAutoTyping(fName.c_str(),500);
		ignoreNextLButtonUp=true;
	}
	if(autoTypeClipboardBtn==gui.PeekLastClicked() && fm77avKeyboardEmu.GetIRToyState()==IRToy_Controller::STATE_GOWILD)
	{
		auto clipboard=ReadFromClipboard();
		fm77avKeyboardEmu.StartAutoTyping(clipboard,500);
		ignoreNextLButtonUp=true;
	}
	if(autoStopBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.SetAutoStop(autoStopBtn->GetCheck());
	}
	if(autoFireBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.SetAutoFire(autoFireBtn->GetCheck());
	}
	if(translateModeBtn==gui.PeekLastClicked() ||
	   directModeBtn==gui.PeekLastClicked() ||
	   rKanaModeBtn==gui.PeekLastClicked())
	{
		if(translateModeBtn==gui.PeekLastClicked())
		{
			fm77avKeyboardEmu.SetMode(FM77AVKeyboardEmulator::MODE_TRANSLATION);
		}
		else if(directModeBtn==gui.PeekLastClicked())
		{
			fm77avKeyboardEmu.SetMode(FM77AVKeyboardEmulator::MODE_DIRECT);
		}
		else if(rKanaModeBtn==gui.PeekLastClicked())
		{
			fm77avKeyboardEmu.SetMode(FM77AVKeyboardEmulator::MODE_RKANA);
		}
	}
	if(portBtn.size()==availablePort.size())
	{
		for(int i=0; i<portBtn.size(); ++i)
		{
			if(portBtn[i]==gui.PeekLastClicked())
			{
				autoPortScan=false;
				fm77avKeyboardEmu.Connect(availablePort[i]);
				port=availablePort[i];
				break;
			}
		}
	}
	if(ZXCforLMRSpaceBtn==gui.PeekLastClicked())
	{
		if(true==ZXCforLMRSpaceBtn->GetCheck())
		{
			fm77avKeyboardEmu.keyMap.ZXCForLMRSpzce();
		}
		else
		{
			fm77avKeyboardEmu.keyMap.ZXCForZXC();
		}
	}
	if(arrowForArrowBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.keyMap.ArrowForArrow();
		fm77avKeyboardEmu.SetArrowFor8Dir(false);
	}
	if(arrowFor4DirBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.keyMap.ArrowFor2468();
		fm77avKeyboardEmu.SetArrowFor8Dir(false);
	}
	if(arrowFor8DirBtn==gui.PeekLastClicked())
	{
		fm77avKeyboardEmu.keyMap.ArrowForNone();
		fm77avKeyboardEmu.SetArrowFor8Dir(true);
	}
	if(saveCOMPortLogBtn==gui.PeekLastClicked())
	{
		auto log=fm77avKeyboardEmu.GetIRToyCOMTransactionLog();
		FILE *fp=fopen("comlog.bin","wb");
		if(nullptr!=fp)
		{
			auto wSize=fwrite(log.data(),1,log.size(),fp);
			fclose(fp);
			if(wSize==log.size())
			{
				statusText->SetText("Saved comlog.bin");
			}
			else
			{
				statusText->SetText("File write error.");
			}
		}
		else
		{
			statusText->SetText("Cannot open log file.");
		}
	}
	gui.GetLastClicked();

	fm77avKeyboardEmu.ProcessChar(FsInkeyChar());
	bool shift=FsGetKeyState(FSKEY_SHIFT)!=0;
	bool ctrl=FsGetKeyState(FSKEY_CTRL)!=0;
	fm77avKeyboardEmu.ProcessKeyStroke(key,shift,ctrl,false);
	fm77avKeyboardEmu.ProcessKeyState();
}

void FM77AVKeyboardEmulatorMain::RunOneStep(void)
{
	fm77avKeyboardEmu.RunOneStep();
	if(true==autoPortScan &&
	   IRToy_Controller::STATE_ERROR==fm77avKeyboardEmu.GetIRToyState())
	{
		int nextPort=-1;
		for(int i=1; i<availablePort.size(); ++i)
		{
			if(availablePort[i]==port)
			{
				nextPort=availablePort[i-1];
				break;
			}
		}
		if(0<=nextPort)
		{
			fm77avKeyboardEmu.Connect(nextPort);
			port=nextPort;
		}
		else
		{
			autoPortScan=false;
		}
	}
	else if(IRToy_Controller::STATE_GOWILD==fm77avKeyboardEmu.GetIRToyState())
	{
		autoPortScan=false;
	}

	if(IRToy_Controller::STATE_CLOSED==fm77avKeyboardEmu.GetIRToyState())
	{
		quit=true;
	}
}

void FM77AVKeyboardEmulatorMain::UpdateGUI(void)
{
	auto newRKana=fm77avKeyboardEmu.GetRKanaBuf();
	if(prevRKana!=newRKana)
	{
		rKanaTxt->SetText(newRKana.c_str());
	}
	prevRKana=newRKana;

	auto irToyState=fm77avKeyboardEmu.GetIRToyState();
	UpdateIRToyState(statusText,irToyState,prevIRToyState);
	if(irToyState!=prevIRToyState)
	{
		irToyVersionTxt->SetText(fm77avKeyboardEmu.GetIRToyVersion().c_str());
		protocolVersionTxt->SetText(fm77avKeyboardEmu.GetIRToyProtocolVersion().c_str());
	}
	prevIRToyState=irToyState;

	auto irToyError=fm77avKeyboardEmu.GetIRToyErrorCode();
	auto autoTyping=fm77avKeyboardEmu.IsAutoTyping();
	UpdateIRToyErrorCode(errorText,irToyError,prevIRToyError,autoTyping,prevAutoTyping);
	prevIRToyError=irToyError;
	prevAutoTyping=autoTyping;

	auto emuMode=fm77avKeyboardEmu.GetMode();
	if(prevMode!=emuMode)
	{
		for(auto btn : modeBtn)
		{
			btn->SetCheck(false);
		}
		switch(emuMode)
		{
		case FM77AVKeyboardEmulator::MODE_TRANSLATION:
			translateModeBtn->SetCheck(true);
			break;
		case FM77AVKeyboardEmulator::MODE_DIRECT:
			directModeBtn->SetCheck(true);
			break;
		case FM77AVKeyboardEmulator::MODE_RKANA:
			rKanaModeBtn->SetCheck(true);
			break;
		}
	}
	prevMode=emuMode;

	if(port!=prevPort && availablePort.size()==portBtn.size())
	{
		for(int i=0; i<availablePort.size(); ++i)
		{
			if(port==availablePort[i])
			{
				portBtn[i]->SetCheck(true);
			}
			else
			{
				portBtn[i]->SetCheck(false);
			}
		}
	}
	prevPort=port;

	if(FM77AVKeyboardEmulator::MODE_DIRECT==emuMode && true==ZXCforLMRSpaceBtn->hide)
	{
		for(auto btnPtr : assignmentWidget)
		{
			btnPtr->Show();
		}
	}
	else if(FM77AVKeyboardEmulator::MODE_DIRECT!=emuMode && true!=ZXCforLMRSpaceBtn->hide)
	{
		for(auto btnPtr : assignmentWidget)
		{
			btnPtr->Hide();
		}
	}
}

void FM77AVKeyboardEmulatorMain::Draw(void) const
{
	gui.Draw();
}

void FM77AVKeyboardEmulatorMain::Quit(void)
{
	if(IRToy_Controller::STATE_GOWILD!=fm77avKeyboardEmu.GetIRToyState() && 
	   IRToy_Controller::STATE_CLOSING!=fm77avKeyboardEmu.GetIRToyState())
	{
		quit=true;
	}
	else
	{
		fm77avKeyboardEmu.StartCloseIRToySequence();
	}
}

bool FM77AVKeyboardEmulatorMain::NeedRedraw(void) const
{
	return gui.needRedraw;
}

void FM77AVKeyboardEmulatorMain::UpdateIRToyState(CheapGUI::Text *statusText,int irToyState,int prevIRToyState)
{
	if(irToyState!=prevIRToyState)
	{
		switch(irToyState)
		{
		case IRToy_Controller::STATE_DISCONNECT:
			statusText->SetText("Disconnected");
			statusText->SetColor(200,200,0,255);
			break;
		case IRToy_Controller::STATE_HANDSHAKE:
			statusText->SetText("Handshake");
			statusText->SetColor(200,200,0,255);
			break;
		case IRToy_Controller::STATE_INITIALIZING:
			statusText->SetText("Initializing");
			statusText->SetColor(200,200,0,255);
			break;
		case IRToy_Controller::STATE_SELFTEST:
			statusText->SetText("Running Self Test");
			statusText->SetColor(200,200,0,255);
			break;
		case IRToy_Controller::STATE_SETSAMPLERMODE:
			statusText->SetText("Entering the Sampler Mode");
			statusText->SetColor(200,200,0,255);
			break;
		case IRToy_Controller::STATE_CONFIGURE:
			statusText->SetText("Final Configuration");
			statusText->SetColor(200,200,0,255);
			break;
		case IRToy_Controller::STATE_GOWILD:
		case IRToy_Controller::STATE_TRANSMITTING:
			if(prevIRToyState!=IRToy_Controller::STATE_GOWILD &&
			   prevIRToyState!=IRToy_Controller::STATE_TRANSMITTING)
			{
				statusText->SetText("Ready");
				statusText->SetColor(0,128,0,255);
			}
			break;

		case IRToy_Controller::STATE_CLOSING:
			statusText->SetText("Closing Connection");
			statusText->SetColor(200,200,0,255);
			break;
		case IRToy_Controller::STATE_CLOSED:
			statusText->SetText("Closed");
			statusText->SetColor(200,200,0,255);
			break;

		case IRToy_Controller::STATE_ERROR:
			statusText->SetText("Device I/O Error");
			statusText->SetColor(255,0,0,255);
			break;
		}
	}
}

void FM77AVKeyboardEmulatorMain::UpdateIRToyErrorCode(CheapGUI::Text *textPtr,int irToyError,int prevIRToyError,bool autoTyping,bool prevAutoTyping)
{
	if(irToyError!=prevIRToyError || autoTyping!=prevAutoTyping)
	{
		if(true==autoTyping)
		{
			textPtr->SetText("AUTO TYPING...");
			textPtr->SetColor(0,255,0,255);
		}
		else
		{
			switch(irToyError)
			{
			case IRToy_Controller::ERROR_NOERROR:
				textPtr->SetText("");
				textPtr->SetColor(0,128,0,255);
				break;
			case IRToy_Controller::ERROR_INITIALIZATION_TIMEOUT:
				textPtr->SetText("CONDITION:INITIALIZATION TIME OUT");
				textPtr->SetColor(255,0,0,255);
				break;
			case IRToy_Controller::ERROR_PACKET_SEND_FAILURE:
				textPtr->SetText("CONDITION:PACKET_SEND_FAILURE");
				textPtr->SetColor(255,0,0,255);
				break;
			case IRToy_Controller::ERROR_IRTOY_NOT_FOUND:
				textPtr->SetText("CONDITION:IR-DEVICE NOT FOUND");
				textPtr->SetColor(255,0,0,255);
				break;
			}
		}
	}
}



////////////////////////////////////////////////////////////

bool OnCloseWindow(void *incoming)
{
	auto appPtr=(FM77AVKeyboardEmulatorMain *)incoming;
	appPtr->Quit();
	return false;
}

int main(void)
{
	FsOpenWindow(0,0,640,480,1);
	FsChangeToProgramDir();

	CheapGUI::CreateBitmapFontRenderer();



	FM77AVKeyboardEmulatorMain app;
	app.Initialize();
	app.SetUpCheapGUI();

	FsRegisterCloseWindowCallBack(OnCloseWindow,&app);
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

			CheapGUI::SetBitmapFontRendererViewportSize(wid,hei);

			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glViewport(0,0,wid,hei);
			app.Draw();
			FsSwapBuffers();
		}

		FsSleep(1); // Give some CPU time for COM driver.
	}

	CheapGUI::DeleteBitmapFontRenderer();

	return 0;
}

