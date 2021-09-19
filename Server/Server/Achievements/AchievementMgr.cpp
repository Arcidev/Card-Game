#include "AchievementMgr.h"
#include "../ConnectedUser.h"

DbAchievementMap const& AchievementManager::getAchievementMap()
{
    if (!m_achievementMap.empty())
        return m_achievementMap;

    std::lock_guard<std::mutex> lck(m_locker);
    if (!m_achievementMap.empty())
        return m_achievementMap;

    m_achievementMap = DatabaseInstance::GetDbCommandHandler().GetAchievements();
    return m_achievementMap;
}

AchievementMap AchievementManager::GetPlayerAchievements(uint32_t userId)
{
    DbAchievementMap const& achievements = getAchievementMap();
    DbUserCriteriaMap userCriterias = DatabaseInstance::GetDbCommandHandler().GetUserAchievementProgress(userId);
    AchievementMap playerAchievements;

    for (auto const& iter : achievements)
    {
        auto& inserted = playerAchievements.insert({ iter.first, Achievement(iter.first, iter.second.ParentId) });
        for (auto const& dbCriteria : iter.second.Criterias)
        {
            uint32_t progress = 0;
            time_t lastModified = 0;
            auto const& userCriteriaIter = userCriterias.find(dbCriteria.Id);
            if (userCriteriaIter != userCriterias.end())
            {
                progress = userCriteriaIter->second.Progress;
                lastModified = userCriteriaIter->second.LastModified;
            }

            AchievementCriteriaEntry entry(dbCriteria.Id, iter.first, (CriteriaTypes)dbCriteria.CriteriaType, dbCriteria.Requirement);
            AchievementCriteria criteria(&inserted.first->second, progress, lastModified, entry);
            inserted.first->second.GetCriterias().push_back(criteria);
        }

        inserted.first->second.CheckCriteriaMet();
    }

    return playerAchievements;
}
