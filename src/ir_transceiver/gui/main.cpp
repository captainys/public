#include <string>
#include <vector>
#include <chrono>


#include <stdio.h>


#include "ysclass.h"
#include "ysport.h"

#include "ysgl.h"
#include "ysglsldrawfontbitmap.h"
#include "fssimplewindow.h"
#include "fssimplefiledialog.h"
#include "gui.h"
#include "comport.h"
#include "irtoy.h"
#include "irdata.h"



class IRSensorGuiMain
{
public:
	enum
	{
		pulseHeightInPixel=30,
		rawHeightInPixel=40,
		raw0Y=400,
	};

private:
	int currentIRData;
	IRDataSet irDataSet;
	std::vector <unsigned char> recvBuf;

	decltype(std::chrono::system_clock::now()) messageTimer;

	bool fNameSpecifiedInArgv;
	YsWString fName;

	IRData *GetCurrent(void);
	const IRData *GetCurrent(void) const;

	FsSimpleFileDialog fdlg;

	YsCOMPort comPort;	// Receiver	
	IRToy_Controller transmitter;

	bool recvPortNameSpecified;
	std::string recvPortName;
	int baudRate;

	bool transPortNameSpecified;
	std::string transPortName;

	enum GUISTATE
	{
		GUI_MAIN,
		GUI_CONNECT
	};

	GUISTATE guiState;

	CheapGUI gui;
	bool terminateFlag;
	bool ignoreNextLButtonUp;

	// GUI >>
	CheapGUI::Text *titleTxt;
	CheapGUI::Text *timedMsgTxt;
	CheapGUI::PushButton *exitBtn;
	CheapGUI::Text *statusText;
	CheapGUI::PushButton *connectBtn;
	CheapGUI::Text *transStatusTxt;
	CheapGUI::PushButton *transConnectBtn;
	CheapGUI::PushButton *zoomBtn,*moozBtn,*leftBtn,*rightBtn,*homeBtn;
	CheapGUI::CheckBox *recordBtn;
	CheapGUI::PushButton *transmitBtn;
	CheapGUI::PushButton *deleteBtn,*moveUpBtn,*moveDwnBtn;
	CheapGUI::PushButton *saveBtn,*saveAsBtn,*openBtn;
	CheapGUI::PushButton *prevDataBtn,*nextDataBtn,*newDataBtn,*deleteDataBtn,*confirmDeleteBtn;
	CheapGUI::TextInput *dataNameTxt;
	// GUI <<

	// >>
	class ConnectDlg : public CheapGUI
	{
	public:
		bool forReceiver;
		CheapGUI::PushButton *backBtn,*connectBtn;
		CheapGUI::TextInput *fNameTxt;
		void Make(void);
	};
	ConnectDlg connectDlg;
	// <<


public:
	IRSensorGuiMain();

	bool Terminate(void) const;
	bool RecognizeCommandParameter(int ac,char *av[]);
	bool Configure(void);
	void Make(void);

	void ProcessUserInput(void);
	void ProcessUserInput_Main(int key,int c,int mouseEvt);
	void ProcessUserInput_ConnectDlg(int key,int c,int mouseEvt);
	void RefreshDataName(void);

	void Zoom(void);
	void Mooz(void);
	void Left(void);
	void Right(void);
	void Home(void);
	void Transmit(void);
	void Delete(void);
	void MoveUp(void);
	void MoveDown(void);
	void MoveCursorUp(void);
	void MoveCursorDown(void);
	void Save(void);
	void SaveAs(void);
	void SaveAs_FileSelected(FsSimpleFileDialog *,int);
	void Open(void);
	void Open_FileSelected(FsSimpleFileDialog *,int);
	bool Open(const YsWString &fName);
	void CopyVizSetting(IRData *to,const IRData *from);
	void NextData(void);
	void PrevData(void);
	void NewData(void);
	void DeleteData(void);
	void ConfirmDeleteData(void);

	void RunOneStep(void);
	void CheckTransmission(std::vector <unsigned char> &recv);

	void UpdateGUI(void);
	void Draw(void) const;
};

IRSensorGuiMain::IRSensorGuiMain()
{
	recvPortNameSpecified=false;
	transPortNameSpecified=false;
	baudRate=115200;

	currentIRData=0;

	terminateFlag=false;
	ignoreNextLButtonUp=false;

	irDataSet.MakeUpTestData();

	guiState=GUI_MAIN;
}

IRData *IRSensorGuiMain::GetCurrent(void)
{
	if(0<=currentIRData && currentIRData<irDataSet.dataSet.size())
	{
		return &irDataSet.dataSet[currentIRData];
	}
	return nullptr;
}
const IRData *IRSensorGuiMain::GetCurrent(void) const
{
	if(0<=currentIRData && currentIRData<irDataSet.dataSet.size())
	{
		return &irDataSet.dataSet[currentIRData];
	}
	return nullptr;
}

bool IRSensorGuiMain::Terminate(void) const
{
	return terminateFlag;
}

bool IRSensorGuiMain::RecognizeCommandParameter(int ac,char *av[])
{
	for(int i=1; i<ac; ++i)
	{
		std::string s(av[i]);
		if((s=="-receive" || s=="-recv") && i+1<ac)
		{
			recvPortNameSpecified=true;
			recvPortName=av[i+1];
			++i;
		}
		else if((s=="-transmit" || s=="-trans" || s=="-trns") && i+1<ac)
		{
			transPortNameSpecified=true;
			transPortName=av[i+1];
			++i;
		}
		else
		{
			fNameSpecifiedInArgv=true;
			fName.SetUTF8String(av[i]);
		}
	}
	return true;
}

bool IRSensorGuiMain::Configure(void)
{
	if(true==recvPortNameSpecified)
	{
		comPort.SetDesiredBaudRate(baudRate);
		comPort.Open(recvPortName.c_str());
	}
	if(true==transPortNameSpecified)
	{
		transmitter.Connect(transPortName.c_str());
	}

	if(true==fNameSpecifiedInArgv)
	{
		Open(fName);
	}

	return true;
}

void IRSensorGuiMain::Make(void)
{
	messageTimer=std::chrono::system_clock::now();

	int guiY=8;

	exitBtn=gui.AddPushButton(8,guiY,240,28,"EXIT(ESC)");
	timedMsgTxt=gui.AddText(8+256,guiY,768,28,"");
	guiY+=30;

	titleTxt=gui.AddText(8,guiY,400,28,"Infra Red Scanner by CaptainYS (http://www.ysflight.com)");
	guiY+=30;

	gui.AddText(8,guiY,240,28,"RECEIVER:");
	connectBtn=gui.AddPushButton(8+256,guiY,240,28,"CONNECT");
	statusText=gui.AddText(8+512,guiY,400,28,"STATUS");
	guiY+=30;

	gui.AddText(8,guiY,240,28,"TRANSMITTER:");
	transConnectBtn=gui.AddPushButton(8+256,guiY,240,28,"CONNECT");
	transStatusTxt=gui.AddText(8+512,guiY,400,28,"STATUS");
	guiY+=30;

	zoomBtn= gui.AddPushButton(8+0  , guiY,240,28,"ZOOM(PgUp)");
	moozBtn= gui.AddPushButton(8+256, guiY,240,28,"MOOZ(PgDn)");
	leftBtn= gui.AddPushButton(8+512, guiY,240,28,"LEFT(<-)");
	rightBtn=gui.AddPushButton(8+768, guiY,240,28,"RIGHT(->)");
	homeBtn= gui.AddPushButton(8+1024,guiY,240,28,"HOME(HOME)");
	guiY+=30;

	recordBtn=gui.AddCheckBox(8,guiY,240,28,"RECORD");
	transmitBtn=gui.AddPushButton(8+256,guiY,240,28,"TRANSMIT");
	guiY+=30;

	deleteBtn=gui.AddPushButton(8+0   ,guiY,240,28,"DELETE(DEL)");
	moveUpBtn=gui.AddPushButton(8+256   ,guiY,240,28,"MOVE UP");
	moveDwnBtn=gui.AddPushButton(8+512   ,guiY,240,28,"MOVE DOWN");
	guiY+=30;

	saveBtn=gui.AddPushButton(8+0     ,guiY,240,28,"SAVE");
	saveAsBtn=gui.AddPushButton(8+256     ,guiY,240,28,"SAVE AS");
	openBtn=gui.AddPushButton(8+512   ,guiY,240,28,"OPEN");
	guiY+=30;

	prevDataBtn=gui.AddPushButton(8+0        ,guiY,240,28,"PREV(Ctl+Shft+Tab)");
	nextDataBtn=gui.AddPushButton(8+256      ,guiY,240,28,"NEXT(Ctl+Tab)");
	newDataBtn=gui.AddPushButton(8+512       ,guiY,240,28,"NEW DATA");
	deleteDataBtn=gui.AddPushButton(8+768    ,guiY,240,28,"DELETE DATA");
	confirmDeleteBtn=gui.AddPushButton(8+1024,guiY,240,28,"CONFIRM DELETE");
	confirmDeleteBtn->Hide();
	guiY+=30;

	gui.AddText                 (8    ,guiY,240,28,"NAME:");
	dataNameTxt=gui.AddTextInput(8+256,guiY,640,28,"");

	connectDlg.Make();
}

void IRSensorGuiMain::ConnectDlg::Make(void)
{
	this->forReceiver=forReceiver;

	int guiY=8;

	AddText(8,guiY,400,28,"Connect to Port");
	guiY+=30;

	fNameTxt=AddTextInput(8,guiY,1024,28,"");
	guiY+=30;

	backBtn=AddPushButton(8,       guiY,240,28,"BACK(ESC)");
	connectBtn=AddPushButton(8+256,guiY,240,28,"CONNECT");
	guiY+=30;
}

void IRSensorGuiMain::ProcessUserInput(void)
{
	int lb,mb,rb,mx,my;
	auto mouseEvt=FsGetMouseEvent(lb,mb,rb,mx,my);
	auto key=FsInkey();
	auto c=FsInkeyChar();

	switch(guiState)
	{
	case GUI_MAIN:
		ProcessUserInput_Main(key,c,mouseEvt);
		break;
	case GUI_CONNECT:
		ProcessUserInput_ConnectDlg(key,c,mouseEvt);
		break;
	}

	// Common input
	switch(key)
	{
	case FSKEY_HOME:
		Home();
		break;
	case FSKEY_LEFT:
		Left();
		break;
	case FSKEY_RIGHT:
		Right();
		break;
	case FSKEY_PAGEUP:
		Zoom();
		break;
	case FSKEY_PAGEDOWN:
		Mooz();
		break;
	case FSKEY_DOWN:
		MoveCursorDown();
		break;
	case FSKEY_UP:
		MoveCursorUp();
		break;
	case FSKEY_TAB:
		if(0!=FsGetKeyState(FSKEY_CTRL))
		{
			if(0==FsGetKeyState(FSKEY_SHIFT))
			{
				NextData();
			}
			else
			{
				PrevData();
			}
		}
		break;
	}

	if(FSMOUSEEVENT_LBUTTONDOWN==mouseEvt)
	{
		ignoreNextLButtonUp=false;
		if(raw0Y<=my)
		{
			auto currentPtr=GetCurrent();
			if(nullptr!=currentPtr)
			{
				int i=(my-raw0Y)/rawHeightInPixel;
				if(0<=i && i<currentPtr->sample.size())
				{
					currentPtr->selSample=i;
				}
			}
		}
	}
}

void IRSensorGuiMain::ProcessUserInput_Main(int key,int c,int mouseEvt)
{
	switch(key)
	{
	case FSKEY_ESC:
		terminateFlag=true;
		break;
	case FSKEY_DEL:
		Delete();
		break;
	}

	if(0!=c)
	{
		gui.NotifyChar(c);
	}

	int lb,mb,rb,mx,my;
	FsGetMouseState(lb,mb,rb,mx,my);
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

	if(true==dataNameTxt->Edited())
	{
		auto currentPtr=GetCurrent();
		if(nullptr!=currentPtr)
		{
			currentPtr->label=dataNameTxt->GetText();
		}
	}

	if(exitBtn==gui.PeekLastClicked())
	{
		terminateFlag=true;
	}
	else if(connectBtn==gui.PeekLastClicked())
	{
		connectDlg.forReceiver=true;
		connectDlg.fNameTxt->SetText("");
		guiState=GUI_CONNECT;
	}
	else if(transConnectBtn==gui.PeekLastClicked())
	{
		connectDlg.forReceiver=false;
		connectDlg.fNameTxt->SetText("");
		guiState=GUI_CONNECT;
	}
	else if(zoomBtn==gui.PeekLastClicked())
	{
		Zoom();
	}
	else if(moozBtn==gui.PeekLastClicked())
	{
		Mooz();
	}
	else if(leftBtn==gui.PeekLastClicked())
	{
		Left();
	}
	else if(rightBtn==gui.PeekLastClicked())
	{
		Right();
	}
	else if(homeBtn==gui.PeekLastClicked())
	{
		Home();
	}
	else if(recordBtn==gui.PeekLastClicked())
	{
		if(true==recordBtn->GetCheck())
		{
			printf("Start Recording\n");
			recvBuf.clear();
			while(0<comPort.Receive().size())
			{
			}
		}
		else
		{
			printf("End Recording\n");
		}
	}
	else if(transmitBtn==gui.PeekLastClicked())
	{
		Transmit();
	}
	else if(deleteBtn==gui.PeekLastClicked())
	{
		Delete();
	}
	else if(moveUpBtn==gui.PeekLastClicked())
	{
		MoveUp();
	}
	else if(moveDwnBtn==gui.PeekLastClicked())
	{
		MoveDown();
	}
	else if(saveBtn==gui.PeekLastClicked())
	{
		Save();
	}
	else if(saveAsBtn==gui.PeekLastClicked())
	{
		SaveAs();
	}
	else if(openBtn==gui.PeekLastClicked())
	{
		Open();
	}
	else if(nextDataBtn==gui.PeekLastClicked())
	{
		NextData();
	}
	else if(prevDataBtn==gui.PeekLastClicked())
	{
		PrevData();
	}
	else if(newDataBtn==gui.PeekLastClicked())
	{
		NewData();
	}
	else if(deleteDataBtn==gui.PeekLastClicked())
	{
		DeleteData();
	}
	else if(confirmDeleteBtn==gui.PeekLastClicked())
	{
		ConfirmDeleteData();
	}
	gui.GetLastClicked();
}
void IRSensorGuiMain::ProcessUserInput_ConnectDlg(int key,int c,int mouseEvt)
{
	auto &gui=connectDlg;

	switch(key)
	{
	case FSKEY_ESC:
		guiState=GUI_MAIN;
		break;
	case FSKEY_BS:
		if(true!=gui.fNameTxt->active)
		{
			guiState=GUI_MAIN;
		}
		break;
	}

	if(0!=c)
	{
		connectDlg.NotifyChar(c);
	}

	int lb,mb,rb,mx,my;
	FsGetMouseState(lb,mb,rb,mx,my);
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

	if(connectDlg.backBtn==gui.PeekLastClicked())
	{
		guiState=GUI_MAIN;
		ignoreNextLButtonUp=true;
	}
	else if(connectDlg.connectBtn==gui.PeekLastClicked())
	{
		if(true==connectDlg.forReceiver)
		{
			comPort.Close();
			comPort.SetDesiredBaudRate(baudRate);
			if(true==comPort.Open(connectDlg.fNameTxt->GetText()))
			{
				guiState=GUI_MAIN;
			}
			ignoreNextLButtonUp=true;
		}
		else
		{
			transmitter.Connect(connectDlg.fNameTxt->GetText());
			guiState=GUI_MAIN;
			ignoreNextLButtonUp=true;
		}
		UpdateGUI();
	}
	gui.GetLastClicked();
}

void IRSensorGuiMain::RefreshDataName(void)
{
	auto current=GetCurrent();
	if(nullptr!=current)
	{
		dataNameTxt->SetText(current->label.c_str());
	}
}

void IRSensorGuiMain::Zoom(void)
{
	auto toDrawPtr=GetCurrent();
	if(nullptr!=toDrawPtr)
	{
		if(toDrawPtr->vizZoom<10000.0)
		{
			toDrawPtr->vizZoom*=1.25f;
		}
	}
}
void IRSensorGuiMain::Mooz(void)
{
	auto toDrawPtr=GetCurrent();
	if(nullptr!=toDrawPtr)
	{
		if(0.001f<toDrawPtr->vizZoom)
		{
			toDrawPtr->vizZoom/=1.25f;
		}
	}
}
void IRSensorGuiMain::Left(void)
{
	auto toDrawPtr=GetCurrent();
	if(nullptr!=toDrawPtr)
	{
		int wid,hei;
		FsGetWindowSize(wid,hei);
		int d=(int)((float)wid/toDrawPtr->vizZoom);
		toDrawPtr->vizTop-=(d/4);
		if(toDrawPtr->vizTop<0)
		{
			toDrawPtr->vizTop=0;
		}
	}
}
void IRSensorGuiMain::Right(void)
{
	auto toDrawPtr=GetCurrent();
	if(nullptr!=toDrawPtr)
	{
		int wid,hei;
		FsGetWindowSize(wid,hei);
		int d=(int)((float)wid/toDrawPtr->vizZoom);
		toDrawPtr->vizTop+=(d/4);
	}
}
void IRSensorGuiMain::Home(void)
{
	auto toDrawPtr=GetCurrent();
	if(nullptr!=toDrawPtr)
	{
		toDrawPtr->vizTop=0;
	}
}

void IRSensorGuiMain::Transmit(void)
{
	if(IRToy_Controller::STATE_GOWILD==transmitter.GetState())
	{
		auto dataPtr=GetCurrent();
		if(0<=dataPtr->selSample && dataPtr->selSample<dataPtr->sample.size())
		{
			auto sample=dataPtr->PWMtoModulated(dataPtr->sample[dataPtr->selSample]);
			transmitter.StartTransmitMicroSecPulse(sample.size(),sample.data());
		}
	}
}

void IRSensorGuiMain::Delete(void)
{
	auto currentPtr=GetCurrent();
	if(nullptr!=currentPtr)
	{
		if(0<=currentPtr->selSample && currentPtr->selSample<currentPtr->sample.size())
		{
			currentPtr->sample.erase(currentPtr->sample.begin()+currentPtr->selSample);
			if(0<currentPtr->selSample && currentPtr->sample.size()<=currentPtr->selSample)
			{
				--currentPtr->selSample;
			}
		}
	}
}
void IRSensorGuiMain::MoveUp(void)
{
	auto currentPtr=GetCurrent();
	if(nullptr!=currentPtr)
	{
		if(0<currentPtr->selSample && currentPtr->selSample<currentPtr->sample.size())
		{
			std::swap(
			    currentPtr->sample[currentPtr->selSample],
			    currentPtr->sample[currentPtr->selSample-1]);
			--currentPtr->selSample;
		}
	}
}
void IRSensorGuiMain::MoveDown(void)
{
	auto currentPtr=GetCurrent();
	if(nullptr!=currentPtr)
	{
		if(0<=currentPtr->selSample && currentPtr->selSample+1<currentPtr->sample.size())
		{
			std::swap(
			    currentPtr->sample[currentPtr->selSample],
			    currentPtr->sample[currentPtr->selSample+1]);
			++currentPtr->selSample;
		}
	}
}

void IRSensorGuiMain::MoveCursorUp(void)
{
	auto currentPtr=GetCurrent();
	if(nullptr!=currentPtr && 
	   0<currentPtr->selSample)
	{
		--currentPtr->selSample;
	}
}
void IRSensorGuiMain::MoveCursorDown(void)
{
	auto currentPtr=GetCurrent();
	if(nullptr!=currentPtr &&
	   currentPtr->selSample+1<currentPtr->sample.size())
	{
		++currentPtr->selSample;
	}
}
void IRSensorGuiMain::Save(void)
{
	if(0==fName.size())
	{
		SaveAs();
	}

	YsFileIO::File fp(fName,"w");
	if(nullptr!=fp.Fp())
	{
		irDataSet.Save(fp);
		timedMsgTxt->SetText("SAVED.");
		messageTimer=
		    std::chrono::system_clock::now()+
		    std::chrono::seconds(3);
	}
	else
	{
		SaveAs();
	}

}
void IRSensorGuiMain::SaveAs(void)
{
	fdlg.mode=fdlg.MODE_SAVE;
	fdlg.fileExtensionArray.clear();
	fdlg.fileExtensionArray.push_back(L"*.txt");
	fdlg.defaultFileName=fName;
	fdlg.title=L"Save File";
	fdlg.BindCloseModalCallBack(&IRSensorGuiMain::SaveAs_FileSelected,this);
	fdlg.Show();
}
void IRSensorGuiMain::SaveAs_FileSelected(FsSimpleFileDialog *fdlg,int returnCode)
{
	if(0!=returnCode && 0<fdlg->selectedFileArray.size())
	{
		auto fName=fdlg->selectedFileArray[0];
		YsFileIO::File fp(fName,"w");
		if(nullptr!=fp.Fp())
		{
			irDataSet.Save(fp);
			this->fName=fName;

			timedMsgTxt->SetText("SAVED.");
			messageTimer=
			    std::chrono::system_clock::now()+
			    std::chrono::seconds(3);
		}
	}
}
void IRSensorGuiMain::Open(void)
{
	fdlg.mode=fdlg.MODE_OPEN;
	fdlg.fileExtensionArray.clear();
	fdlg.fileExtensionArray.push_back(L"*.txt");
	fdlg.defaultFileName=fName;
	fdlg.title=L"Open File";
	fdlg.BindCloseModalCallBack(&IRSensorGuiMain::Open_FileSelected,this);
	fdlg.Show();
}
void IRSensorGuiMain::Open_FileSelected(FsSimpleFileDialog *fdlg,int returnCode)
{
	if(0!=returnCode && 0<fdlg->selectedFileArray.size())
	{
		if(true==Open(fdlg->selectedFileArray[0]))
		{
			RefreshDataName();
		}
	}
}
bool IRSensorGuiMain::Open(const YsWString &fName)
{
	YsFileIO::File fp(fName,"r");
	if(nullptr!=fp.Fp())
	{
		irDataSet.Load(fp);
		this->fName=fName;
		currentIRData=0;
		return true;
	}
	return false;
}
void IRSensorGuiMain::CopyVizSetting(IRData *to,const IRData *from)
{
	to->vizTop=from->vizTop;
	to->vizZoom=from->vizZoom;
}
void IRSensorGuiMain::NextData(void)
{
	auto prevCurrentPtr=GetCurrent();
	if(currentIRData+1<irDataSet.dataSet.size())
	{
		++currentIRData;
	}
	else
	{
		currentIRData=0;
	}
	if(nullptr!=prevCurrentPtr)
	{
		CopyVizSetting(GetCurrent(),prevCurrentPtr);
	}
	RefreshDataName();
}
void IRSensorGuiMain::PrevData(void)
{
	auto prevCurrentPtr=GetCurrent();
	if(0<currentIRData)
	{
		--currentIRData;
	}
	else if(0<irDataSet.dataSet.size())
	{
		currentIRData=irDataSet.dataSet.size()-1;
	}
	if(nullptr!=prevCurrentPtr)
	{
		CopyVizSetting(GetCurrent(),prevCurrentPtr);
	}
	RefreshDataName();
}
void IRSensorGuiMain::NewData(void)
{
	auto prevCurrentPtr=GetCurrent();

	IRData newDat;
	char str[256];
	sprintf(str,"Data%lld",irDataSet.dataSet.size()+1);
	newDat.label=str;
	irDataSet.dataSet.push_back(newDat);
	currentIRData=irDataSet.dataSet.size()-1;
	if(nullptr!=prevCurrentPtr)
	{
		CopyVizSetting(GetCurrent(),prevCurrentPtr);
	}
	RefreshDataName();
}
void IRSensorGuiMain::DeleteData(void)
{
	if(true==confirmDeleteBtn->hide)
	{
		confirmDeleteBtn->hide=false;
	}
	else
	{
		confirmDeleteBtn->hide=true;
	}
}
void IRSensorGuiMain::ConfirmDeleteData(void)
{
	if(0<=currentIRData && currentIRData<irDataSet.dataSet.size())
	{
		auto prevCurrentPtr=GetCurrent();

		irDataSet.dataSet.erase(irDataSet.dataSet.begin()+currentIRData);
		if(0<currentIRData && irDataSet.dataSet.size()<=currentIRData)
		{
			--currentIRData;
		}
		if(nullptr!=prevCurrentPtr)
		{
			CopyVizSetting(GetCurrent(),prevCurrentPtr);
		}
		RefreshDataName();
	}
}

void IRSensorGuiMain::RunOneStep(void)
{
	transmitter.RunOneStep();
	if(true==recordBtn->GetCheck())
	{
		auto recv=comPort.Receive();
		if(0<recv.size())
		{
			recvBuf.insert(recvBuf.end(),recv.begin(),recv.end());
			CheckTransmission(recvBuf);
		}
	}
	FsPushOnPaintEvent();
}

void IRSensorGuiMain::CheckTransmission(std::vector <unsigned char> &recv)
{
	long long int beginPtr=-1,endPtr=-1;
	for(long long int ptr=0; ptr+6<recv.size(); ++ptr)
	{
		if(('b'==recv[ptr  ] || 'B'==recv[ptr  ])&&
		   ('e'==recv[ptr+1] || 'E'==recv[ptr+1])&&
		   ('g'==recv[ptr+2] || 'G'==recv[ptr+2])&&
		   ('i'==recv[ptr+3] || 'I'==recv[ptr+3])&&
		   ('n'==recv[ptr+4] || 'N'==recv[ptr+4]))
		{
			beginPtr=ptr+5;
			break;
		}
	}
	if(0<=beginPtr)
	{
		for(long long int ptr=beginPtr; ptr<recv.size(); ++ptr)
		{
			if(('e'==recv[ptr  ] || 'E'==recv[ptr  ])&&
			   ('n'==recv[ptr+1] || 'N'==recv[ptr+1])&&
			   ('d'==recv[ptr+2] || 'D'==recv[ptr+2]))
			{
				endPtr=ptr;
				break;
			}
		}
	}
	if(0<=beginPtr && beginPtr<endPtr)
	{
		std::vector <unsigned int> microsPulse;
		bool inNumber=false;
		bool hexadecimal=false;
		for(auto ptr=beginPtr; ptr<endPtr; ++ptr)
		{
			if(true!=inNumber)
			{
				if('0'<=recv[ptr] && recv[ptr]<='9')
				{
					microsPulse.push_back(recv[ptr]-'0');
					inNumber=true;
					hexadecimal=false;
				}
			}
			else // if(true==inNumber)
			{
				if(true!=hexadecimal)
				{
					if('x'==recv[ptr])
					{
						hexadecimal=true;
					}
					else if('0'<=recv[ptr] && recv[ptr]<='9')
					{
						microsPulse.back()*=10;
						microsPulse.back()+=(recv[ptr]-'0');
					}
					else
					{
						inNumber=false;
					}
				}
				else
				{
					if('0'<=recv[ptr] && recv[ptr]<='9')
					{
						microsPulse.back()*=16;
						microsPulse.back()+=(recv[ptr]-'0');
					}
					else if('a'<=recv[ptr] && recv[ptr]<='f')
					{
						microsPulse.back()*=16;
						microsPulse.back()+=(recv[ptr]+10-'a');
					}
					else if('A'<=recv[ptr] && recv[ptr]<='F')
					{
						microsPulse.back()*=16;
						microsPulse.back()+=(recv[ptr]+10-'A');
					}
					else
					{
						printf("%d\n",microsPulse.back());
						inNumber=false;
					}
				}
			}
		}

		std::string verify;
		verify.insert(verify.end(),recv.begin()+beginPtr,recv.begin()+endPtr);
		printf("%s\n",verify.c_str());

		int total=0;
		for(auto h : microsPulse)
		{
			total+=h;
		}
		printf("Total=%d\n",total);


		auto current=GetCurrent();
		if(nullptr!=current)
		{
			current->sample.push_back(microsPulse);
			recvBuf.clear();
		}
	}
}

void IRSensorGuiMain::UpdateGUI(void)
{
	if(true!=comPort.IsConnected())
	{
		statusText->SetText("STATUS:COM Disconnected");
		statusText->SetColor(255,0,0,255);
	}
	else
	{
		statusText->SetText("STATUS:COM Connected");
		statusText->SetColor(0,255,0,255);
	}

	switch(transmitter.GetState())
	{
	case IRToy_Controller::STATE_DISCONNECT:
		transStatusTxt->SetText("STATUS:DISCONNECTED");
		transStatusTxt->SetColor(255,0,0,255);
		break;

	case IRToy_Controller::STATE_INITIALIZING:
		transStatusTxt->SetText("STATUS:INITIALIZING");
		transStatusTxt->SetColor(255,255,0,255);
		break;
	case IRToy_Controller::STATE_HANDSHAKE:
		transStatusTxt->SetText("STATUS:HANDSHAKE");
		transStatusTxt->SetColor(255,255,0,255);
		break;
	case IRToy_Controller::STATE_SELFTEST:
		transStatusTxt->SetText("STATUS:SELFTEST");
		transStatusTxt->SetColor(255,255,0,255);
		break;
	case IRToy_Controller::STATE_SETSAMPLERMODE:
	case IRToy_Controller::STATE_CONFIGURE:
		transStatusTxt->SetText("STATUS:SETTING UP");
		transStatusTxt->SetColor(255,255,0,255);
		break;
	case IRToy_Controller::STATE_GOWILD:
		transStatusTxt->SetText("STATUS:READY");
		transStatusTxt->SetColor(0,255,0,255);
		break;
	case IRToy_Controller::STATE_TRANSMITTING:
		transStatusTxt->SetText("STATUS:TRANSMITTING");
		transStatusTxt->SetColor(0,255,0,255);
		break;
	case IRToy_Controller::STATE_ARDUINO_WAITING_READY:
		transStatusTxt->SetText("STATUS:WAITING READY SIGNAL");
		transStatusTxt->SetColor(255,255,0,255);
		break;

	case IRToy_Controller::STATE_CLOSING:
		transStatusTxt->SetText("STATUS:CLOSING");
		transStatusTxt->SetColor(255,255,0,255);
		break;
	case IRToy_Controller::STATE_CLOSED:
		transStatusTxt->SetText("STATUS:CLOSED");
		transStatusTxt->SetColor(255,0,0,255);
		break;

	case IRToy_Controller::STATE_ERROR:
		transStatusTxt->SetText("STATUS:ERROR!");
		transStatusTxt->SetColor(255,0,0,255);
		break;
	}

	if(transmitter.GetState()==IRToy_Controller::STATE_GOWILD)
	{
		transmitBtn->hide=false;
	}
	else
	{
		transmitBtn->hide=true;
	}

	if(""!=timedMsgTxt->label &&
	   messageTimer<std::chrono::system_clock::now())
	{
		timedMsgTxt->SetText("");
	}
}

void IRSensorGuiMain::Draw(void) const
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	CheapGUI::SetBitmapFontRendererViewportSize(wid,hei);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,wid,hei);
	switch(guiState)
	{
	case GUI_MAIN:
		gui.Draw();
		break;
	case GUI_CONNECT:
		connectDlg.Draw();
		break;
	}

	const int vSpace=(rawHeightInPixel-pulseHeightInPixel)/2;


	auto toDrawPtr=GetCurrent();
	if(nullptr!=toDrawPtr)
	{
		auto &toDraw=*toDrawPtr;

		{
			auto *fontRenderer=YsGLSLSharedBitmapFontRenderer();
			auto *plain2dRenderer=YsGLSLSharedPlain2DRenderer();

			int y0=raw0Y;
			std::vector <float> vtx;
			for(int us=100; us<=10000; us*=10)
			{
				float intervalInPix=(float)us*toDraw.vizZoom;
				if(50.0f<=intervalInPix)
				{
					auto t=((toDraw.vizTop+us-1)/us)*us;
					for(;;)
					{
						float x=(float)(t-toDraw.vizTop)*toDraw.vizZoom;
						if(wid<(int)x)
						{
							break;
						}
						vtx.push_back(x);
						vtx.push_back(y0);
						vtx.push_back(x);
						vtx.push_back(y0-8);
						t+=us;
					}
				}
				y0-=8;
			}
			YsGLSLUsePlain2DRenderer(plain2dRenderer);
			YsGLSLDrawPlain2DPrimitiveVtxfv(plain2dRenderer,GL_LINES,vtx.size()/2,vtx.data());
			YsGLSLEndUsePlain2DRenderer(plain2dRenderer);

			YsGLSLUseBitmapFontRenderer(fontRenderer);
			{
				int us=1000;
				for(;;)
				{
					float intervalInPix=(float)us*toDraw.vizZoom;
					if(50.0f<intervalInPix)
					{
						break;
					}
					us*=10;
				}

				auto t=((toDraw.vizTop+us-1)/us)*us;
				for(;;)
				{
					float x=(float)(t-toDraw.vizTop)*toDraw.vizZoom;
					if(wid<(int)x)
					{
						break;
					}
					char str[256];
					sprintf(str,"%dms",t/1000);
					YsGLSLRenderBitmapFontString2D(fontRenderer,(int)x,y0,str);
					t+=us;
				}
			}

			y0-=16;
			YsGLSLRenderBitmapFontString2D(fontRenderer,0,y0,toDraw.label.c_str());

			YsGLSLEndUseBitmapFontRenderer(fontRenderer);
		}


		auto renderer=YsGLSLSharedPlain2DRenderer();
		YsGLSLUsePlain2DRenderer(renderer);

		int y0=raw0Y;
		for(int sampleIdx=0; sampleIdx<toDraw.sample.size() && y0<hei; ++sampleIdx)
		{
			std::vector <float> vtx;

			if(toDraw.selSample==sampleIdx)
			{
				const GLfloat col[4]={0.7,0.7,0.7,1};
				YsGLSLSetPlain2DRendererUniformColor(renderer,col);

				GLfloat vtx[8];
				vtx[0]=(0);
				vtx[1]=(y0);

				vtx[2]=(wid);
				vtx[3]=(y0);

				vtx[4]=(wid);
				vtx[5]=(y0+rawHeightInPixel);

				vtx[6]=(0);
				vtx[7]=(y0+rawHeightInPixel);

				YsGLSLDrawPlain2DPrimitiveVtxfv(renderer,GL_TRIANGLE_FAN,4,vtx);
			}

			const GLfloat col[4]={0,0,1,1};
			YsGLSLSetPlain2DRendererUniformColor(renderer,col);

			int onOff=pulseHeightInPixel,t=0;
			float x=(float)(t-toDraw.vizTop)*toDraw.vizZoom;
			vtx.push_back(x);
			vtx.push_back(y0+pulseHeightInPixel+vSpace);
			for(auto h : toDraw.sample[sampleIdx])
			{
				x=(float)(t-toDraw.vizTop)*toDraw.vizZoom;
				int y=y0+pulseHeightInPixel-onOff+vSpace;
				vtx.push_back(x);
				vtx.push_back(y);
				t+=h;

				x=(float)(t-toDraw.vizTop)*toDraw.vizZoom;
				vtx.push_back(x);
				vtx.push_back(y);

				onOff=pulseHeightInPixel-onOff;
			}
			vtx.push_back(x);
			vtx.push_back(y0+pulseHeightInPixel+vSpace);

			YsGLSLDrawPlain2DPrimitiveVtxfv(renderer,GL_LINE_STRIP,vtx.size()/2,vtx.data());

			y0+=rawHeightInPixel;
		}

		YsGLSLEndUsePlain2DRenderer(renderer);
	}

	FsSwapBuffers();
}

////////////////////////////////////////////////////////////

void Render(void *dat)
{
	IRSensorGuiMain *mainPtr=(IRSensorGuiMain*)(dat);
	mainPtr->Draw();
}

int main(int ac,char *av[])
{
	IRSensorGuiMain main;

	FsOpenWindow(0,0,1280,800,1);
	FsRegisterOnPaintCallBack(Render,&main);

	// FsChangeToProgramDir();

	CheapGUI::CreateBitmapFontRenderer();

	if(true!=main.RecognizeCommandParameter(ac,av))
	{
		printf("Usage:\n");
		printf("  IRscannergui <filename.txt> <options>\n");
		printf("Options:\n");
		printf("  -receive portName\n");
		printf("  -recv portName\n");
		printf("     Specify receiver port name.\n");
		printf("     In Windows, just give port number.\n");
		printf("  -transmit portName\n");
		printf("  -trans portName\n");
		printf("  -trns portName\n");
		printf("     Specify transmitter port name.\n");
		printf("     In Windows, just give port number.\n");
		return 1;
	}

	{
		auto *fontRenderer=YsGLSLSharedBitmapFontRenderer();
		YsGLSLUseBitmapFontRenderer(fontRenderer);
		YsGLSLBitmapFontRendererRequestFontSize(fontRenderer,8,16);
		YsGLSLEndUseBitmapFontRenderer(fontRenderer);
	}

	main.Configure();
	main.Make();
	main.RefreshDataName();

	while(true!=main.Terminate())
	{
		FsPollDevice();
		main.ProcessUserInput();
		main.RunOneStep();
		main.UpdateGUI();
	}

	return 0;
}
