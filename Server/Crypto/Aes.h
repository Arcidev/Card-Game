#pragma once
#include <string>
#include <vector>

class Aes
{
    public:
        static std::string Encrypt(std::string const& data, std::string const& key, std::vector<uint8_t> const& iVec);
        static std::string Decrypt(std::string const& data, std::string const& key, std::vector<uint8_t> const& iVec);
};
