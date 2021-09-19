#include <algorithm>
#include "Achievement.h"

Achievement::Achievement(uint32_t id, uint32_t parentId) : m_id(id), m_parentId(parentId), m_isCompleted(false) { }

time_t Achievement::GetCompletionDate() const
{
    if (!m_isCompleted)
        return 0;

    auto const& iter = std::max_element(m_criterias.begin(), m_criterias.end(), [](AchievementCriteria const& elem1, AchievementCriteria const& elem2) { return elem1.GetLastModified() > elem2.GetLastModified(); });
    return iter == m_criterias.end() ? 0 : iter->GetLastModified();
}

void Achievement::CheckCriteriaMet()
{
    if (m_isCompleted)
        return;

    m_isCompleted = std::all_of(m_criterias.begin(), m_criterias.end(), [](AchievementCriteria const& criteria) { return criteria.IsMet(); });
}
