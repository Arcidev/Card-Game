#pragma once
#include <cstdint>

enum class AttackResult : uint8_t
{
    ATTACK_RESULT_INVALID_TARGET = 0,
    ATTACK_RESULT_CARD_ATTACKED,
    ATTACK_RESULT_CARD_DESTROYED
};

enum class SystemStats : uint8_t
{
    DEFENSE_PERCENT_PER_POINT   = 6,
    DEFENSE_BONUS_ON_DEFEND     = 2,
    MANA_REPLENISHMENT_MOVES    = 15,
    MANA_REPLENISHMENT_VALUE    = 2
};
