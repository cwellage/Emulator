#include "Winsock2.h" // necessary for sockets, Windows.h is not needed.
#include "mswsock.h"
#include "process.h"  // necessary for threading
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <array>
#include "KeyboardReceiver.h"
#include <string>
#include "CommandSender.h"
#define _DEBUG

#define DEFAULT_BUFLEN 512
#if defined(UNICODE) || defined(_UNICODE)
#define _tcout std::wcout
#define _tcin std::wcin
#else
#define _tcout std::cout
#define _tcout std::cin
#endif 
using namespace std;
typedef std::basic_string<TCHAR> _tstring;
//
// Global variables
//

SOCKET hClientSocket = INVALID_SOCKET;


	void KeyboardReceiver::ReadKeyboard(SOCKET ConnectSocket)
	{
		hClientSocket = ConnectSocket;
		_tstring sCommand = _T("");
		int iResult;

		do {
			string message{};
			char recvbuf[DEFAULT_BUFLEN]{};
			int recvbuflen = DEFAULT_BUFLEN;

			if (sCommand != _T("Break") && sCommand != _T("Stop"))
			{
				iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
				message = getUnicodeString(recvbuf);
			}
			if (iResult > 0)
			{
				message = getUnicodeString(recvbuf);
				if (message == "Identify")
				{
					cout << "emulator :" + message << endl;
					cout << "Logger: coursework" << endl;
					iResult = ProcessCommandAndSend(_T("coursework"));
					continue;
				}
				if (message == "Accepted")
				{
					cout << "emulator :" + message << endl;
					//continue;
				}
				if (sCommand != _T("") && sCommand != _T("Break"))
				{
					std::cout <<""<< endl;
					std::cout << getOutputMeasurementResult(recvbuf) << endl;
					std::cout << "" << endl;
				}

				InputFromKeyBoard(sCommand, iResult);

				if (iResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ConnectSocket);
					WSACleanup();
					printf("Bytes received: %d\n", iResult);
				}
			}

			else if (iResult == 0)
			{
				printf("Invalid Command no bytes received \n");
				InputFromKeyBoard(sCommand, iResult);
			}
			else {
				printf("recv failed with error: %d\n", WSAGetLastError());
				break;
			}
				

		} while (true);

		printf("Bytes Sent: %ld\n", iResult);

		//exit 

		// shutdown the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("return 0: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			//return 1;
		}


		// cleanup
		closesocket(ConnectSocket);
		WSACleanup();
	};

	 int KeyboardReceiver::ProcessCommandAndSend(_tstring command)
	{
		 const int cmdByteLen = command.length() * 2;
		 const int packetLen = cmdByteLen + 6;
		 char* byteArray{ new char[packetLen] {} };

		 char bytes[4] = { 0,0,0,0 };
		 memcpy(bytes, &packetLen, 4);

		 char* stringBytes{ new char[packetLen] {} };
		 //static const char password[] = command";
		 int s = 0;
		 //char* tempdata = command.data()[;
		 for (int i = 0; i < cmdByteLen / 2; i++)
		 {
			 stringBytes[s] = command.data()[i];
			 s = s + 2;
		 }
		 memcpy(byteArray, bytes, 4);
		 memcpy(byteArray + 4, stringBytes, cmdByteLen - 1);

		 // Send an initial buffer
		 int iResult = send(hClientSocket, byteArray, packetLen, 0);

		 return iResult;
	};

	 std::string KeyboardReceiver::getUnicodeString(char array[])
	 {
		 std::string s{};
		 for (int i = 4; i < array[0] - 1; i++)
		 {
			 if (array[i] != '\0')
			 {
				 s += array[i];
			 }
		 }
		 return s;
	 };

	 std::string KeyboardReceiver::getOutputMeasurementResult(char array[])
	 {
		 std::string s{};
		 int a = (int)array[0];
		 for (int i = 4; i < 512; i++)
		 {
			 if (array[i] != '\0')
			 {
				 s += (char)array[i];
			 }
		 }
		 return s;
	 };

	 void KeyboardReceiver::InputFromKeyBoard(_tstring &sCommand,int &iResult)
	 {
		 _tcout << _T("Logger:");
		 _tcin >> sCommand;
		 if (!sCommand.compare(_T("exit")))
			 return;
		 else
		 {
			sCommand[0] = toupper(sCommand[0]); // first letter upper start -> Start
			iResult = ProcessCommandAndSend(sCommand);
		 }			 		 
	 }