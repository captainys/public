#include "irtoy.h"
#include "cheaputil.h"



void IRToy_Controller::AddLog(unsigned char d1,unsigned char d2)
{
	log.push(d1);
	log.push(d2);
	while(65536<log.size())
	{
		log.pop();
		log.pop();
	}
}
std::vector <unsigned char> IRToy_Controller::GetLog(void) const
{
	auto logCopy=log;
	std::vector <unsigned char> logSerial;
	while(true!=logCopy.empty())
	{
		logSerial.push_back(logCopy.front());
		logCopy.pop();
	}
	return logSerial;
}

void IRToy_Controller::Send(long long int nByte,const unsigned char dat[])
{
	for(int i=0; i<nByte; ++i)
	{
		if(dat[i]=='$')
		{
			printf("Danger! '$' detected!  Not going to send it!\n");
			return;
		}
	}

	if(true==comPort.IsConnected())
	{
		auto byteWritten=comPort.Send(nByte,dat);
		for(int i=0; i<byteWritten; ++i)
		{
			AddLog('S',dat[i]);
		}
		if(byteWritten<nByte)
		{
			ChangeState(STATE_ERROR);
			errorCode=ERROR_PACKET_SEND_FAILURE;
		}
		printf("Sent %d bytes\n",byteWritten);
	}
}
std::vector <unsigned char> IRToy_Controller::Read(void)
{
	if(true==comPort.IsConnected())
	{
		auto recv=comPort.Receive();
		for(auto r : recv)
		{
			AddLog('R',r);
		}
		return recv;
	}
	std::vector <unsigned char> empty;
	return empty;
}

void IRToy_Controller::ChangeState(int nextState)
{
	if(holdStateForDebug==state)
	{
		return;
	}

	if(STATE_ERROR!=state || STATE_INITIALIZING==nextState || STATE_DISCONNECT==nextState)
	{
		state=nextState;
		stateCounter=0;
		ResetStateTimer();
		stateCommandSent=false;
		nAccumByte=0;
		feedBackBuf.clear();
	}
}

void IRToy_Controller::MakeTerminator(std::vector <unsigned char> &dat)
{
	if(0!=dat.size()%2)
	{
		dat.pop_back();
	}

	// Just in case
	if(0==dat.size() ||
	   dat.back()!=0xff ||
	   dat[dat.size()-2]!=0xff)
	{
		dat.push_back(0xff);
		dat.push_back(0xff);
	}
}

/* static */ void IRToy_Controller::PrintRecording(const std::vector <unsigned char> &dat)
{
	int totalCount=0;
	for(int i=0; i+2<=dat.size(); i+=2)
	{
		int tick=(dat[i]*256+dat[i+1]);
		if(65535!=tick)
		{
			totalCount+=tick;
		}
		printf("%5d ",tick);
		if(i%24==22)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("%d counts\n",totalCount);
}

/* static */ void IRToy_Controller::ShowBuf(long long int nByte,const unsigned char dat[])
{
	if(0<nByte)
	{
		for(int i=0; i<nByte; ++i)
		{
			printf("%02x",dat[i]);
			printf("(%d)",dat[i]);
			if(' '<=dat[i] && dat[i]<128)
			{
				printf("\'%c\'",dat[i]);
			}
		}
		printf("\n");
	}
}

/* static */ void IRToy_Controller::CleanRecording(std::vector <unsigned char> &dat)
{
	if(0!=(dat.size()%2))
	{
		dat.pop_back();
	}

	for(int i=0; i+2<=dat.size(); i+=2)
	{
		if(dat[i]==0xff && dat[i+1]==0xff && i+2!=dat.size())
		{
			dat.resize(i+2);
			return;
		}
	}

	if(dat[dat.size()-2]!=0xff || dat[dat.size()-1]!=0xff)
	{
		dat.push_back(0xff);
		dat.push_back(0xff);
	}
}

IRToy_Controller::IRToy_Controller()
{
	holdStateForDebug=STATE_NULL;
	Reset();
}

bool IRToy_Controller::Connect(int portNumber)
{
	Disconnect();
	Reset();
	if(true==comPort.Open(portNumber))
	{
		AddLog('P',portNumber);

		ChangeState(STATE_INITIALIZING);
		auto dummyRead=comPort.Receive();
		printf("Flushed %d bytes readings.\n",(int)dummyRead.size());
		return true;
	}
	return false;
}
bool IRToy_Controller::IsConnected(void) const
{
	return comPort.IsConnected();
}
int IRToy_Controller::GetPort(void) const
{
	return comPort.GetPort();
}
void IRToy_Controller::Disconnect()
{
	comPort.Close();
	AddLog('D','D');
}

void IRToy_Controller::Reset(void)
{
	state=STATE_DISCONNECT;
	stateCommandSent=false;
	errorCode=ERROR_NOERROR;
	nAccumByte=0;
	stateTimer0=std::chrono::system_clock::now();
	recording.clear();
	comPort.Close();
	for(auto &c : irToyVersion)
	{
		c=0;
	}
	for(auto &c : protocolVersion)
	{
		c=0;
	}
}

void IRToy_Controller::SetIRToyNotFoundError(void)
{
	ChangeState(STATE_ERROR);
	errorCode=ERROR_IRTOY_NOT_FOUND;
}
void IRToy_Controller::SetHoldStateForDebug(int state)
{
	holdStateForDebug=state;
}

void IRToy_Controller::StartCloseIRToySequence(void)
{
	ChangeState(STATE_CLOSING);
}

void IRToy_Controller::ResetStateTimer(void)
{
	stateTimer0=std::chrono::system_clock::now();
}
long long int IRToy_Controller::StateTimer(void) const
{
	auto now=std::chrono::system_clock::now();
	return std::chrono::duration_cast <std::chrono::milliseconds> (now-stateTimer0).count();
}

void IRToy_Controller::State_Initializing(void)
{
	if(true!=stateCommandSent)
	{
		const unsigned char cmd[5]={CMD_RESET,CMD_RESET,CMD_RESET,CMD_RESET,CMD_RESET};
		Send(5,cmd);
		ResetStateTimer();
		errorCode=ERROR_NOERROR;
		stateCommandSent=true;
	}
	else if(300<StateTimer())
	{
		ChangeState(STATE_HANDSHAKE);
	}
}
void IRToy_Controller::State_Handshake(void)
{
	if(true!=stateCommandSent)
	{
		const unsigned char cmd[1]={'R'};
		Send(1,cmd);
		ResetStateTimer();
		errorCode=ERROR_NOERROR;
		stateCommandSent=true;
	}

	auto readBuf=Read();
	if(0<readBuf.size())
	{
		if(0<feedBackBuf.size() && feedBackBuf.back()=='O' && readBuf[0]=='K')
		{
			ChangeState(STATE_SELFTEST);
		}
		std::swap(feedBackBuf,readBuf);
		for(int i=0; i+1<feedBackBuf.size(); ++i)
		{
			if('O'==feedBackBuf[i] && 'K'==feedBackBuf[i+1])
			{
				ChangeState(STATE_SELFTEST);
			}
		}
	}
	if(1000<StateTimer())
	{
		ChangeState(STATE_ERROR);
		errorCode=ERROR_INITIALIZATION_TIMEOUT;
	}
}
void IRToy_Controller::State_SelfTest(void)
{
	if(true!=stateCommandSent)
	{
		const unsigned char cmd[1]={'V'};
		Send(1,cmd);
		stateCommandSent=true;
		ResetStateTimer();
	}

	auto readBuf=Read();
	if(0<readBuf.size())
	{
		ShowBuf(readBuf.size(),readBuf.data());
		for(auto b : readBuf)
		{
			feedBackBuf.push_back(b);
		}
		if(4<=feedBackBuf.size())
		{
			for(int i=0; i+3<feedBackBuf.size(); ++i)
			{
				if('V'==feedBackBuf[i] &&
				   '0'<=feedBackBuf[i+1] && feedBackBuf[i+1]<='9' &&
				   '0'<=feedBackBuf[i+2] && feedBackBuf[i+2]<='9' &&
				   '0'<=feedBackBuf[i+3] && feedBackBuf[i+3]<='9')
				{
					irToyVersion[0]=feedBackBuf[i];
					irToyVersion[1]=feedBackBuf[i+1];
					irToyVersion[2]=feedBackBuf[i+2];
					irToyVersion[3]=feedBackBuf[i+3];
					irToyVersion[4]=0;
					ChangeState(STATE_SETSAMPLERMODE);
				}
			}
		}
	}
	if(1000<StateTimer())
	{
		ChangeState(STATE_ERROR);
		errorCode=ERROR_INITIALIZATION_TIMEOUT;
	}
}

void IRToy_Controller::State_SetSamplerMode(void)
{
	if(true!=stateCommandSent && 300<StateTimer())
	{
		const unsigned char cmd[1]={CMD_SAMPLERMODE};
		Send(1,cmd);
		stateCommandSent=true;
		ResetStateTimer();
	}

	auto readBuf=Read();
	if(0<readBuf.size())
	{
		ShowBuf(readBuf.size(),readBuf.data());

		for(auto b : readBuf)
		{
			feedBackBuf.push_back(b);
		}
		if(3<=feedBackBuf.size())
		{
			for(int i=0; i+2<feedBackBuf.size(); ++i)
			{
				if('S'==feedBackBuf[i] &&
				   '0'<=feedBackBuf[i+1] && feedBackBuf[i+1]<='9' &&
				   '0'<=feedBackBuf[i+2] && feedBackBuf[i+2]<='9')
				{
					protocolVersion[0]=feedBackBuf[i];
					protocolVersion[1]=feedBackBuf[i+1];
					protocolVersion[2]=feedBackBuf[i+2];
					protocolVersion[3]=0;
					ChangeState(STATE_CONFIGURE);
				}
			}
		}
	}
}

void IRToy_Controller::State_Configure(void)
{
	if(true!=stateCommandSent && 250<StateTimer())
	{
		if(0==stateCounter)
		{
			const unsigned char cmd[2]={0x05,0x00};
			// Set up for 1:2 scale
			// 00 -> 21.3333us / 128 = 0.17us
			// 01 -> 21.3333us /  64 = 0.33us
			// 02 -> 21.3333us /  32 = 0.66666us
			// 03 -> 21.3333us /  16 = 1.33us

			// 1 cycle = 12.5us On + 12.5us Off =25us
			// 1 bit=4 cycles=100us
			// 1 key stroke=40bits=4000us
			// 4000us/0.17us=23529ticks
			// 4000us/0.67us= 5970ticks 0x1752.  Top byte never reaches '$'
			// 4000us/1.33us= 3007ticks

			// I want to avoid sending '$' even as a pulse.  But, the test indicated that
			// FM77AV doesn't like 0.6666us unit time.  I go with 0.17us, and hopefully it doesn't appear
			// in the bit patterns.

			// To get to 0x2400 ticks, 0 or 1 must continue for 1566seconds.
			// 16bit of continuous 0 or 1.
			// Looking at the codes, nowhere 0 or 1 continus more than 3 bits.
			// Safe with 0.17us.
			Send(2,cmd);
			++stateCounter;
			stateCommandSent=false;
			ResetStateTimer();
		}
		else if(1==stateCounter)
		{
			const unsigned char cmd[1]={CMD_ENABLE_BYTECOUNT};
			Send(1,cmd);
			++stateCounter;
			stateCommandSent=false;
			ResetStateTimer();
		}
		else if(2==stateCounter)
		{
			const unsigned char cmd[1]={CMD_ENABLE_TRANSMISSION_NOTIFY};
			Send(1,cmd);
			++stateCounter;
			stateCommandSent=false;
			ResetStateTimer();
		}
		else if(3==stateCounter)
		{
			// const unsigned char cmd[1]={CMD_ENABLE_HANDSHAKE};  // This doesn't seem to work. Even after upgrading to firmware v22
			// Send(1,cmd);
			++stateCounter;
			ResetStateTimer();
		}
		else
		{
			stateCommandSent=true;
			ResetStateTimer();
		}
	}
	if(300<StateTimer())
	{
		printf("Raw Sampling Mode\n");
		ChangeState(STATE_GOWILD);
	}

}

void IRToy_Controller::State_GoWild(void)
{
	auto readBuf=Read();
	if(0<readBuf.size())
	{
		ShowBuf(readBuf.size(),readBuf.data());
		nAccumByte+=readBuf.size();
	}
}

bool IRToy_Controller::StartRecording(void)
{
	if(STATE_GOWILD==state && 500<StateTimer())
	{
		printf("Start Recording\n");
		ChangeState(STATE_RECORDING);
		recording.clear();
		return true;
	}
	return false;
}

bool IRToy_Controller::EndRecording(void)
{
	if(STATE_RECORDING==state)
	{
		ChangeState(STATE_GOWILD);
		MakeTerminator(recording);
		PrintRecording(recording);
		return true;
	}
	return false;
}

std::vector <unsigned char> IRToy_Controller::GetRecording(void) const
{
	return recording;
}

bool IRToy_Controller::IsInRecordingMode(void) const
{
	if(STATE_RECORDING==state)
	{
		return true;
	}
	return false;
}

void IRToy_Controller::State_Recording(void)
{
	for(auto c : Read())
	{
		printf(".");
		recording.push_back(c);
	}
}

bool IRToy_Controller::SetRecording(const std::vector <unsigned char> &recorded)
{
	if(STATE_GOWILD==state)
	{
		recording=recorded;
		return true;
	}
	return false;
}

bool IRToy_Controller::StartTransmit(void)
{
	if(STATE_GOWILD==state && 0<recording.size())
	{
		printf("Start Transmitting\n");
		ChangeState(STATE_TRANSMITTING);
		transmitPtr=0;
		return true;
	}
	return false;
}
void IRToy_Controller::State_Transmitting(void)
{
	if(true!=stateCommandSent)
	{
		CleanRecording(recording);
	}
	const unsigned char cmd[]={CMD_TRANSMIT};
	Send(1,cmd);
	Send(recording.size(),recording.data());
	comPort.FlushWriteBuffer();  // If the transmission is terminated in the middle, it's the end of the IRToy.  Need to unplug and plug back in.
	ChangeState(STATE_GOWILD);

/*  The following is supposed to work if IRToy sends hand-shake signal as advertised, which does not happen.
    Anyway, it needs to be fast.

	if(true!=stateCommandSent)
	{
		CleanRecording();
		const unsigned char cmd[]={CMD_TRANSMIT};
		Send(1,cmd);
		stateCommandSent=true;
	}

	unsigned char readBuf[256];
	long long int byteRead=0;
	if(true==Read(byteRead,readBuf,256))
	{
		if(0==stateCounter)
		{
			for(int i=0; i<byteRead; ++i)
			{
				if(0x3E==byteRead)
				{
					bool lastByte=false;
					int nSend=0;
					unsigned char sendBuf[64];
					for(int i=0; i<64 && transmitPtr+2<=recording.size() && true!=lastByte; i+=2)
					{
						sendBuf[nSend]  =recording[transmitPtr];
						sendBuf[nSend+1]=recording[transmitPtr+1];
						if(recording[transmitPtr]==0xff && recording[transmitPtr+1]==0xff)
						{
							lastByte=true;
						}
						transmitPtr+=2;
						nSend+=2;
					}
					Send(nSend,sendBuf);
					if(true==lastByte)
					{
						stateCounter=1;
					}
				}
			}
		}
		else if(1==stateCounter)
		{
			for(int i=0; i<byteRead; ++i)
			{
				feedBackBuf.push_back(readBuf[i]);
				ShowBuf(feedBackBuf.size(),feedBackBuf.data());
			}
			if(4==feedBackBuf.size())
			{
				ChangeState(STATE_GOWILD);
			}
		}
	}*/
}

void IRToy_Controller::State_Closing(void)
{
	if(true!=stateCommandSent)
	{
		const unsigned char cmd[5]={CMD_RESET,CMD_RESET,CMD_RESET,CMD_RESET,CMD_RESET};
		Send(5,cmd);
		ResetStateTimer();
		stateCommandSent=true;
	}
	else if(300<StateTimer())
	{
		const unsigned char cmd[1]={'R'};
		Send(1,cmd);
		ResetStateTimer();
	}

	auto readBuf=Read();
	if(0<readBuf.size())
	{
		if(0<feedBackBuf.size() && feedBackBuf.back()=='O' && readBuf[0]=='K')
		{
			ChangeState(STATE_CLOSED);
		}
		std::swap(feedBackBuf,readBuf);
		for(int i=0; i+1<feedBackBuf.size(); ++i)
		{
			if('O'==feedBackBuf[i] && 'K'==feedBackBuf[i+1])
			{
				ChangeState(STATE_CLOSED);
			}
		}
	}
}
void IRToy_Controller::State_Closed(void)
{
}

void IRToy_Controller::SaveRecording(const char fName[]) const
{
	FILE *fp=fopen(fName,"w");
	if(nullptr!=fp)
	{
		fprintf(fp,"DEFAULT");
		for(auto c : recording)
		{
			fprintf(fp," %d",c);
		}
		fprintf(fp,"\n");
		fclose(fp);
	}
}

void IRToy_Controller::LoadRecording(const char fName[])
{
	FILE *fp=fopen(fName,"r");
	if(nullptr!=fp)
	{
		std::string str;
		if(nullptr!=CheapFGets(str,fp))
		{
			recording.clear();
			auto argv=CheapParser(str.c_str());
			for(int i=1; i<argv.size(); ++i)
			{
				recording.push_back(atoi(argv[i].c_str()));
			}
			MakeTerminator(recording);
			PrintRecording(recording);
		}

		fclose(fp);
	}
}

void IRToy_Controller::Make100usPulse(const char ptn[],bool verbose)
{
	// Mr. Kobayashi from Classic PC & Retro Gaming JAPAN suggests that
	// the keyboard is using pulse of 100us 125us 175us 100us 125us 175us ....
	// I am going with 100us fixed pulse here with every 3rd and 4th bit the same.
	// Both looks to work fine.
	std::vector <double> microSecAccum;
	double t=0.0;
	char prev='1';
	for(int i=0; 0!=ptn[i]; ++i)
	{
		if(prev!=ptn[i])
		{
			microSecAccum.push_back(t);
		}

		t+=100.0;
		prev=ptn[i];
	}
	microSecAccum.push_back(t);

	if(true==verbose)
	{
		for(auto t : microSecAccum)
		{
			printf("%lf ",t);
		}
		printf("\n");
	}


	std::vector <unsigned int> timing;

	int discreteT=0;
	for(int i=0; i<microSecAccum.size(); ++i)
	{
		int accumDiscreteT=(int)(microSecAccum[i]/0.17);
		auto stepT=accumDiscreteT-discreteT;
		if((stepT&255)=='$')
		{
			printf("Adjust for '$'!\n");
			stepT--;
		}
		timing.push_back(stepT);
		if(true==verbose)
		{
			printf("%d ",stepT);
		}
		discreteT+=stepT;
	}
	if(true==verbose)
	{
		printf("\n");
	}

	if(0==timing.size()%2)
	{
		timing.back()=65535;
	}
	else
	{
		timing.push_back(65535);
	}

	recording.clear();
	for(auto t : timing)
	{
		recording.push_back(t/256);
		recording.push_back(t&255);
	}

	if(true==verbose)
	{
		PrintRecording(recording);
	}
}

void IRToy_Controller::RunOneStep(void)
{
	comPort.Update();

	switch(state)
	{
	case STATE_DISCONNECT:
		break;
	case STATE_INITIALIZING:
		State_Initializing();
		break;
	case STATE_HANDSHAKE:
		State_Handshake();
		break;
	case STATE_SELFTEST:
		State_SelfTest();
		break;
	case STATE_SETSAMPLERMODE:
		State_SetSamplerMode();
		break;
	case STATE_CONFIGURE:
		State_Configure();
		break;
	case STATE_GOWILD:
		State_GoWild();
		break;

	case STATE_RECORDING:
		State_Recording();
		break;
	case STATE_TRANSMITTING:
		State_Transmitting();
		break;

	case STATE_CLOSING:
		State_Closing();
		break;
	case STATE_CLOSED:
		State_Closed();
		break;

	case STATE_ERROR:
		break;

	default:
		printf("Undefined state.\n");
		break;
	}
}

int IRToy_Controller::GetState(void) const
{
	return state;
}
int IRToy_Controller::GetErrorCode(void) const
{
	return errorCode;
}

std::string IRToy_Controller::GetIRToyVersion(void) const
{
	return irToyVersion;
}
std::string IRToy_Controller::GetProtocolVersion(void) const
{
	return protocolVersion;
}
