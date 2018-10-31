#include "Sha.h"
#include "OpenSSL/sha.h"

constexpr int bufferLength = SHA256_DIGEST_LENGTH * 2 + 1;

// Creates hash from data
std::string Sha::CreateHash(std::string_view data)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(hash, &sha256);

    char outputBuffer[bufferLength];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        snprintf(outputBuffer + (i * 2), bufferLength - (i * 2), "%02x", hash[i]);

    outputBuffer[SHA256_DIGEST_LENGTH * 2] = '\0';
    return outputBuffer;
}
