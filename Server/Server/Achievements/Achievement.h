#pragma once
#include <list>
#include "AchievementCriteria.h"

class Achievement
{
    friend class AchievementCriteria;

    private:
        bool m_isCompleted;
        std::list<AchievementCriteria> m_criterias;
        void checkCriteriaMet();

    public:
        bool IsCompleted() const { return m_isCompleted; }
};
