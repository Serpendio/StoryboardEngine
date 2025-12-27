#include "pch.h"
#include "Utils/StringUtils.h"
#include <windows.h>

std::string StoryboardEngine::StringUtils::ToString(const wchar_t* wstr)
{
	if (!wstr || *wstr == L'\0')
		return "";

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (size_needed <= 0)
		return "";

	std::string utf8Str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8Str.data(), size_needed, NULL, NULL);
	return utf8Str;
}

std::string StoryboardEngine::StringUtils::ToString(const char* str)
{
	if (!str) return std::string();

	return std::string(str);
}

std::string StoryboardEngine::StringUtils::ToString(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();

	return ToString(wstr.c_str());
}

// Purely for Logger convenience
std::string StoryboardEngine::StringUtils::ToString(const std::string& str)
{
	return str;
}

std::string StoryboardEngine::StringUtils::ToString(std::wstring_view wstrView)
{
	if (wstrView.empty())
		return std::string();
	
	return ToString(std::wstring(wstrView));
}

std::string StoryboardEngine::StringUtils::ToString(std::string_view strView)
{
	if (strView.empty())
		return std::string();

	return std::string(strView);
}



std::wstring StoryboardEngine::StringUtils::ToWString(const char* str)
{
	if (!str || *str == '\0')
		return L"";

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	if (size_needed <= 0)
		return L"";

	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr.data(), size_needed);
	return wstr;
}

std::wstring StoryboardEngine::StringUtils::ToWString(const wchar_t* wstr)
{
	if (!wstr) return std::wstring();

	return std::wstring(wstr);
}

std::wstring StoryboardEngine::StringUtils::ToWString(const std::wstring& wstr)
{
	return wstr;
}

std::wstring StoryboardEngine::StringUtils::ToWString(const std::string& str)
{
	if (str.empty()) return std::wstring();

	return ToWString(str.c_str());
}

std::wstring StoryboardEngine::StringUtils::ToWString(std::wstring_view wstrView)
{
	if (wstrView.empty())
		return std::wstring();

	return std::wstring(wstrView);
}

std::wstring StoryboardEngine::StringUtils::ToWString(std::string_view strView)
{
	if (strView.empty())
		return std::wstring();

	return ToWString(std::string(strView));
}
