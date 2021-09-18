#include <algorithm>
#include "Achievement.h"

Achievement::Achievement(uint32_t id, uint32_t parentId) : m_id(id), m_parentId(parentId), m_isCompleted(false) { }

void Achievement::CheckCriteriaMet()
{
    if (m_isCompleted)
        return;

    m_isCompleted = std::all_of(m_criterias.begin(), m_criterias.end(), [](AchievementCriteria const& criteria) { return criteria.IsMet(); });
}
