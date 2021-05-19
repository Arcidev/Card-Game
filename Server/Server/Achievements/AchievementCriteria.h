#pragma once
#include "AchievementCriteriaEntry.h"

class Achievement;

class AchievementCriteria : AchievementCriteriaEntry
{
    private:
        bool m_isMet;
        uint32_t m_progress;
        Achievement* m_achievement;

        void checkCriteriaMet();
        void setIsMet() { m_isMet = m_progress >= m_requirement; }

    public:
        AchievementCriteria(Achievement* achievement, uint32_t progress, AchievementCriteriaEntry const& entry);

        bool IsMet() const { return m_isMet; }
        void SetProgress(int progress);
        uint32_t GetProgress() const { return m_progress; }
};
