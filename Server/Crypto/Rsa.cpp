#include <cstdint>
#include <cstring>
#include <iostream>
#include "Rsa.h"
#include "OpenSSL/pem.h"

#define PADDING RSA_PKCS1_PADDING

// Creates RSA key
RSA* Rsa::createRSA(unsigned char* key, bool isPublic)
{
    RSA* rsa = nullptr;
    BIO* keybio = BIO_new_mem_buf(key, -1);
    if (!keybio)
    {
        std::cerr << "Failed to create key BIO" << std::endl;
        return nullptr;
    }

    rsa = isPublic ? PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL) : PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    if (!rsa)
        std::cerr << "Failed to create RSA" << std::endl;

    return rsa;
}

// Encrypts data with RSA algorithm
std::vector<uint8_t> Rsa::Encrypt(std::vector<uint8_t> const& data, unsigned char* key, bool isPublic)
{
    if (data.empty())
        return data;

    uint8_t encrypted[4098];
    RSA* rsa = createRSA(key, isPublic);
    if (!rsa)
        return data;

    int(*encryptFnc)(int, unsigned char const*, unsigned char*, RSA*, int) = isPublic ? RSA_public_encrypt : RSA_private_encrypt;

    int encryptedLength = encryptFnc(data.size(), data.data(), encrypted, rsa, PADDING);
    if (encryptedLength == -1)
        return data;

    std::vector<uint8_t> value;
    value.resize(encryptedLength);
    std::memcpy(&value[0], encrypted, encryptedLength);
    
    return value;
}

// Decrypts data ecnrypted by RSA algorithm
std::vector<uint8_t> Rsa::Decrypt(std::vector<uint8_t> const& data, unsigned char* key, bool isPublic)
{
    if (data.empty())
        return data;

    uint8_t decrypted[4098];
    RSA* rsa = createRSA(key, isPublic);
    if (!rsa)
        return data;

    int(*decryptFnc)(int, unsigned char const*, unsigned char*, RSA*, int) = isPublic ? RSA_public_decrypt : RSA_private_decrypt;

    int decryptedLength = decryptFnc(data.size(), data.data(), decrypted, rsa, PADDING);
    if (decryptedLength == -1)
        return data;

    std::vector<uint8_t> value;
    value.resize(decryptedLength);
    std::memcpy(&value[0], decrypted, decryptedLength);

    return value;
}
