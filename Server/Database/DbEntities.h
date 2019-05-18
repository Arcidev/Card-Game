#pragma once
#include <string>

struct User
{
    int Id;
    std::string Email;
    std::string UserName;
    std::string PasswordSalt;
    std::string PasswordHash;
    uint8_t UserRole;

    User()
    {
        Id = 0;
        UserRole = 0;
    }
};
