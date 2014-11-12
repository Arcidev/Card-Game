#pragma once
#include <string>
#include "OpenSSL/rsa.h"

class Rsa
{
    private:
        static RSA* createRSA(unsigned char* key, bool isPublic);

    public:
        static std::string Encrypt(std::string& data, unsigned char* key, bool isPublic);
        static std::string Decrypt(std::string& data, unsigned char* key, bool isPublic);
};