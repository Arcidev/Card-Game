#include <algorithm>
#include "StaticHelper.h"

std::string StaticHelper::ToUpper(std::string_view str)
{
    std::string upper = std::string(str);
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

void StaticHelper::Trim(std::string& str)
{
    auto it1 = std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace(ch); });
    str.erase(str.begin(), it1);

    auto it2 = std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace(ch); });
    str.erase(it2.base(), str.end());
}
