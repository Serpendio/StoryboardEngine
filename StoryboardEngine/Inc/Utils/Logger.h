#pragma once
#include <iostream>
#include <sstream>
#include "Utils\StringUtils.h"

namespace StoryboardEngine::Logger
{
	// ToDo: Is it possible to link back into IDE to see the line that logged the error/warning/info? (Like Unity does)

    void CheckDebugBreak();

    template <typename... Args>
    void LogInfo(Args... args)
    {
        std::wostringstream oss;
        oss << L"Info:";

        ((oss << L" " << StringUtils::ToWString(args)), ...);

        std::wclog << oss.str() << std::endl;
    }

    template <typename... Args>
    void LogWarning(Args... args)
    {
        std::wostringstream oss;
        oss << L"Warning:";

        ((oss << L" " << StringUtils::ToWString(args)), ...);

        std::wclog << oss.str() << std::endl;

        CheckDebugBreak();
    }

	// ToDo: Allow custom exception types

    template <typename... Args>
    void LogError(Args... args)
    {
        std::wostringstream oss;
        oss << L"ERROR:";

        ((oss << L" " << StringUtils::ToWString(args)), ...);

        std::wcerr << oss.str() << std::endl;

        CheckDebugBreak();
    }
};

