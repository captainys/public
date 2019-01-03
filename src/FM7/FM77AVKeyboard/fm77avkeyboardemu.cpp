#include <fssimplewindow.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

#include "fm77avkeyboardemu.h"
#include "cheaputil.h"
#include "gui.h"

/*
What's found so far.

- Firmware upgrade cannot be done from Windows 10.  In Windows 7, firmware was updated, but the Windows 7 PC stopped recognizing IRToy.
  I almost lost hope, but it revived again when I plugged it to Windows 10 PC.  Then it is working like a charm.

- Hand-shake mode is a false advertisement.  It doesn't happen even after upgrading to firmware v22.

- Once transmission command 0x03 is sent, IRToy enters transmission mode, it accepts nothing else until two bytes: 0xff 0xff are sent.  If the program is terminated before sending 0xff 0xff, IRToy no longer can be connected via COM port.  It needs to be unplugged and plugged again.

- Baud rate concern: FM77AV keyboard requires minimum 1,280,000 bps.  Control Panel can go up to only 128,000.  Win32 API seems to allow 1,600,000 bps.  The question is if it is real.

- When multiple keys are pressed, a code is still sent every 20ms, but looks like it is just the keycode of the last-pressed key.  Therefore, in every 20ms, only one keycode seems to be sent.

- IRToy has a bad design that as soon as it received '$', it enters the boot-loader mode.
  There is no get out from the mode even when I unplugged and plug it back, unless I flash the firmware.
  And, Windows 10 cannot do it.  Windows 7 can succeed, but there are reports of failure.  (Worked for me luckily).

  What's even worse?  A command for enabling sent-byte-count is '$'.  If the unit is somehow reset and
  accidentally this code is sent, IRToy is bricked.

  Also, if I use 0x00 time step (0.17us time step), the pulse for 4000us is like 23000 ticks.
  The higher-byte may be greater than '$'.  If I increment or decremnt the higher byte, it makes
  a large difference in pulse.  If I go down to 0.67us time step, only the lower byte can go
  larger than '$' then I can make a small adjustment, hopefully FM77AV doesn't care.

  Turned out FM77AV cares 0.67us difference.  Really???  Definitely 0.67us pulse drops key strokes far more often than 0.17us pulse.

*/


////////////////////////////////////////////////////////////


FM77AVKeyboardScheduler::FM77AVKeyboardScheduler()
{
	CleanUp();
}

void FM77AVKeyboardScheduler::CleanUp(void)
{
	keyHeldDown.clear();

	std::queue <FM77AVKeyEvent> zeroQueue;
	std::swap(keyEventQueue,zeroQueue);

	repeatTimer=std::chrono::system_clock::now();
}

void FM77AVKeyboardScheduler::AddKeyEvent(bool pressEvent,int keyCode,bool shift,bool ctrl,bool graph)
{
	if(true==pressEvent)
	{
		if(true==shift)
		{
			PressKey(AVKEY_LEFT_SHIFT);
		}
		if(true==ctrl)
		{
			PressKey(AVKEY_CTRL);
		}
		if(true==graph)
		{
			PressKey(AVKEY_GRAPH);
		}
		PressKey(keyCode);
	}
	else
	{
		ReleaseKey(keyCode);
		if(true==shift)
		{
			ReleaseKey(AVKEY_LEFT_SHIFT);
		}
		if(true==ctrl)
		{
			ReleaseKey(AVKEY_CTRL);
		}
		if(true==graph)
		{
			ReleaseKey(AVKEY_GRAPH);
		}
	}
}

void FM77AVKeyboardScheduler::AddKeyEvent(bool pressEvent,int keyCode)
{
	keyEventQueue.push(FM77AVKeyEvent(pressEvent,keyCode));
}

void FM77AVKeyboardScheduler::PressKey(int keyCode)
{
	if(true!=IsKeyAlreadyHeldDown(keyCode))
	{
		keyEventQueue.push(FM77AVKeyEvent(true,keyCode));
		keyHeldDown.push_back(keyCode);
	}
}
void FM77AVKeyboardScheduler::ReleaseKey(int keyCode)
{
	keyEventQueue.push(FM77AVKeyEvent(false,keyCode));
	for(auto iter=keyHeldDown.begin(); iter!=keyHeldDown.end(); ++iter)
	{
		if(*iter==keyCode)
		{
			keyHeldDown.erase(iter);
			break;
		}
	}
}

bool FM77AVKeyboardScheduler::IsKeyAlreadyHeldDown(int keyCode) const
{
	for(auto c : keyHeldDown)
	{
		if(c==keyCode)
		{
			return true;
		}
	}
	return false;
}


void FM77AVKeyboardScheduler::AddStroke(int keyCode,bool shift,bool ctrl,bool graph)
{
	AddKeyEvent(true,keyCode,shift,ctrl,graph);
	AddKeyEvent(false,keyCode,shift,ctrl,graph);
}

void FM77AVKeyboardScheduler::Flush(IRToy_Controller &irToy)
{
	FM77AVKeyEvent toSend;
	toSend.keyCode=AVKEY_NULL;
	if(true!=keyEventQueue.empty())
	{
		toSend=keyEventQueue.front();
		keyEventQueue.pop();
	}
	else if(0<keyHeldDown.size())
	{
		toSend.keyCode=keyHeldDown.back();
		toSend.pressEvent=true;
	}

	if(AVKEY_NULL!=toSend.keyCode)
	{
		auto code30=FM77AVMake30BitPattern(toSend);
		if(0<code30.size())
		{
			char code40[41];
			FM77AV30bitTo40bit(code40,code30.c_str());
			irToy.Make100usPulse(code40);
			irToy.StartTransmit();
		}
		repeatTimer=std::chrono::system_clock::now();
	}
}

int FM77AVKeyboardScheduler::MilliSecondSinceLastFlush(void) const
{
	auto now=std::chrono::system_clock::now();
	return std::chrono::duration_cast <std::chrono::milliseconds> (now-repeatTimer).count();
}

////////////////////////////////////////////////////////////

FM77AVKeyboardEmulator::FM77AVKeyboardEmulator()
{
	Initialize();
}

FM77AVKeyboardEmulator::~FM77AVKeyboardEmulator()
{
	Disconnect();
}

void FM77AVKeyboardEmulator::Connect(int port)
{
	Disconnect();
	Initialize();
	if(true!=irToy.Connect(port))
	{
		mode=MODE_ERROR;
	}
}

void FM77AVKeyboardEmulator::Disconnect(void)
{
	irToy.Disconnect();
}

void FM77AVKeyboardEmulator::SetIRToyNotFoundError(void)
{
	irToy.SetIRToyNotFoundError();
}

void FM77AVKeyboardEmulator::StartCloseIRToySequence(void)
{
	irToy.StartCloseIRToySequence();
}

void FM77AVKeyboardEmulator::SetIRToyHoldStateForDebug(int stateForHold)
{
	irToy.SetHoldStateForDebug(stateForHold);
}

void FM77AVKeyboardEmulator::SetMode(int nextMode)
{
	mode=nextMode;
}

int FM77AVKeyboardEmulator::GetMode(void) const
{
	return mode;
}

void FM77AVKeyboardEmulator::SetPause(bool pause)
{
	this->pause=pause;
}
bool FM77AVKeyboardEmulator::GetPause(void) const
{
	return this->pause;
}

void FM77AVKeyboardEmulator::SetAutoStop(bool autoStop)
{
	this->autoStop=autoStop;
}
bool FM77AVKeyboardEmulator::GetAutoStop(void) const
{
	return this->autoStop;
}

void FM77AVKeyboardEmulator::SetAutoFire(bool autoFire)
{
	this->autoFire=autoFire;
}
bool FM77AVKeyboardEmulator::GetAutoFire(void) const
{
	return this->autoFire;
}

void FM77AVKeyboardEmulator::SetArrowFor8Dir(bool arrowFor8Dir)
{
	this->arrowFor8Dir=arrowFor8Dir;
}
bool FM77AVKeyboardEmulator::GetArrowFor8Dir(void) const
{
	return this->arrowFor8Dir;
}

void FM77AVKeyboardEmulator::Initialize(void)
{
	mode=MODE_TRANSLATION;
	autoStop=false;
	autoFire=false;
	arrowFor8Dir=false;
	pause=false;
	nextDotTimer=std::chrono::system_clock::now();
	breakKeyState=false;

	nRKanaBufUsed=0;
	rKanaBuf[0]=0;

	for(auto &b : keyState)
	{
		b=false;
	}

	keySched.CleanUp();
	irToy.Reset();
}

void FM77AVKeyboardEmulator::ProcessChar(char c)
{
	if(true==pause)
	{
		return;
	}

	if(MODE_TRANSLATION==mode)
	{
		auto comb=FM77AVTranslateCharToCode(c);
		if(AVKEY_NULL!=comb.keyCode)
		{
			keySched.AddStroke(comb.keyCode,comb.shift,comb.ctrl,comb.graph);
		}
	}
	else if(MODE_RKANA==mode)
	{
		if(('a'<=c && c<='z') || ('A'<=c && c<='Z'))
		{
			if('A'<=c && c<='Z')
			{
				c=c+'a'-'A';
			}

			if(3>nRKanaBufUsed)
			{
				rKanaBuf[nRKanaBufUsed++]=c;
				rKanaBuf[nRKanaBufUsed  ]=0;
			}

			auto match=FM77AVFindKeyCombinationForRomaji(rKanaBuf);
			if(AVKEY_NULL!=match.comb[0].keyCode)
			{
				for(auto &comb : match.comb)
				{
					if(AVKEY_NULL!=comb.keyCode)
					{
						keySched.AddStroke(comb.keyCode,comb.shift,comb.ctrl,comb.graph);
					}
				}

				if(true==match.carryOver)
				{
					auto lastChar=rKanaBuf[nRKanaBufUsed-1];
					rKanaBuf[0]=lastChar;
					rKanaBuf[1]=0;
					nRKanaBufUsed=1;
				}
				else
				{
					nRKanaBufUsed=0;
					rKanaBuf[0]=0;
				}
			}

			if(3==nRKanaBufUsed)
			{
				nRKanaBufUsed=0;
				rKanaBuf[0]=0;
			}
		}
		else
		{
			auto comb=FM77AVTranslateCharToCode(c);
			if(AVKEY_NULL!=comb.keyCode)
			{
				keySched.AddStroke(comb.keyCode,comb.shift,comb.ctrl,comb.graph);
			}
		}
	}
}
void FM77AVKeyboardEmulator::ProcessKeyStroke(int key,bool shift,bool ctrl,bool graph)
{
	if(true==pause)
	{
		return;
	}

	if(FSKEY_NULL==key)
	{
		return;
	}

	if(MODE_RKANA==mode && 0<nRKanaBufUsed)
	{
		if(FSKEY_BS==key)
		{
			--nRKanaBufUsed;
			rKanaBuf[nRKanaBufUsed]=0;
			return;
		}
	}

	if(MODE_TRANSLATION==mode || MODE_RKANA==mode)
	{
		// Note: Don't add shift and ctrl unless the key code is processed in this function.
		if(FSKEY_BS==key)
		{
			keySched.AddStroke(AVKEY_BACKSPACE,shift,ctrl,graph);
		}
		else if(FSKEY_INS==key)
		{
			keySched.AddStroke(AVKEY_INS,shift,ctrl,graph);
		}
		else if(FSKEY_DEL==key)
		{
			keySched.AddStroke(AVKEY_DEL,shift,ctrl,graph);
		}
		else if(FSKEY_HOME==key)
		{
			keySched.AddStroke(AVKEY_HOME,shift,ctrl,graph);
		}
		else if(FSKEY_ENTER==key)
		{
			keySched.AddStroke(AVKEY_RETURN,shift,ctrl,graph);
		}
		else if(FSKEY_F1==key)
		{
			keySched.AddStroke(AVKEY_PF1,shift,ctrl,graph);
		}
		else if(FSKEY_F2==key)
		{
			keySched.AddStroke(AVKEY_PF2,shift,ctrl,graph);
		}
		else if(FSKEY_F3==key)
		{
			keySched.AddStroke(AVKEY_PF3,shift,ctrl,graph);
		}
		else if(FSKEY_F4==key)
		{
			keySched.AddStroke(AVKEY_PF4,shift,ctrl,graph);
		}
		else if(FSKEY_F5==key)
		{
			keySched.AddStroke(AVKEY_PF5,shift,ctrl,graph);
		}
		else if(FSKEY_F6==key)
		{
			keySched.AddStroke(AVKEY_PF6,shift,ctrl,graph);
		}
		else if(FSKEY_F7==key)
		{
			keySched.AddStroke(AVKEY_PF7,shift,ctrl,graph);
		}
		else if(FSKEY_F8==key)
		{
			keySched.AddStroke(AVKEY_PF8,shift,ctrl,graph);
		}
		else if(FSKEY_F9==key)
		{
			keySched.AddStroke(AVKEY_PF9,shift,ctrl,graph);
		}
		else if(FSKEY_F10==key)
		{
			keySched.AddStroke(AVKEY_PF10,shift,ctrl,graph);
		}

		if(FSKEY_LEFT==key)
		{
			keySched.AddStroke(AVKEY_LEFT,shift,ctrl,graph);
		}
		if(FSKEY_RIGHT==key)
		{
			keySched.AddStroke(AVKEY_RIGHT,shift,ctrl,graph);
		}
		if(FSKEY_UP==key)
		{
			keySched.AddStroke(AVKEY_UP,shift,ctrl,graph);
		}
		if(FSKEY_DOWN==key)
		{
			keySched.AddStroke(AVKEY_DOWN,shift,ctrl,graph);
		}
	}
}

void FM77AVKeyboardEmulator::ProcessKeyState(void)
{
	if(true==pause)
	{
		return;
	}

	bool nextKeyState[AVKEY_NUM_KEYCODE];
	for(auto &b : nextKeyState)
	{
		b=false;
	}
	for(int fsKey=1; fsKey<FSKEY_NUM_KEYCODE; ++fsKey)
	{
		auto avKey=keyMap.map[fsKey];
		if(AVKEY_NULL!=avKey && 0!=FsGetKeyState(fsKey))
		{
			nextKeyState[avKey]=true;
		}
	}
	if(true==arrowFor8Dir)
	{
		nextKeyState[AVKEY_UP]=false;
		nextKeyState[AVKEY_DOWN]=false;
		nextKeyState[AVKEY_LEFT]=false;
		nextKeyState[AVKEY_RIGHT]=false;
		nextKeyState[AVKEY_NUM_1]=false;
		nextKeyState[AVKEY_NUM_2]=false;
		nextKeyState[AVKEY_NUM_3]=false;
		nextKeyState[AVKEY_NUM_4]=false;
		nextKeyState[AVKEY_NUM_6]=false;
		nextKeyState[AVKEY_NUM_7]=false;
		nextKeyState[AVKEY_NUM_8]=false;
		nextKeyState[AVKEY_NUM_9]=false;
		if(0!=FsGetKeyState(FSKEY_LEFT) && 0!=FsGetKeyState(FSKEY_UP))
		{
			nextKeyState[AVKEY_NUM_7]=true;
		}
		else if(0!=FsGetKeyState(FSKEY_RIGHT) && 0!=FsGetKeyState(FSKEY_UP))
		{
			nextKeyState[AVKEY_NUM_9]=true;
		}
		else if(0!=FsGetKeyState(FSKEY_LEFT) && 0!=FsGetKeyState(FSKEY_DOWN))
		{
			nextKeyState[AVKEY_NUM_1]=true;
		}
		else if(0!=FsGetKeyState(FSKEY_RIGHT) && 0!=FsGetKeyState(FSKEY_DOWN))
		{
			nextKeyState[AVKEY_NUM_3]=true;
		}
		else if(0!=FsGetKeyState(FSKEY_LEFT))
		{
			nextKeyState[AVKEY_NUM_4]=true;
		}
		else if(0!=FsGetKeyState(FSKEY_RIGHT))
		{
			nextKeyState[AVKEY_NUM_6]=true;
		}
		else if(0!=FsGetKeyState(FSKEY_UP))
		{
			nextKeyState[AVKEY_NUM_8]=true;
		}
		else if(0!=FsGetKeyState(FSKEY_DOWN))
		{
			nextKeyState[AVKEY_NUM_2]=true;
		}
	}


	if(MODE_TRANSLATION==mode)
	{
		if(true!=breakKeyState && 0!=FsGetKeyState(FSKEY_ESC))
		{
			keySched.PressKey(AVKEY_BREAK);
			breakKeyState=true;
		}
		else if(true==breakKeyState && 0==FsGetKeyState(FSKEY_ESC))
		{
			keySched.ReleaseKey(AVKEY_BREAK);
			breakKeyState=false;
		}
	}
	else if(MODE_DIRECT==mode)
	{
		for(int avKey=1; avKey<AVKEY_NUM_KEYCODE; ++avKey)
		{
			if(true==keyState[avKey] && true!=nextKeyState[avKey])
			{
				keySched.ReleaseKey(avKey);
			}
			if(true!=keyState[avKey] && true==nextKeyState[avKey])
			{
				keySched.PressKey(avKey);
			}
		}
	}


	if(true==autoStop)
	{
		if((true==keyState[AVKEY_NUM_1] ||
		    true==keyState[AVKEY_NUM_2] ||
		    true==keyState[AVKEY_NUM_3] ||
		    true==keyState[AVKEY_NUM_4] ||
		    true==keyState[AVKEY_NUM_6] ||
		    true==keyState[AVKEY_NUM_7] ||
		    true==keyState[AVKEY_NUM_8] ||
		    true==keyState[AVKEY_NUM_9]) &&
		   (true!=nextKeyState[AVKEY_NUM_1] &&
		    true!=nextKeyState[AVKEY_NUM_2] &&
		    true!=nextKeyState[AVKEY_NUM_3] &&
		    true!=nextKeyState[AVKEY_NUM_4] &&
		    true!=nextKeyState[AVKEY_NUM_6] &&
		    true!=nextKeyState[AVKEY_NUM_7] &&
		    true!=nextKeyState[AVKEY_NUM_8] &&
		    true!=nextKeyState[AVKEY_NUM_9]))
		{
			keySched.AddStroke(AVKEY_NUM_5,false,false,false);
		}
	}

	if(true==autoFire && true==keySched.keyEventQueue.empty())
	{
		for(int avKey=1; avKey<AVKEY_NUM_KEYCODE; ++avKey)
		{
			if(true==keyState[avKey])
			{
				keySched.AddStroke(avKey,false,false,false);
			}
		}
	}

	for(int i=0; i<AVKEY_NUM_KEYCODE; ++i)
	{
		keyState[i]=nextKeyState[i];
	}
}

void FM77AVKeyboardEmulator::SendStroke(int keyCode,bool shift,bool ctrl,bool graph)
{
	if(true==pause)
	{
		return;
	}

	keySched.AddStroke(keyCode,shift,ctrl,graph);
}

void FM77AVKeyboardEmulator::SendFM77AVSecretMessage(void)
{
	if(true==pause)
	{
		return;
	}

	// CAPS & KANA on
	keySched.AddKeyEvent(true,AVKEY_CAPS,false,false,false);
	keySched.AddKeyEvent(true,AVKEY_KANA,false,false,false);
	keySched.AddKeyEvent(false,AVKEY_CAPS,false,false,false);
	keySched.AddKeyEvent(false,AVKEY_KANA,false,false,false);

	// Press RIGHT & LEFT SHIFT
	keySched.AddKeyEvent(true,AVKEY_RIGHT_SHIFT,false,false,false);
	keySched.AddKeyEvent(true,AVKEY_LEFT_SHIFT,false,false,false);
	// CTRL+SHIFT(redundant)+T (Hold for at least 80 ms to be sure)
	keySched.AddKeyEvent(true,AVKEY_T,true,true,true);
	keySched.AddKeyEvent(true,AVKEY_T,true,true,true);
	keySched.AddKeyEvent(true,AVKEY_T,true,true,true);
	keySched.AddKeyEvent(true,AVKEY_T,true,true,true);

	// Release T
	keySched.AddKeyEvent(false,AVKEY_T,true,true,true);
	keySched.AddKeyEvent(false,AVKEY_RIGHT_SHIFT,false,false,false);
	keySched.AddKeyEvent(false,AVKEY_LEFT_SHIFT,false,false,false);

	// CAPS & KANA off
	keySched.AddKeyEvent(true,AVKEY_CAPS,false,false,false);
	keySched.AddKeyEvent(true,AVKEY_KANA,false,false,false);
	keySched.AddKeyEvent(false,AVKEY_CAPS,false,false,false);
	keySched.AddKeyEvent(false,AVKEY_KANA,false,false,false);
}

void FM77AVKeyboardEmulator::RunOneStep(void)
{
	if(true==IsAutoTyping())
	{
		AutoType();
	}

	if(true!=keySched.keyEventQueue.empty() && 10<=keySched.MilliSecondSinceLastFlush())
	{
		keySched.Flush(irToy);
	}
	else if(true==keySched.keyEventQueue.empty() && true!=keySched.keyHeldDown.empty() && 18<=keySched.MilliSecondSinceLastFlush())
	{
		// May be 20 is good enough.  Re-send must be within 20 milli seconds.
		keySched.Flush(irToy);
	}
	irToy.RunOneStep();

	auto now=std::chrono::system_clock::now();
	int passed=std::chrono::duration_cast <std::chrono::milliseconds> (now-nextDotTimer).count();
	if(1000<passed)
	{
		nextDotTimer=now;
		printf(".\n");
	}
}

int FM77AVKeyboardEmulator::GetIRToyState(void) const
{
	return irToy.GetState();
}
int FM77AVKeyboardEmulator::GetIRToyErrorCode(void) const
{
	return irToy.GetErrorCode();
}

std::string FM77AVKeyboardEmulator::GetRKanaBuf(void) const
{
	std::string rkBuf=rKanaBuf;
	return rkBuf;
}

std::string FM77AVKeyboardEmulator::GetIRToyVersion(void) const
{
	return irToy.GetIRToyVersion();
}
std::string FM77AVKeyboardEmulator::GetIRToyProtocolVersion(void) const
{
	return irToy.GetProtocolVersion();
}

std::vector <unsigned char> FM77AVKeyboardEmulator::GetIRToyCOMTransactionLog(void) const
{
	return irToy.GetLog();
}

////////////////////////////////////////////////////////////

