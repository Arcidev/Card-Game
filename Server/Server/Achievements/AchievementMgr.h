#pragma once
#include <list>
#include <mutex>
#include <thread>
#include "Achievement.h"
#include "../../Database/DatabaseInstance.h"

typedef std::map<uint32_t, Achievement> AchievementMap;

class AchievementManager
{
    private:
        typedef std::map<std::tuple<uint32_t, uint32_t>, uint32_t> UserAchievementProgress;

        std::mutex m_locker;
        UserAchievementProgress m_userAchievementProgress;
        DbAchievementMap m_achievementMap;
        std::thread m_userAchievementProgressThread;
        bool m_stopUpdateThread;

        DbAchievementMap const& getAchievementMap();
        void updateUserAchievementProgress();

    public:
        AchievementManager();
        ~AchievementManager();
        AchievementMap GetPlayerAchievements(uint32_t userId);
        void SetAchievementProgress(uint32_t userId, uint32_t criteriaId, uint32_t progress);
};
