#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "comport.h"

int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage:\n");
		printf("  comrecv port\n");
		return 1;
	}

	std::string port(av[1]);

	YsCOMPort comPort;
	comPort.SetDesiredBaudRate(19200);
	if(true==comPort.Open(port))
	{
		for(;;)
		{
			auto incoming=comPort.Receive();
			if(0<incoming.size())
			{
				incoming.push_back(0);
				printf("Received: %s\n",incoming.data());
			}
		}
	}
	else
	{
		printf("Failed to open COM %s\n",port.c_str());
	}

	return 0;
}
