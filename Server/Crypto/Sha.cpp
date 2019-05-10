#include <cstdint>
#include <vector>
#include "Sha.h"
#include "OpenSSL/rand.h"
#include "OpenSSL/sha.h"

constexpr int bufferLength = SHA256_DIGEST_LENGTH * 2 + 1;
constexpr int saltBufferLength = 16;

// Creates hash from data
std::tuple<std::string, std::string> Sha::CreateHash(std::string_view data)
{
    uint8_t saltBuffer[saltBufferLength];
    RAND_bytes(saltBuffer, saltBufferLength);

    return { convertToHexString(saltBuffer, saltBufferLength), createHash(data, saltBuffer, saltBufferLength) };
}

std::string Sha::CreateHash(std::string_view data, std::string_view hexSalt)
{
    std::vector<uint8_t> bytes(hexSalt.size() / 2);
    bytes.resize(hexSalt.size() / 2);

    int value;
    for (int i = 0; i < bytes.size(); i++)
    {
        sscanf_s(hexSalt.data() + i * 2, "%02X", &value);
        bytes[i] = (uint8_t)value;
    }

    return createHash(data, &bytes[0], (uint8_t)bytes.size());
}

std::string Sha::createHash(std::string_view data, uint8_t const* salt, uint8_t saltLength)
{
    std::vector<uint8_t> saltedPassword(data.size() + saltLength);
    saltedPassword.resize(data.size() + saltLength);

    std::memcpy(&saltedPassword[0], salt, saltLength);
    std::memcpy(&saltedPassword[saltLength], data.data(), data.size());

    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, &saltedPassword[0], saltedPassword.size());
    SHA256_Final(hash, &sha256);

    return convertToHexString(hash, SHA256_DIGEST_LENGTH);
}

std::string Sha::convertToHexString(uint8_t const* data, uint8_t length)
{
    char outputBuffer[bufferLength];
    for (int i = 0; i < length; i++)
        snprintf(outputBuffer + (i * 2), bufferLength - (i * 2), "%02x", data[i]);

    outputBuffer[length * 2] = '\0';
    return outputBuffer;
}
