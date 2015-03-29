#include "StaticHelper.h"

bool StaticHelper::CompareStringCaseInsensitive(std::string const& str1, std::string const& str2)
{
    if (str1.length() != str2.length())
        return false;

    for (unsigned int i = 0; i < str1.length(); i++)
        if (tolower(str1[i]) != tolower(str2[i]))
            return false;

    return true;
}