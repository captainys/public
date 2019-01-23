#ifndef FM77AVKEYBOARDEMU_IS_INCLUDED
#define FM77AVKEYBOARDEMU_IS_INCLUDED
/* { */

#include <chrono>
#include <vector>
#include <string>

#include "irtoy.h"
#include "fm77avkey.h"
#include "fm77avkeymap.h"
#include "fm77avrkana.h"

class FM77AVKeyboardScheduler
{
public:
	std::chrono::time_point <std::chrono::system_clock> repeatTimer;


	// Looks like what needs to be done is:
	// - If a key code is in the queue (press or release event), send it as soon as possible.
	//   Probably, 8 to 10ms interval should be good enough.
	// - If nothing is in the queue, but some keys are still held down, send the last-pressed
	//   key code every 20ms, actually within 20ms.
	// Key scheduler needs to know:
	//   Key-Event Queue.
	//   What keys are held down.

	std::queue <FM77AVKeyEvent> keyEventQueue;
	std::vector <int> keyHeldDown;



	FM77AVKeyboardScheduler();

	void CleanUp(void);

	void AddKeyEvent(bool pressEvent,int keyCode,bool shift,bool ctrl,bool graph);
	void AddKeyEvent(bool pressEvent,int keyCode);
	void PressKey(int keyCode);
	void ReleaseKey(int keyCode);
	bool IsKeyAlreadyHeldDown(int keyCode) const;

	void AddStroke(int keyCode,bool shift,bool ctrl,bool graph);

	void Flush(IRToy_Controller &irToy);
	int MilliSecondSinceLastFlush(void) const;
};

class FM77AVKeyboardEmulator
{
public:
	enum
	{
		MODE_NULL,
		MODE_TRANSLATION,
		MODE_DIRECT,
		MODE_RKANA,
		MODE_ERROR
	};
	int mode;
	bool pause;
	std::string errorMessage;

private:
	IRToy_Controller irToy;

public:
	FM77AVKeyMap keyMap;

private:
	bool keyState[AVKEY_NUM_KEYCODE];
	int nRKanaBufUsed;
	char rKanaBuf[4];
	bool autoStop,autoFire;
	bool arrowFor8Dir;

	FM77AVKeyboardScheduler keySched;
	std::chrono::time_point <std::chrono::system_clock> nextDotTimer;
	bool breakKeyState;

	std::string autoTypingFName;
	long long int autoTypingPtr;
	long long int autoTypingLineBreakWait;
	std::vector <unsigned char> autoTypingTxt;
	std::chrono::time_point <std::chrono::system_clock> nextAutoTypingTimer;
public:
	FM77AVKeyboardEmulator();
	~FM77AVKeyboardEmulator();

	void Connect(int port);
	void Disconnect(void);
	void SetIRToyNotFoundError(void);
	void StartCloseIRToySequence(void);

	/*! For debugging purpose, it will prevent IRToy to get out of the specified state.
	*/
	void SetIRToyHoldStateForDebug(int stateForHold);

	void SetMode(int nextMode);
	int GetMode(void) const;

	void SetPause(bool pause);
	bool GetPause(void) const;

	void SetAutoStop(bool autoStop);
	bool GetAutoStop(void) const;

	void SetAutoFire(bool autoStop);
	bool GetAutoFire(void) const;

	void SetArrowFor8Dir(bool arrowFor8Dir);
	bool GetArrowFor8Dir(void) const;

	void StartAutoTyping(const char fName[],int lineBreakWait);
	void StartAutoTyping(const std::vector <char> text,int lineBreakWait);
	void StopAutoTyping(void);
	bool IsAutoTyping(void) const;
	std::string GetAutoTypingFileName(void) const;

private:
	void AutoType(void);

private:
	void Initialize(void);

public:
	void ProcessChar(char c);
	void ProcessKeyStroke(int fsKey,bool shift,bool ctrl,bool graph);
	void ProcessKeyState(void);

public:
	void SendFM77AVSecretMessage(void);
	void SendStroke(int keyCode,bool shift,bool ctrl,bool graph);

	void RunOneStep(void);

	int GetIRToyState(void) const;
	int GetIRToyErrorCode(void) const;
	std::string GetRKanaBuf(void) const;
	std::string GetIRToyVersion(void) const;
	std::string GetIRToyProtocolVersion(void) const;

	/*! Get COM Transaction Log for debugging purposes.
	    It is possible to recover what you typed from the transaction log,
	    if someone decides to write a decoder.
	    But, I don't bother doing it.  I'm more interested in what commands
	    are sent and what IRToy responded.
	    And, you are not supposed to type your sensitive password in FM77AV,
	    unless somehow you fabricated your own Ethernet adapter for FM77AV.
	    So, you should be fine.  And, unless you save it to outside, it will
	    disappear when you close your program.
	*/
	std::vector <unsigned char> GetIRToyCOMTransactionLog(void) const;
};


/* } */
#endif
