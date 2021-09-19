#pragma once
#include  <cstdint>

enum class CriteriaTypes
{
    CRITERIA_TYPE_LOGIN,
    CRITERIA_TYPE_GAME_PLAYED,
    CRITERIA_TYPE_GAME_WON
};


class AchievementCriteriaEntry
{
    protected:
        uint32_t const m_id;
        uint32_t const m_achievementId;
        CriteriaTypes const m_type;
        uint32_t const m_requirement;

    public:
        AchievementCriteriaEntry(uint32_t id, uint32_t achievementId, CriteriaTypes type, uint32_t requirement);
        uint32_t GetId() const { return m_id; }
        uint32_t GetRequirement() const { return m_requirement; }
        CriteriaTypes GetType() const { return m_type; }
};
