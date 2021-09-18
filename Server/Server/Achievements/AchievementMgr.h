#pragma once
#include <list>
#include <mutex>
#include "Achievement.h"
#include "../../Database/DatabaseInstance.h"

typedef std::map<uint32_t, Achievement> AchievementMap;
class ConnectedUser;

class AchievementManager
{
    private:
        std::mutex m_locker;
        DbAchievementMap m_achievementMap;
        DbAchievementMap const& getAchievementMap();

    public:
        AchievementMap GetPlayerAchievements(uint32_t userId);
};
