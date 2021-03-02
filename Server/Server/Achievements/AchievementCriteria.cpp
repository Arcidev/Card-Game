#include "AchievementCriteria.h"
#include "Achievement.h"

AchievementCriteria::AchievementCriteria(Achievement* achievement) : m_isMet(false), m_progress(0), m_achievement(achievement) { }

void AchievementCriteria::CheckCriteriaMet()
{
    if (m_isMet)
        return;

    // TODO

    if (m_isMet)
        m_achievement->checkCriteriaMet();
}
