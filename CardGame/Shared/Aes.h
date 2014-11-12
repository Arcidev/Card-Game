#pragma once
#include <string>
#include <vector>

class Aes
{
    private:
        static std::vector<unsigned char> ivec;

    public:
        static std::string Encrypt(std::string const& data);
        static std::string Decrypt(std::string const& data);
};
