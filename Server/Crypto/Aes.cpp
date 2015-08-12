#include <cstdint>
#include <cstring>
#include "Aes.h"
#include "OpenSSL/aes.h"

#define BIT_SIZE 128

// Encrypts data with AES algorithm
std::string Aes::Encrypt(std::string const& data, std::string const& key, std::vector<uint8_t> const& iVec)
{
    if (data.empty() || key.empty())
        return data;

    unsigned char encryptionIvec[AES_BLOCK_SIZE];
    std::memcpy(encryptionIvec, &iVec[0], AES_BLOCK_SIZE);

    AES_KEY AESkey;
    AES_set_encrypt_key((unsigned const char*)key.c_str(), BIT_SIZE, &AESkey);
    unsigned char buffer[AES_BLOCK_SIZE];
    std::string value;

    for (uint32_t i = 0; i < data.size(); i += AES_BLOCK_SIZE)
    {
        AES_cbc_encrypt((unsigned const char*)data.c_str() + i, buffer, ((i + AES_BLOCK_SIZE) < data.size()) ? AES_BLOCK_SIZE : (data.size() - i), &AESkey, encryptionIvec, AES_ENCRYPT);
        value.resize(value.size() + AES_BLOCK_SIZE);
        std::memcpy(&value[i], buffer, AES_BLOCK_SIZE);
    }

    return value;
}

// Decrypts data ecnrypted by AES algorithm
std::string Aes::Decrypt(std::string const& data, std::string const& key, std::vector<uint8_t> const& iVec)
{
    if (data.empty() || key.empty())
        return data;

    unsigned char decryptionIvec[AES_BLOCK_SIZE];
    std::memcpy(decryptionIvec, &iVec[0], AES_BLOCK_SIZE);

    AES_KEY AESkey;
    AES_set_decrypt_key((unsigned const char*)key.c_str(), BIT_SIZE, &AESkey);
    unsigned char buffer[AES_BLOCK_SIZE];
    std::string value;

    for (uint32_t i = 0; i < data.size(); i += AES_BLOCK_SIZE)
    {
        AES_cbc_encrypt((unsigned const char*)data.c_str() + i, buffer, AES_BLOCK_SIZE, &AESkey, decryptionIvec, AES_DECRYPT);
        value.resize(value.size() + AES_BLOCK_SIZE);
        std::memcpy(&value[i], buffer, AES_BLOCK_SIZE);
    }

    return value;
}
