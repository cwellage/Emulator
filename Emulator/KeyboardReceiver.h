#pragma once
#if defined(UNICODE) || defined(_UNICODE)
#define _tcout std::wcout
#define _tcin std::wcin
#else
#define _tcout std::cout
#define _tcout std::cin
#endif 
using namespace std;
typedef std::basic_string<TCHAR> _tstring;

class KeyboardReceiver
{
public:	void ReadKeyboard(SOCKET socket);

private: int ProcessCommandAndSend(_tstring command);
		 std::string getUnicodeString(char array[]);
		 std::string getOutputMeasurementResult(char array[]);
		 void InputFromKeyBoard(_tstring& sCommand, int& iResult);

};