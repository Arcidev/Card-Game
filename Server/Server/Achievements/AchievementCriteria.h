#pragma once

enum class CriteriaType
{

};

class Achievement;

class AchievementCriteria
{
    private:
        bool m_isMet;
        int m_progress;
        Achievement* m_achievement;

    public:
        AchievementCriteria(Achievement* achievement);

        bool IsMet() const { return m_isMet; }
        void CheckCriteriaMet();
};
