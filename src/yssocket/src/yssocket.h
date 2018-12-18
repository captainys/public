#ifndef YSSOCKET_IS_INCLUDED
#define YSSOCKET_IS_INCLUDED
/* { */


#include <ysclass.h>


#ifdef _WIN32
#define _WINSOCKAPI_
	// According to the Win32API documentation,
	// I must define _WINSOCKAPI_ to prevent inclusion of
	// winsock.h from windows.h
	// WIN32_LEAN_AND_MEAN works the same.
	#ifndef WIN32_LEAN_AND_MEAN
		// Prevent inclusion of winsock.h
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#undef WIN32_LEAN_AND_MEAN
	#else
		#include <windows.h>
	#endif

	#include <winsock2.h>
#else
	#include <unistd.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	typedef int SOCKET;
#endif


class YsSocket
{
protected:
	enum
	{
		nBufferSize=4096
	};
	unsigned char buffer[nBufferSize];
};


class YsSocketServer : public YsSocket
{
public:
	YsSocketServer(int port,int nMaxNumClient);
	~YsSocketServer();

	YSRESULT Start(void);
	YSRESULT Terminate(void);

	YSRESULT CheckAndAcceptConnection(void);
	YSRESULT CheckReceive(void);

	/*! Returns max number of clients that can connect to this server. */
	int GetNumClient(void) const;

	YSBOOL IsClientActive(int clientId) const;

	/*! Returns number of connected clients. */
	int GetNumConnectedClient(void) const;

	YSRESULT Disconnect(int clientId);   /* clientId=-1 to close all connections */

	YSRESULT Send(int clientId,YSSIZE_T nBytes,unsigned char dat[],unsigned timeout);   /* clientId=-1 to broadcast */
	virtual YSRESULT ReceivedFrom(int clientId,YSSIZE_T nBytes,unsigned char dat[])=0;
	virtual YSRESULT SendTerminateMessage(int clientId,unsigned timeout);
	virtual YSRESULT ConnectionAccepted(int clientId,unsigned int ipAddr[4]);
	virtual YSRESULT ConnectionClosedByClient(int clientId);

protected:
	SOCKET GetClientSocket(int clientId);

	YSBOOL started;

	int listeningPort;
	int maxNumClient;
	YSBOOL *clientSockUsed;

	/* Temporary Variable */
	YSBOOL *clientReady;
	SOCKET listeningSocket;
	SOCKET *clientSock;
};

////////////////////////////////////////////////////////////

class YsSocketClient : public YsSocket
{
public:
	YsSocketClient();
	YsSocketClient(int port);
private:
	void Initialize(int port);

public:
	~YsSocketClient();

	/*! Starts the client.  If port is already specified in the constructor, the port given to the constructor is ignored.
	*/
	YSRESULT Start(int port);

	/*! Starts the client.  The port number must be specified in the constructor.
	*/
	YSRESULT Start(void);
	YSRESULT Terminate(void);

	YSRESULT Connect(const char hostaddr[]);
	YSRESULT Disconnect(void);

	YSBOOL IsConnected(void);

	YSRESULT Send(YSSIZE_T nBytes,unsigned char dat[],unsigned timeout);
	YSRESULT CheckReceive(void);

	virtual YSRESULT Received(YSSIZE_T nBytes,unsigned char dat[])=0;
	virtual YSRESULT ConnectionClosedByServer(void);
protected:
	YSBOOL started;

	YSBOOL connected;
	int port;
	SOCKET sock;
};


/* } */
#endif
