#include <ctime>
#include "AchievementCriteria.h"
#include "Achievement.h"

AchievementCriteria::AchievementCriteria(Achievement* achievement, uint32_t progress, time_t lastModified, AchievementCriteriaEntry const& entry) : AchievementCriteriaEntry(entry), m_isMet(false), m_progress(progress), m_lastModified(lastModified), m_achievement(achievement)
{
    setIsMet();
}

void AchievementCriteria::checkCriteriaMet()
{
    setIsMet();

    if (m_isMet)
        m_achievement->CheckCriteriaMet();
}

void AchievementCriteria::SetProgress(int progress)
{
    if (m_isMet)
        return;

    m_progress = progress;
    m_lastModified = std::time(nullptr);
    checkCriteriaMet();
}
