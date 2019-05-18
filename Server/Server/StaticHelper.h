#include <string>

class StaticHelper
{
    public:
        static bool CompareStringCaseInsensitive(std::string_view str1, std::string_view str2);
        static void Trim(std::string& str);
};
