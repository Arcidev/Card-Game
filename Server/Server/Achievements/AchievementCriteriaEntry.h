#pragma once
#include  <cstdint>

enum class CriteriaTypes
{
    CRITERIA_TYPE_LOGIN
};


class AchievementCriteriaEntry
{
    protected:
        uint32_t const m_achievementId;
        CriteriaTypes const m_type;
        uint32_t const m_requirement;

    public:
        AchievementCriteriaEntry(uint32_t achievementId, CriteriaTypes type, uint32_t requirement);
};
