#include <algorithm>
#include "Achievement.h"

void Achievement::checkCriteriaMet()
{
    if (m_isCompleted)
        return;

    m_isCompleted = std::all_of(m_criterias.begin(), m_criterias.end(), [](AchievementCriteria const& criteria) { return criteria.IsMet(); });
}
