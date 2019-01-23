#include "fstream"
#include <string.h>
#include "fm77avkeyboardemu.h"

void FM77AVKeyboardEmulator::StartAutoTyping(const char fName[],int lineBreakWait)
{
	if(nullptr==fName || 0==fName[0])
	{
		StopAutoTyping();
	}
	else
	{
		FILE *fp=fopen(fName,"rb");
		if(nullptr!=fp)
		{
			autoTypingFName=fName;
			autoTypingPtr=0;

			fseek(fp,0,SEEK_END);
			auto fSize=ftell(fp);
			fseek(fp,0,SEEK_SET);
			autoTypingTxt.resize(fSize);
			fread(autoTypingTxt.data(),1,fSize,fp);

			autoTypingTxt.push_back(0);

			autoTypingLineBreakWait=lineBreakWait;

			nextAutoTypingTimer=std::chrono::system_clock::now();
			fclose(fp);
		}
		else
		{
			StopAutoTyping();
		}
	}
}
void FM77AVKeyboardEmulator::StartAutoTyping(const std::vector <char> text,int lineBreakWait)
{
	autoTypingFName="Clipboard";
	autoTypingPtr=0;

	autoTypingTxt.clear();
	autoTypingTxt.insert(autoTypingTxt.end(),text.begin(),text.end());
	autoTypingTxt.push_back(0);

	autoTypingLineBreakWait=lineBreakWait;

	nextAutoTypingTimer=std::chrono::system_clock::now();
}
void FM77AVKeyboardEmulator::StopAutoTyping(void)
{
	autoTypingFName="";
	autoTypingPtr=0;
	autoTypingLineBreakWait=0;
	autoTypingTxt.clear();
	nextAutoTypingTimer=std::chrono::system_clock::now();
}
bool FM77AVKeyboardEmulator::IsAutoTyping(void) const
{
	return (autoTypingPtr<autoTypingTxt.size());
}
std::string FM77AVKeyboardEmulator::GetAutoTypingFileName(void) const
{
	return autoTypingFName;
}

void FM77AVKeyboardEmulator::AutoType(void)
{
	auto dt=std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::system_clock::now()-nextAutoTypingTimer).count();
	if(dt<50)
	{
		return;
	}

	nextAutoTypingTimer=std::chrono::system_clock::now();
	if(autoTypingPtr<autoTypingTxt.size() && true==keySched.keyEventQueue.empty())
	{
		auto nextLetter=autoTypingTxt[autoTypingPtr];
		int nConsumed=0;

		if(0==nConsumed && 0x0d==nextLetter)
		{
			keySched.AddStroke(AVKEY_RETURN,false,false,false);
			nextAutoTypingTimer+=std::chrono::milliseconds(autoTypingLineBreakWait);
			nConsumed=1;
			if(0x0a==autoTypingTxt[autoTypingPtr+1])
			{
				++nConsumed;
			}
		}

		if(0==nConsumed && 0==strncmp("\\\\\\WAIT",(const char *)autoTypingTxt.data()+autoTypingPtr,7))
		{
			// Just wait
			nConsumed=7;
		}

		if(0==nConsumed && '\\'==nextLetter && '\\'==autoTypingTxt[autoTypingPtr+1] && '\\'==autoTypingTxt[autoTypingPtr+2])
		{
			auto code=FM77AVKeyLabelToKeyCode((const char *)autoTypingTxt.data()+autoTypingPtr+3);
			if(0!=code)
			{
				keySched.AddStroke(code,false,false,false);
				for(nConsumed=3; autoTypingPtr+nConsumed<autoTypingTxt.size(); ++nConsumed)
				{
					if(0==autoTypingTxt[autoTypingPtr+nConsumed] ||
					   '\\'==autoTypingTxt[autoTypingPtr+nConsumed] ||
					   0x0d==autoTypingTxt[autoTypingPtr+nConsumed] ||
					   0x0a==autoTypingTxt[autoTypingPtr+nConsumed])
					{
						break;
					}
				}
			}
		}

		if(0==nConsumed && 0!=nextLetter)
		{
			auto comb=FM77AVTranslateCharToCode(nextLetter);
			if(AVKEY_NULL!=comb.keyCode)
			{
				keySched.AddStroke(comb.keyCode,comb.shift,comb.ctrl,comb.graph);
			}
			++nConsumed;
		}

		autoTypingPtr+=nConsumed;
		if(0==nConsumed)
		{
			++autoTypingPtr;
		}
	}

	if(autoTypingTxt.size()<=autoTypingPtr)
	{
		StopAutoTyping();
	}
}
