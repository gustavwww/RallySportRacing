#include "TCPClient.h"
#include <iostream>

#define ADDRESS "localhost"
#define PORT 2005

using namespace std;

namespace Server {

	TCPClient::TCPClient() {

		WSADATA wsaData;

		sendingSocket = INVALID_SOCKET;
		result = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (result != 0) {
			cout << "WSAStartup failed, error: " << result << endl;
		}

		sendingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sendingSocket == INVALID_SOCKET) {
			cout << "Client socket failed to initialize, error: " << WSAGetLastError() << endl;
			WSACleanup();
		}
		cout << "Client socket created." << endl;

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(PORT);
		serverAddress.sin_addr.s_addr = inet_addr(ADDRESS);
	}

	void TCPClient::connectToServer() {

		int retCode = connect(sendingSocket, (SOCKADDR*) &serverAddress, sizeof(serverAddress));
		if (retCode != 0) {
			cout << "Connection to server failed " << WSAGetLastError() << endl;
			closesocket(sendingSocket);
			WSACleanup();
			return;
		}

		getsockname(sendingSocket, (SOCKADDR*)&serverAddress, (int*)sizeof(serverAddress));
		cout << "Connected to server address " << inet_ntoa(serverAddress.sin_addr) << " port " << htons(serverAddress.sin_port) << endl;

		//TODO

	}


}