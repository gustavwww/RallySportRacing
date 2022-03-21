#ifndef TCPCLIENT
#define TCPCLIENT

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

namespace Server {

	class TCPClient {
	public:
		TCPClient();
		void connectToServer();

	private:
		SOCKET sendingSocket;
		SOCKADDR_IN serverAddress;
		int result;

	};

}

#endif
