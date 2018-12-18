#include <chrono>
#include <string>
#include <thread>
#include <mutex>
#include "comport.h"

std::mutex mutex;
std::string toSend;

void Thread(void)
{
	printf("Type String to Send.\n");
	for(;;)
	{
		char str[256];
		fgets(str,255,stdin);

		mutex.lock();
		toSend=str;
		mutex.unlock();
	}
}

int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage:\n");
		printf("  comsendrecv port\n");
		return 1;
	}

	int port=atoi(av[1]);

	std::thread t(Thread);

	YsCOMPort comPort;
	comPort.SetDesiredBaudRate(19200);
	if(true==comPort.Open(port))
	{
		auto t0=std::chrono::system_clock::now();
		for(;;)
		{
			auto incoming=comPort.Receive();
			if(0<incoming.size())
			{
				incoming.push_back(0);
				printf("Received: %s\n",incoming.data());
			}

			auto tNow=std::chrono::system_clock::now();
			auto dt=std::chrono::duration_cast<std::chrono::milliseconds> (tNow-t0).count();
			if(10<dt)
			{
				t0=tNow;
				mutex.lock();
				if(0<toSend.size())
				{
					while(0<toSend.size() && (toSend.back()==0x0d || toSend.back()==0x0a))
					{
						toSend.pop_back();
					}
					toSend.push_back(0x0d);
					printf("Sending: %s\n",toSend.c_str());
					comPort.Send(toSend.size(),(const unsigned char *)toSend.data());
					comPort.FlushWriteBuffer();
					toSend="";
				}
				mutex.unlock();
			}
		}
	}
	else
	{
		printf("Failed to open COM %d\n",port);
	}

	return 0;
}
