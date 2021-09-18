#pragma once
#include <list>
#include "AchievementCriteria.h"

class Achievement
{
    private:
        uint32_t m_id;
        uint32_t m_parentId;
        bool m_isCompleted;
        std::list<AchievementCriteria> m_criterias;

    public:
        Achievement(uint32_t id, uint32_t parentId);
        uint32_t GetId() const { return m_id; }
        uint32_t GetParentId() const { return m_parentId; }
        bool IsCompleted() const { return m_isCompleted; }
        std::list<AchievementCriteria>& GetCriterias() { return m_criterias; }
        std::list<AchievementCriteria> const& GetCriterias() const { return m_criterias; }
        void CheckCriteriaMet();
};
