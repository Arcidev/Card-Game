#include "AchievementMgr.h"

AchievementMap const& AchievementManager::getAchievementMap()
{
    if (!m_achievementMap.empty())
        return m_achievementMap;

    std::lock_guard<std::mutex> lck(m_locker);
    if (!m_achievementMap.empty())
        return m_achievementMap;

    return m_achievementMap;
}

std::list<Achievement> AchievementManager::GetPlayerAchievements(uint32_t userId)
{
    AchievementMap const& achievements = getAchievementMap();
    return std::list<Achievement>();
}
