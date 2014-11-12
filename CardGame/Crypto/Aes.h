#pragma once
#include <string>
#include <vector>

class Aes
{
    private:
        static std::vector<unsigned char> ivec;

    public:
        static std::string GenerateKey();
        static std::string Encrypt(std::string const& data, std::string const& key);
        static std::string Decrypt(std::string const& data, std::string const& key);
};
