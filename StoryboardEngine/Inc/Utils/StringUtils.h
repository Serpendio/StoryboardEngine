#pragma once
#include <string>

namespace StoryboardEngine::StringUtils
{
    /*template <typename T>
    std::string to_narrow_string(T&& t)
    {
        if constexpr (std::is_same_v<std::decay_t<T>, const wchar_t*> || std::is_same_v<std::decay_t<T>, wchar_t*>)
        {
            std::wstring ws(t);
            return std::string(ws.begin(), ws.end());
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::wstring>)
        {
            return std::string(t.begin(), t.end());
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
        {
            return std::string(std::forward<T>(t));
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, char*> || std::is_same_v<std::decay_t<T>, const char*>)
        {
            return std::string(t);
        }
        else
        {
            return std::to_string(t);
        }
    }

    template <typename T>
    std::wstring to_wide_string(T&& t)
    {
        if constexpr (std::is_same_v<std::decay_t<T>, const char*> || std::is_same_v<std::decay_t<T>, char*>)
        {
            std::string s(t);
            return std::wstring(s.begin(), s.end());
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
        {
            return std::wstring(t.begin(), t.end());
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, std::wstring>)
        {
            return std::wstring(std::forward<T>(t));
        }
        else if constexpr (std::is_same_v<std::decay_t<T>, wchar_t*> || std::is_same_v<std::decay_t<T>, const wchar_t*>)
        {
            return std::wstring(t);
        }
        else
        {
            return std::to_wstring(t);
        }
    }*/

	std::string ToString(const wchar_t* wstr);
	std::string ToString(const char* str);
    std::string ToString(const std::wstring& wstr);
	std::string ToString(const std::string& str);
	std::string ToString(std::wstring_view wstrView);
	std::string ToString(std::string_view strView);
	template <typename T>
    std::string ToString(const T& value)
    {
        // ToDo: Look into static asserts for unsupported types (e.g. filesystem::path)

        return std::to_string(value);
	}

	std::wstring ToWString(const wchar_t* wstr);
	std::wstring ToWString(const char* str);
	std::wstring ToWString(const std::wstring& wstr);
	std::wstring ToWString(const std::string& str);
	std::wstring ToWString(std::wstring_view wstrView);
	std::wstring ToWString(std::string_view strView);
    template <typename T>
    std::wstring ToWString(const T& value)
    {
		// ToDo: Look into static asserts for unsupported types (e.g. filesystem::path)

        return std::to_wstring(value);
    }
}