#pragma once
#include <string>

class Sha
{
    private:
        static std::string convertToHexString(uint8_t const* data, uint8_t length);
        static std::string createHash(std::string_view data, uint8_t const* salt, uint8_t saltLength);

    public:
        static std::tuple<std::string, std::string> CreateHash(std::string_view data);
        static std::string CreateHash(std::string_view data, std::string_view hexSalt);
};
