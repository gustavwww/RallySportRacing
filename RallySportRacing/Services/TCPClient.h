#ifndef TCPCLIENT
#define TCPCLIENT

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

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
