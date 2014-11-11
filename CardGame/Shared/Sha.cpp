#include "Sha.h"
#include "OpenSSL/sha.h"

std::string Sha::CreateHash(std::string const& data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    char outputBuffer[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);

    outputBuffer[SHA256_DIGEST_LENGTH * 2] = '\0';
    return outputBuffer;
}
