#include <cstdint>
#include <cstring>
#include "Aes.h"
#include "OpenSSL/evp.h"

#define BIT_SIZE 128

// Encrypts data with AES algorithm
std::string Aes::Encrypt(std::string const& data, std::string const& key, std::vector<uint8_t> const& iVec)
{
    if (data.empty() || key.empty())
        return data;

    std::vector<uint8_t> outdata(data.length() + 16);
    int outLen = 0, outLen2 = 0;
    EVP_CIPHER_CTX ctx;
    EVP_EncryptInit(&ctx, EVP_aes_256_cbc(), (uint8_t const*)key.c_str(), &iVec[0]);
    EVP_EncryptUpdate(&ctx, &outdata[0], &outLen, (uint8_t const*)data.c_str(), data.length());
    EVP_EncryptFinal(&ctx, &outdata[outLen], &outLen2);
    std::string value;
    value.resize(outLen + outLen2);
    std::memcpy(&value[0], &outdata[0], outLen + outLen2);

    EVP_CIPHER_CTX_cleanup(&ctx);

    return value;
}

// Decrypts data ecnrypted by AES algorithm
std::string Aes::Decrypt(std::string const& data, std::string const& key, std::vector<uint8_t> const& iVec)
{
    if (data.empty() || key.empty())
        return data;

    std::vector<uint8_t> outdata(data.length());
    int outLen = 0, outLen2 = 0;

    EVP_CIPHER_CTX ctx;
    EVP_DecryptInit(&ctx, EVP_aes_256_cbc(), (uint8_t const*)key.c_str(), &iVec[0]);
    EVP_DecryptUpdate(&ctx, &outdata[0], &outLen, (uint8_t const*)data.c_str(), data.length());
    EVP_DecryptFinal(&ctx, &outdata[outLen], &outLen2);

    std::string value;
    value.resize(outLen + outLen2);
    std::memcpy(&value[0], &outdata[0], outLen + outLen2);

    EVP_CIPHER_CTX_cleanup(&ctx);

    return value;
}
