#include "TCPClient.h"
#include <iostream>

using namespace std;

namespace Server {

	TCPClient::TCPClient() {

		connectSocket = INVALID_SOCKET;
		
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) {
			cout << "WSAStartup failed, error: " << result << endl;
			throw "WSA Failed to startup.";
		}

		connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connectSocket == INVALID_SOCKET) {
			cout << "Client socket failed to initialize, error: " << WSAGetLastError() << endl;
			WSACleanup();
			throw "Socket could not be initialized.";
		}
		cout << "TCP socket created." << endl;

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(PORT);
		serverAddress.sin_addr.s_addr = inet_addr(ADDRESS);
	}

	void TCPClient::connectToServer() {

		cout << "Atempting to connect to server..." << endl;
		int result = connect(connectSocket, (SOCKADDR*) &serverAddress, sizeof(serverAddress));
		if (result != 0) {
			cout << "Connection to server failed " << WSAGetLastError() << endl;
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			WSACleanup();
			throw "Error connecting to server";
		}

		//getsockname(connectSocket, (SOCKADDR*)&serverAddress, (int*)sizeof(serverAddress));
		cout << "Connected to server address " << inet_ntoa(serverAddress.sin_addr) << " port " << htons(serverAddress.sin_port) << endl;

	}

	void TCPClient::listen() {
		char receiveBuffer[BUFLEN];
		int receiveBufLen = BUFLEN;

		int result;
		do {
			result = recv(connectSocket, receiveBuffer, receiveBufLen, 0);

			string str(receiveBuffer);
			str.erase(find_if(str.begin(), str.end(), [](unsigned char c) {
				return c == '\n';
				}), str.end());

			for (void(*func)(string str) : funcs) {
				if (func) {
					(*func)(str);
				}
			}

		} while (result > 0);

		if (result == 0) {
			cout << "Connection to server closed." << endl;
		} else {
			throw "Error receiving packet from server";
		}

		closesocket(connectSocket);
		WSACleanup();
	}

	void TCPClient::sendPacket(string msg) {
		if (connectSocket == INVALID_SOCKET) {
			cout << "Attempted to send packet. Not connected to server." << endl;
			return;
		}
		string msg_nl = msg + "\n";
		const char* str = msg_nl.c_str();

		int result = send(connectSocket, str, (int) strlen(str), 0);
		if (result == SOCKET_ERROR) {
			closesocket(connectSocket);
			WSACleanup();
			cout << "Error sending TCP message, " << WSAGetLastError() << endl;
		}
	}

	void TCPClient::terminate() {
		if (connectSocket != INVALID_SOCKET) {
			closesocket(connectSocket);
			WSACleanup();
		}
	}

	void TCPClient::addCallback(void (*func)(string str)) {
		funcs.push_back(func);
	}


}