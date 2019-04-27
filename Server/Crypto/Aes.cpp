#include <cstdint>
#include "Aes.h"
#include "OpenSSL/evp.h"

// Encrypts data with AES algorithm
std::vector<uint8_t> Aes::Encrypt(std::vector<uint8_t> const& data, std::vector<uint8_t> const& key, std::vector<uint8_t> const& iVec)
{
    if (data.empty() || key.empty())
        return data;

    std::vector<uint8_t> outdata(data.size() + 16);
    int outLen = 0, outLen2 = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit(ctx, EVP_aes_256_cbc(), key.data(), iVec.data());
    EVP_EncryptUpdate(ctx, outdata.data(), &outLen, data.data(), (int)data.size());
    EVP_EncryptFinal(ctx, &outdata[outLen], &outLen2);
    EVP_CIPHER_CTX_free(ctx);

    outdata.resize(outLen + outLen2);
    return outdata;
}

// Decrypts data ecnrypted by AES algorithm
std::vector<uint8_t> Aes::Decrypt(std::vector<uint8_t> const& data, std::vector<uint8_t> const& key, std::vector<uint8_t> const& iVec)
{
    if (data.empty() || key.empty())
        return std::vector<uint8_t>(data.begin(), data.end());

    std::vector<uint8_t> outdata(data.size());
    int outLen = 0, outLen2 = 0;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key.data(), iVec.data());
    EVP_DecryptUpdate(ctx, outdata.data(), &outLen, data.data(), (int)data.size());
    EVP_DecryptFinal(ctx, &outdata[outLen], &outLen2);
    EVP_CIPHER_CTX_free(ctx);

    outdata.resize(outLen + outLen2);
    return outdata;
}
 