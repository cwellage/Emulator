#pragma once
class CommandSender
{
public :unsigned int __stdcall SendNet(char data[]);
		int  SendToServer(char data[], int byteLength, SOCKET ConnectSocket);
};