#include <cstring>
#include <map>
#include <sstream>
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

void DbCommandHandler::SetUserAchievementProgress(std::map<std::tuple<uint32_t, uint32_t>, uint32_t> const& userAchievements) const
{
    if (userAchievements.empty())
        return;

    std::stringstream ss;
    ss << "INSERT INTO user_achievement_criterias(user_id, criteria_id, progress) VALUES ";

    for (auto const& item : userAchievements)
        ss << '(' << std::get<0>(item.first) << ", " << std::get<1>(item.first) << ", " << item.second << "),";

    ss.seekp(-1, std::ios_base::end);
    ss << " ON CONFLICT (user_id, criteria_id) DO UPDATE SET progress = excluded.progress, last_modified = CURRENT_TIMESTAMP;";

    PreparedStatement stmt(ss.str());
    dbHandler.ExecuteCommand(stmt, nullptr);
}
