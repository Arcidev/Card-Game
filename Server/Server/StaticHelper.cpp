#include <algorithm>
#include <cctype>
#include "StaticHelper.h"

// Compares 2 strings
bool StaticHelper::CompareStringCaseInsensitive(std::string_view str1, std::string_view str2)
{
    if (str1.length() != str2.length())
        return false;

    for (size_t i = 0; i < str1.length(); i++)
        if (tolower(str1[i]) != tolower(str2[i]))
            return false;

    return true;
}

void StaticHelper::Trim(std::string& str)
{
    auto it1 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace(ch); });
    str.erase(str.begin(), it1);

    auto it2 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace(ch); });
    str.erase(it2.base(), str.end());
}
