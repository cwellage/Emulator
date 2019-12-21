// Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include<winsock.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "KeyboardReceiver.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#if defined(UNICODE) || defined(_UNICODE)
#define _tcout std::wcout
#define _tcin std::wcin
#else
#define _tcout std::cout
#define _tcout std::cin
#endif 
using namespace std;
typedef std::basic_string<TCHAR> _tstring;


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1234"
SOCKET ConnectSocket = 296;
const SOCKET ServerSocket = 1234;
int iResult;


void CreateClient()
{
	WSADATA wsaData;
	
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	char recvbuf[DEFAULT_BUFLEN]{};
	int recvbuflen = DEFAULT_BUFLEN;
	
	//// Validate the parameters
	//if (argc != 2) {
	//	printf("usage: %s server-name\n", argv[0]);
	//	return 1;
	//}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		//return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		//return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			//return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr-> ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		//return 1;
	}
};


int main(int argc, char* argv[])
{
	try
	{
		//start server
		int result = system("start C:\\Users\\Chatura\\source\\repos\\Emulator\\Emulator\\Debug\\IAS0410PlantEmulator.exe 4");
		//string path = "start"+ argv[0];
		//int result = system("start "+ *argv[0]+ (char*)" 4");
		//create client
		CreateClient();
		//Keyboard thread and send and receive
		KeyboardReceiver rec;
		thread RunKeyboard(&KeyboardReceiver::ReadKeyboard, rec, ConnectSocket);
		RunKeyboard.join();
	}
	catch (const std::exception& e)
	{

	}

};

	
//start logic here
	//			/*KeyboardReceiver rec;
	//			thread RunKeyboard(&KeyboardReceiver::KeyboardThread, rec, ConnectSocket);
	//			RunKeyboard.join();*/