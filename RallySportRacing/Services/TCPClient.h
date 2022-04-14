#ifndef TCPCLIENT
#define TCPCLIENT

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

#define ADDRESS "167.71.13.30"
//#define ADDRESS "127.0.0.1"
#define PORT 2005

#define BUFLEN 1024

using namespace std;

namespace Server {

	class TCPClient {
	public:
		TCPClient();
		void connectToServer();
		void listen();
		void sendPacket(string msg);
		void terminate();

		void addCallback(void (*func)(string str));

	private:
		SOCKET connectSocket;
		SOCKADDR_IN serverAddress;
		WSADATA wsaData;
		vector<void (*)(string str)> funcs;
	};

}

#endif
