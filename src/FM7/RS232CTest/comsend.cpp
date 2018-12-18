#include <stdio.h>
#include <stdlib.h>
#include "comport.h"

int main(int ac,char *av[])
{
	if(ac<2)
	{
		printf("Usage:\n");
		printf("  comsend port <Text>\n");
		return 1;
	}

	int port=atoi(av[1]);
	const char *toSend=(ac<3 ? "Hello from COMSEND" : av[2]);

	YsCOMPort comPort;
	comPort.SetDesiredFlowControl(comPort.FLOWCONTROL_HARDWARE);
	comPort.SetDesiredBaudRate(19200);
	if(true==comPort.Open(port))
	{
		printf("Press Enter to Transmit>");
		getchar();

		comPort.Send(strlen(toSend),(unsigned char*)toSend);

		unsigned char CRLF[]={0x0d,0x0a};
		comPort.Send(2,CRLF);

		comPort.FlushWriteBuffer();
	}
	else
	{
		printf("Failed to open COM %d\n",port);
	}

	return 0;
}
