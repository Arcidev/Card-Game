#pragma once
#include <vector>

class Aes
{
    public:
        static std::vector<uint8_t> Encrypt(std::vector<uint8_t> const& data, std::vector<uint8_t> const& key, std::vector<uint8_t> const& iVec);
        static std::vector<uint8_t> Decrypt(std::vector<uint8_t> const& data, std::vector<uint8_t> const& key, std::vector<uint8_t> const& iVec);
};
