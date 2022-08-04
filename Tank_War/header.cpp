#include "header.h"
#include <locale>
#include <codecvt>

bool isstart = false;
bool isready = false;
bool host = false;
bool isonline_game = false;

//wstring utf82wstring(string str)
//{
//	LPCSTR pszSrc = str.c_str();
//	int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
//	if (nLen == 0)
//		return std::wstring(L"");
//
//	wchar_t* pwszDst = new wchar_t[nLen];
//	if (!pwszDst)
//		return std::wstring(L"");
//
//	MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
//	std::wstring wstr(pwszDst);
//	delete[] pwszDst;
//	pwszDst = NULL;
//
//	return wstr;
//}
//
//string wstring2utf8(wstring wstr)
//{
//	LPCWSTR pwszSrc = wstr.c_str();
//	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
//	if (nLen == 0)
//		return std::string("");
//
//	char* pszDst = new char[nLen];
//	if (!pszDst)
//		return std::string("");
//
//	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
//	std::string str(pszDst);
//	delete[] pszDst;
//	pszDst = NULL;
//
//	return str;
//}

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