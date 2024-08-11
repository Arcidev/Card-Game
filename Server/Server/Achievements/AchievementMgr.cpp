#include <chrono>
#include "AchievementMgr.h"
#include "../../Shared/SharedDefines.h"

AchievementManager::AchievementManager() : m_userAchievementProgressThread(&AchievementManager::updateUserAchievementProgress, this), m_stopUpdateThread(false) { }

AchievementManager::~AchievementManager()
{
    DEBUG_LOG("Stopping achievement update process\r\n");
    m_stopUpdateThread = true;
    m_userAchievementProgressThread.join();
}

DbAchievementMap const& AchievementManager::getAchievementMap()
{
    if (!m_achievementMap.empty())
        return m_achievementMap;

    std::scoped_lock <std::mutex> lck(m_locker);
    if (!m_achievementMap.empty())
        return m_achievementMap;

    m_achievementMap = DatabaseInstance::GetDbCommandHandler().GetAchievements();
    return m_achievementMap;
}

void AchievementManager::updateUserAchievementProgress()
{
    DEBUG_LOG("Achievement update process started\r\n");
    while (!m_stopUpdateThread)
    {
        if (m_userAchievementProgress.empty())
        {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            continue;
        }

        m_locker.lock();
        auto mapCopy = UserAchievementProgress(m_userAchievementProgress);
        m_userAchievementProgress.clear();
        m_locker.unlock();

        for (auto const& item : mapCopy)
            DatabaseInstance::GetDbCommandHandler().SetUserAchievementProgress(std::get<0>(item.first), std::get<1>(item.first), item.second);
    }
    DEBUG_LOG("Achievement update process ended\r\n");
}

AchievementMap AchievementManager::GetPlayerAchievements(uint32_t userId)
{
    DbAchievementMap const& achievements = getAchievementMap();
    DbUserCriteriaMap userCriterias = DatabaseInstance::GetDbCommandHandler().GetUserAchievementProgress(userId);
    AchievementMap playerAchievements;

    for (auto const& iter : achievements)
    {
        auto const& inserted = playerAchievements.insert({ iter.first, Achievement(iter.first, iter.second.ParentId) });
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

void AchievementManager::SetAchievementProgress(uint32_t userId, uint32_t criteriaId, uint32_t progress)
{
    std::scoped_lock <std::mutex> lck(m_locker);
    m_userAchievementProgress[{userId, criteriaId}] = progress;
}
