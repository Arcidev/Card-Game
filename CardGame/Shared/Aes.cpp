#include <cstdint>
#include "Aes.h"

#define AESKEY "yayayaIamLordeyayaya"

std::string Aes::encrypt(char const* data)
{
    uint32_t length = strlen(data);
    if (!length)
        return "";

    std::string value;

    unsigned char output[17];
    AES_KEY AESkey;
    AES_set_encrypt_key((unsigned const char*)AESKEY, 128, &AESkey);
    for (uint32_t i = 0; i < (length / 16) + ((length % 16) ? 1 : 0); i++)
    {
        AES_encrypt((unsigned const char*)data + (16 * i), output, &AESkey);
        output[16] = '\0';
        value += (char*)output;
    }
    
    return value;
}

std::string Aes::decrypt(char const* data)
{
    uint32_t length = strlen(data);
    if (!length)
        return "";

    std::string value;

    unsigned char output[17];
    AES_KEY AESkey;
    AES_set_decrypt_key((unsigned const char*)AESKEY, 128, &AESkey);
    for (uint32_t i = 0; i < length / 16; i++)
    {
        AES_decrypt((unsigned const char*)data + (16 * i), output, &AESkey);
        output[16] = '\0';
        value += (char*)output;
    }

    return value;
}
