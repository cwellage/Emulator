#include "Winsock2.h" // necessary for sockets, Windows.h is not needed.
#include "mswsock.h"
#include "process.h"  // necessary for threading
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <array>
#include "DataReceiverAndAnalyzer.h"
#define _DEBUG


unsigned int __stdcall DataReceiverAndAnalyzer::ReceiveNet(void* pArguments)
{
	//wchar_t CommandBuf[81]{};
	//HANDLE hCommandGot;       // event "the user has typed a command"
	HANDLE hStopCommandGot{};   // event "the main thread has recognized that it was the stop command"
	//HANDLE hCommandProcessed; // event "the main thread has finished the processing of command"
	//HANDLE hReadKeyboard;     // keyboard reading thread handle
	//HANDLE hStdIn;			  // stdin standard input stream handle
	//WSADATA WsaData;          // filled during Winsock initialization 
	DWORD Error;
	SOCKET hClientSocket = INVALID_SOCKET;
	//sockaddr_in ClientSocketInfo;
	//HANDLE hReceiveNet;       // TCP/IP info reading thread handle
	//BOOL SocketError;

	//
	// Preparations
	//
	WSABUF DataBuf;  // Buffer for received data is a structure
	char ArrayInBuf[2048];
	DataBuf.buf = &ArrayInBuf[0];
	DataBuf.len = 2048;
	DWORD nReceivedBytes = 0, ReceiveFlags = 0;
	HANDLE NetEvents[2];
	NetEvents[0] = hStopCommandGot;
	WSAOVERLAPPED Overlapped;
	memset(&Overlapped, 0, sizeof Overlapped);
	Overlapped.hEvent = NetEvents[1] = WSACreateEvent(); // manual and nonsignaled
	DWORD Result;
	//
	// Receiving loop
	//
	while (TRUE)
	{
		Result = WSARecv(hClientSocket,
			&DataBuf,
			1,  // no comments here
			&nReceivedBytes,
			&ReceiveFlags, // no comments here
			&Overlapped,
			NULL);  // no comments here
		if (Result == SOCKET_ERROR)
		{  // Returned with socket error, let us examine why
			if ((Error = WSAGetLastError()) != WSA_IO_PENDING)
			{  // Unable to continue, for example because the server has closed the connection
				_tprintf(_T("WSARecv() failed, error %d\n"), Error);
				goto out;
			}
			DWORD WaitResult = WSAWaitForMultipleEvents(2, NetEvents, FALSE, WSA_INFINITE, FALSE); // wait for data
			switch (WaitResult) // analyse why the waiting ended
			{
			case WAIT_OBJECT_0:
				// Waiting stopped because hStopCommandGot has become signaled, i.e. the user has decided to exit
				goto out;
			case WAIT_OBJECT_0 + 1:
				// Waiting stopped because Overlapped.hEvent is now signaled, i.e. the receiving operation has ended. 
				// Now we have to see how many bytes we have got.
				WSAResetEvent(NetEvents[1]); // to be ready for the next data package
				if (WSAGetOverlappedResult(hClientSocket, &Overlapped, &nReceivedBytes, FALSE, &ReceiveFlags))
				{
					_tprintf(_T("%d bytes received\n"), nReceivedBytes);
					// Here should follow the processing of received data
					break;
				}
				else
				{	// Fatal problems
					_tprintf(_T("WSAGetOverlappedResult() failed, error %d\n"), GetLastError());
					goto out;
				}
			default: // Fatal problems
				_tprintf(_T("WSAWaitForMultipleEvents() failed, error %d\n"), WSAGetLastError());
				goto out;
			}
		}
		else
		{  // Returned immediately without socket error
			if (!nReceivedBytes)
			{  // When the receiving function has read nothing and returned immediately, the connection is off  
				_tprintf(_T("Server has closed the connection\n"));
				goto out;
			}
			else
			{
				_tprintf(_T("%d bytes received\n"), nReceivedBytes);
				// std::cout <<getString(ArrayInBuf);
						  // Here should follow the processing of received data
			}
		}
	}

out:
	WSACloseEvent(NetEvents[1]);
	return 0;
}

