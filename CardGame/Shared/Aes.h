#pragma once
#include <string>

class Aes
{
    public:
        static std::string Encrypt(std::string const& data);
        static std::string Decrypt(std::string const& data);
};
