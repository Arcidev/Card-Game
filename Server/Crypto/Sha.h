#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>

class Sha
{
    public:
        static std::string CreateHash(std::string_view data);
};
