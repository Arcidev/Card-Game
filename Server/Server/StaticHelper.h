#pragma once
#include <string>

class StaticHelper
{
    public:
        static std::string ToUpper(std::string_view str);
        static void Trim(std::string& str);
};
