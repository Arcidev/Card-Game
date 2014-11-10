#pragma once
#include <string>
#include "OpenSSL/aes.h"

class Aes
{
    public:
        static std::string encrypt(char const* data);
        static std::string decrypt(char const* data);
};
