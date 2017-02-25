#pragma once
#include <vector>
#include "OpenSSL/rsa.h"

class Rsa
{
    private:
        static RSA* createRSA(unsigned char* key, bool isPublic);

    public:
        static std::vector<uint8_t> Encrypt(std::vector<uint8_t> const& data, unsigned char* key, bool isPublic);
        static std::vector<uint8_t> Decrypt(std::vector<uint8_t> const& data, unsigned char* key, bool isPublic);
};
