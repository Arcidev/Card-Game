#include <cstdint>
#include <cstring>
#include "Aes.h"
#include "OpenSSL/aes.h"

#define AESKEY "yayayaIamLordeyayaya"
#define BYTE_SIZE 16
#define BIT_SIZE 128

std::string Aes::Encrypt(std::string const& data)
{
    if (data.empty())
        return "";

    std::string value;

    unsigned char output[BYTE_SIZE];
    AES_KEY AESkey;
    AES_set_encrypt_key((unsigned const char*)AESKEY, BIT_SIZE, &AESkey);
    for (uint32_t i = 0; i < (data.size() / BYTE_SIZE) + ((data.size() % BYTE_SIZE) ? 1 : 0); i++)
    {
        AES_encrypt((unsigned const char*)data.c_str() + (BYTE_SIZE * i), output, &AESkey);
        value.resize(value.size() + BYTE_SIZE);
        std::memcpy(&value[BYTE_SIZE * i], output, BYTE_SIZE);
    }
    
    return value;
}

std::string Aes::Decrypt(std::string const& data)
{
    if (data.empty())
        return "";

    std::string value;

    unsigned char output[BYTE_SIZE + 1];
    AES_KEY AESkey;
    AES_set_decrypt_key((unsigned const char*)AESKEY, BIT_SIZE, &AESkey);

    for (uint32_t i = 0; i < data.size() / BYTE_SIZE; i++)
    {
        AES_decrypt((unsigned const char*)data.c_str() + (16 * i), output, &AESkey);
        output[16] = '\0';
        value += (char*)output;
    }

    return value;
}
