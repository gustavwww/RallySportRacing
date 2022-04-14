#include "UDPClient.h"
#include <iostream>

using namespace std;

namespace Server {

	UDPClient::UDPClient() {

		sock = INVALID_SOCKET;

		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) {
			cout << "WSAStartup failed, error: " << result << endl;
			throw "WSA Failed to startup.";
		}

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET) {
			cout << "Client socket failed to initialize, error: " << WSAGetLastError() << endl;
			WSACleanup();
			throw "Socket could not be initialized.";
		}
		cout << "UDP socket created." << endl;

		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(PORT);
		serverAddress.sin_addr.s_addr = inet_addr(ADDRESS);

		sendPacket("s");
	}

	void UDPClient::listen() {
		char receiveBuffer[BUFLEN];
		int receiveBufLen = BUFLEN;

		int addressLen = sizeof(serverAddress);

		int result;
		do {
			result = recvfrom(sock, receiveBuffer, receiveBufLen, 0, (SOCKADDR*)&serverAddress, &addressLen);
			if (result == SOCKET_ERROR) {
				cout << "Error receiving UDP packet " << WSAGetLastError() << endl;
			}

			string str(receiveBuffer);
			str.erase(find_if(str.begin(), str.end(), [](unsigned char c) {
				return c == '\n';
				}), str.end());
			

			for (void(*func)(string str) : funcs) {
				if (func) {
					(*func)(str);
				}
			}
			
		} while (result != 0);

		closesocket(sock);
		WSACleanup();
	}

	void UDPClient::sendPacket(string msg) {
		if (sock == INVALID_SOCKET) {
			cout << "Attempted to send packet. Invalid socket." << endl;
			return;
		}
		string msg_nl = msg + "\n";
		const char* str = msg_nl.c_str();

		int result = sendto(sock, str, strlen(str), 0, (SOCKADDR*) &serverAddress, sizeof(serverAddress));
		if (result == SOCKET_ERROR) {
			closesocket(sock);
			WSACleanup();
			cout << "Error sending UDP message, " << WSAGetLastError() << endl;
		}
	}

	void UDPClient::terminate() {
		if (sock != INVALID_SOCKET) {
			closesocket(sock);
			WSACleanup();
		}
	}

	void UDPClient::addCallback(void (*func)(string str)) {
		funcs.push_back(func);
	}


}