#pragma once
#include <list>
#include <map>
#include <mutex>
#include "Achievement.h"

typedef std::map<uint32_t, AchievementCriteriaEntry> AchievementMap;

class AchievementManager
{
    private:
        std::mutex m_locker;
        AchievementMap m_achievementMap;
        AchievementMap const& getAchievementMap();

    public:
        std::list<Achievement> GetPlayerAchievements(uint32_t userId);
};
