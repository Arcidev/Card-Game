#pragma once
#include <string_view>

struct User
{
    int Id;
    std::string_view Email;
    std::string_view UserName;
    std::string_view PasswordSalt;
    std::string_view PasswordHash;
    uint8_t UserRole;

    User()
    {
        Id = 0;
        UserRole = 0;
    }
};
