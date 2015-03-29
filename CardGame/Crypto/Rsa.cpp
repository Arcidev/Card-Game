#include <cstdint>
#include <cstring>
#include "Rsa.h"
#include "OpenSSL/pem.h"

#define PADDING RSA_PKCS1_PADDING

RSA* Rsa::createRSA(unsigned char* key, bool isPublic)
{
    RSA* rsa = nullptr;
    BIO* keybio = BIO_new_mem_buf(key, -1);
    if (!keybio)
    {
        printf("Failed to create key BIO");
        return nullptr;
    }

    rsa = isPublic ? PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL) : PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

    if (!rsa)
        printf("Failed to create RSA");

    return rsa;
}

std::string Rsa::Encrypt(std::string const& data, unsigned char* key, bool isPublic)
{
    if (data.empty())
        return "";

    char encrypted[4098];
    RSA* rsa = createRSA(key, isPublic);
    if (!rsa)
        return "";

    int(*encryptFnc)(int, unsigned char const*, unsigned char*, RSA*, int) = isPublic ? RSA_public_encrypt : RSA_private_encrypt;

    int encryptedLength = encryptFnc(data.length(), (unsigned char*)&data[0], (unsigned char*)encrypted, rsa, PADDING);
    if (encryptedLength == -1)
        return "";

    std::string value;
    value.resize(encryptedLength);
    std::memcpy(&value[0], encrypted, encryptedLength);
    
    return value;
}

std::string Rsa::Decrypt(std::string const& data, unsigned char* key, bool isPublic)
{
    if (data.empty())
        return "";

    char decrypted[4098];
    RSA* rsa = createRSA(key, isPublic);
    if (!rsa)
        return "";

    int(*decryptFnc)(int, unsigned char const*, unsigned char*, RSA*, int) = isPublic ? RSA_public_decrypt : RSA_private_decrypt;

    int decryptedLength = decryptFnc(data.length(), (unsigned char*)&data[0], (unsigned char*)decrypted, rsa, PADDING);
    if (decryptedLength == -1)
        return "";

    std::string value;
    value.resize(decryptedLength);
    std::memcpy(&value[0], decrypted, decryptedLength);

    return value;
}
