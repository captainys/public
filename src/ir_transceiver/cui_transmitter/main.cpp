#include <stdio.h>

#include <vector>
#include <string>

#include <chrono>
#include <thread>

#include "../gui/irdata.h"
#include "irtoy.h"

class IRSend
{
public:
	std::string dataFName;
	std::vector <std::string> keyName;
	std::vector <const IRData *> keyData;

	IRDataSet irDataSet;

	std::string portName;
	IRToy_Controller transmitter;

	int wait;

	IRSend();
	bool RecognizeCommandParameter(int ac,char *av[]);
	bool LoadData(void);
	bool AssembleKeyData(void);

	bool Connect(void);
	void Send(void);
	void Close(void);
};

IRSend::IRSend()
{
	wait=0;
}

bool IRSend::RecognizeCommandParameter(int ac,char *av[])
{
	int nonOptionCtr=0;
	for(int i=1; i<ac; ++i)
	{
		std::string cmd=av[i];
		if(cmd=="-wait")
		{
			if(i+1<ac)
			{
				this->wait=atoi(av[i+1]);
				++i;
			}
			else
			{
				fprintf(stderr,"Missing parametr for -wait option.\n");
				return false;
			}
		}
		else
		{
			if(0==nonOptionCtr)
			{
				dataFName=av[i];
			}
			else if(1==nonOptionCtr)
			{
				portName=av[i];
			}
			else
			{
				keyName.push_back(av[i]);
			}
			++nonOptionCtr;
		}
	}

	if(0==nonOptionCtr)
	{
		fprintf(stderr,"Data File Name is not given.\n");
		return false;
	}
	else if(1==nonOptionCtr)
	{
		fprintf(stderr,"Port Name is not given.\n");
		return false;
	}
	else if(2==nonOptionCtr)
	{
		fprintf(stderr,"No key name is given.\n");
		return false;
	}
	return true;
}

bool IRSend::LoadData(void)
{
	FILE *fp=fopen(dataFName.c_str(),"r");
	if(nullptr!=fp)
	{
		auto res=irDataSet.Load(fp);
		fclose(fp);
		return res;
	}
	return false;
}

bool IRSend::AssembleKeyData(void)
{
	keyData.clear();
	for(auto &name : keyName)
	{
		const IRData *dataPtr=nullptr;
		for(auto &data : irDataSet.dataSet)
		{
			if(name==data.label)
			{
				dataPtr=&data;
				break;
			}
		}
		if(nullptr==dataPtr)
		{
			fprintf(stderr,"Key %s not found.\n",name.c_str());
			return false;
		}
		else if(0==dataPtr->sample.size())
		{
			fprintf(stderr,"Key %s does not have a sample data.\n",name.c_str());
		}
		keyData.push_back(dataPtr);
	}
	return true;
}

bool IRSend::Connect(void)
{
	if(true==transmitter.Connect(portName))
	{
		while(transmitter.STATE_GOWILD!=transmitter.GetState())
		{
			transmitter.RunOneStep();
			if(transmitter.STATE_DISCONNECT==transmitter.GetState() ||
			   transmitter.STATE_ERROR==transmitter.GetState())
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}
void IRSend::Send(void)
{
	for(int i=0; i<keyName.size() && i<keyData.size(); ++i)
	{
		printf("Sending [%s]\n",keyName[i].c_str());

		auto modulated=keyData[i]->PWMtoModulated(keyData[i]->sample[0]);
		transmitter.StartTransmitMicroSecPulse(modulated.size(),modulated.data());

		while(transmitter.STATE_GOWILD!=transmitter.GetState())
		{
			transmitter.RunOneStep();
			if(true!=transmitter.IsConnected() ||
		      transmitter.STATE_ERROR==transmitter.GetState() &&
		      transmitter.STATE_DISCONNECT==transmitter.GetState())
		    {
				fprintf(stderr,"Something went wrong.\n");
				return;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(wait));
	}
}
void IRSend::Close(void)
{
	while(true==transmitter.IsConnected() &&
	      transmitter.STATE_GOWILD!=transmitter.GetState() &&
	      transmitter.STATE_ERROR!=transmitter.GetState() &&
	      transmitter.STATE_DISCONNECT!=transmitter.GetState())
	{
		transmitter.RunOneStep();
	}
	transmitter.Disconnect();
	while(true==transmitter.IsConnected() &&
	      transmitter.STATE_CLOSED!=transmitter.GetState() &&
	      transmitter.STATE_ERROR!=transmitter.GetState() &&
	      transmitter.STATE_DISCONNECT!=transmitter.GetState())
	{
		transmitter.RunOneStep();
	}
}

int main(int ac,char *av[])
{
	IRSend irsend;

	if(true!=irsend.RecognizeCommandParameter(ac,av))
	{
		printf("Usage:\n");
		printf("  irsend datafile.txt portName keyName keyName ...\n");
		printf("Options:\n");
		printf("  -wait 500\n");
		printf("    Wait 500ms between keys.\n");
		return 1;
	}

	if(true!=irsend.LoadData())
	{
		fprintf(stderr,"Failed to load IR data.\n");
		return 1;
	}

	if(true!=irsend.AssembleKeyData())
	{
		fprintf(stderr,"Failed to assemble key data.\n");
		return 1;
	}

	if(true==irsend.Connect())
	{
		irsend.Send();
		irsend.Close();
	}
	else
	{
		fprintf(stderr,"Failed to connect to the transmitter device.\n");
		return 1;
	}

	return 0;
}
