#include <cstring>
#include <map>
#include <string>
#include "DbCommandHandler.h"

DbAchievementMap DbCommandHandler::GetAchievements() const
{
    PreparedStatement stmt("SELECT achievement_id, achievements.parent_id, achievement_criterias.id, criteria_type, requirement FROM achievement_criterias INNER JOIN achievements ON achievement_id = achievements.id;");
    DbAchievementMap achievementMap;

    dbHandler.ExecuteCommand(stmt, [&achievementMap](auto res)
    {
        int count = PQntuples(res);
        if (count < 1)
            return;

        Db::Achievement* achievementPtr;
        for (int i = 0; i < count; i++)
        {
            uint32_t achievementId = (uint32_t)strtoul(PQgetvalue(res, i, 0), nullptr, 0);
            auto const& achievementIter = achievementMap.find(achievementId);
            if (achievementIter == achievementMap.end())
            {
                uint32_t parentAchievement = PQgetisnull(res, i, 1) ? 0 : (uint32_t)strtoul(PQgetvalue(res, i, 1), nullptr, 0);
                std::list<Db::AchievementCriteria> criterias;
                Db::Achievement achievement =
                {
                    achievementId,
                    parentAchievement,
                    criterias
                };

                achievementPtr = &achievementMap.insert({ achievement.Id, achievement }).first->second;
            }
            else
                achievementPtr = &achievementIter->second;

            Db::AchievementCriteria criteria =
            {
                (uint32_t)strtoul(PQgetvalue(res, i, 2), nullptr, 0), // Criteria Id
                (uint8_t)strtoul(PQgetvalue(res, i, 3), nullptr, 0), // Criteria Type
                (uint32_t)strtoul(PQgetvalue(res, i, 4), nullptr, 0) // Requirement
            };

            achievementPtr->Criterias.push_back(criteria);
        }
    });

    return achievementMap;
}

DbUserCriteriaMap DbCommandHandler::GetUserAchievementProgress(uint32_t userId) const
{
    std::string idStr = std::to_string(userId);
    PreparedStatement stmt("SELECT criteria_id, progress, extract(epoch from last_modified)::bigint FROM user_achievement_criterias WHERE user_id = $1;");
    stmt.AddParameter(idStr);

    DbUserCriteriaMap userCriterias;
    dbHandler.ExecuteCommand(stmt, [&userCriterias](auto res)
    {
        int count = PQntuples(res);
        if (count < 1)
            return;

        for (int i = 0; i < count; i++)
        {
            Db::UserAchievementCriteria criteria =
            {
                (uint32_t)strtoul(PQgetvalue(res, i, 0), nullptr, 0), // Criteria Id
                (uint32_t)strtoul(PQgetvalue(res, i, 1), nullptr, 0), // Progress
                (time_t)strtoll(PQgetvalue(res, i, 2), nullptr, 0)    // Last Modified
            };

            userCriterias.insert({ criteria.CriteriaId, criteria });
        }
    });

    return userCriterias;
}

void DbCommandHandler::SetUserAchievementProgress(uint32_t userId, uint32_t criteriaId, uint32_t progress) const
{
    std::string userStr = std::to_string(userId);
    std::string criteriaStr = std::to_string(criteriaId);
    std::string progressStr = std::to_string(progress);
    PreparedStatement stmt("INSERT INTO user_achievement_criterias (user_id, criteria_id, progress) VALUES ($1, $2, $3) ON CONFLICT (user_id, criteria_id) DO UPDATE SET progress = $2, last_modified = CURRENT_TIMESTAMP;");
    stmt.AddParameter(userStr);
    stmt.AddParameter(criteriaStr);
    stmt.AddParameter(progressStr);

    dbHandler.ExecuteCommand(stmt, nullptr);
}
