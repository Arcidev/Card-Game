#include "Aes.h"

#define AESKEY "yayayaIamLordeyayaya"

std::string Aes::encrypt(char const* data)
{
    unsigned char output[17];
    AES_KEY AESkey;
    AES_set_encrypt_key((unsigned const char*)AESKEY, 128, &AESkey);
    AES_encrypt((unsigned const char*)data, output, &AESkey);
    output[16] = '\0';
    return (char*)output;
}

std::string Aes::decrypt(char const* data)
{
    unsigned char output[17];
    AES_KEY AESkey;
    AES_set_decrypt_key((unsigned const char*)AESKEY, 128, &AESkey);
    AES_decrypt((unsigned const char*)data, output, &AESkey);
    return (char*)output;
}
