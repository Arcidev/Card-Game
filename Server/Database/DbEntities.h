#pragma once
#include <string>
#include <list>

namespace Db
{
    struct User
    {
        uint32_t Id;
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

    struct AchievementCriteria
    {
        uint32_t Id;
        uint8_t CriteriaType;
        uint32_t Requirement;
    };

    struct Achievement
    {
        uint32_t Id;
        uint32_t ParentId;
        std::list<AchievementCriteria> Criterias;
    };

    struct UserAchievementCriteria
    {
        uint32_t CriteriaId;
        uint32_t Progress;
    };
}
