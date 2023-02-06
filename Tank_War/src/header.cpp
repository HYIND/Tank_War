#include "header.h"
#include <locale>
#include <codecvt>

bool isstart = false;
bool isready = false;
bool host = false;
bool isonline_game = false;

//auto logger;

string wstring2string(wstring wstr)
{
	string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	if (len <= 0)return result;
	char* buffer = new char[len + 1];
	if (buffer == NULL)return result;
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}
wstring string2wstring(string str)
{
	wstring result;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	if (len < 0)return result;
	wchar_t* buffer = new wchar_t[len + 1];
	if (buffer == NULL)return result;
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

string wstring2utf8(const std::wstring& str)
{
	wstring_convert<codecvt_utf8<wchar_t> > strCnv;
	return strCnv.to_bytes(str);
}

wstring utf82wstring(const std::string& str)
{
	wstring_convert<codecvt_utf8<wchar_t> > strCnv;
	return strCnv.from_bytes(str);
}

SOCKET get_new_socket(std::string IP, uint16_t socket_port, int protocol, sockaddr_in& addr)
{

	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(socket_port);
	if (IP != "")
		inet_pton(AF_INET, IP.c_str(), &(addr.sin_addr.s_addr));
	else
		addr.sin_addr.s_addr = INADDR_ANY;

	return WSASocket(addr.sin_family, protocol, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

}