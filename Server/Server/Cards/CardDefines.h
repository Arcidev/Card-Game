#pragma once
#include <cstdint>

enum class CardType : uint8_t
{
    CARD_TYPE_MELEE = 1,
    CARD_TYPE_RANGED,
    CARD_TYPE_DEFENSE
};

enum class CardActions : uint8_t
{
    CARD_ACTION_NONE = 0,
    CARD_ACTION_BASIC_ATTACK,
    CARD_ACTION_SPELL_USE
};

enum class FailReson : uint8_t
{
    INVALID_CARD_COUNT = 1,
    INVALID_CARD_ID
};

enum class CardStats : uint8_t
{
    CARD_STAT_DEFENSE = 0x01,
    CARD_STAT_DAMAGE = 0x02,
    CARD_STAT_DAMAGE_TAKEN = 0x04,
    CARD_STAT_SPELL_DAMAGE = 0x08,
    CARD_STAT_SPELL_DAMAGE_TAKEN = 0x10,
    CARD_STAT_SPELL_HEAL = 0x20,
    CARD_STAT_SPELL_HEAL_TAKEN = 0x40
};

inline uint8_t operator & (CardStats a, CardStats b)
{
    return static_cast<uint8_t>(a) | static_cast<uint8_t>(b);
}
