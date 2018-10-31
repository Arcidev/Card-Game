#pragma once
#include <string>

class Sha
{
    public:
        static std::string CreateHash(std::string_view data);
};
