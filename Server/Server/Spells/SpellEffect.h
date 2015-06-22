#pragma once
#include "SpellTargetSelector.h"

enum SpellEffects
{
    SPELL_EFFECT_DIRECT_DAMAGE = 0,
    SPELL_EFFECT_APPLY_AURA,
    SPELL_EFFECT_HEAL,
    MAX_SPELL_EFFECT_VALUE
};

enum SpellAttributes
{
};

struct SpellEffectValues
{
    uint32_t const SpellId;
    uint32_t const SpellAttributes;
    uint8_t const Target;
    uint8_t const Value1;
    uint8_t const Value2;
    uint8_t const Value3;
    uint8_t const Value4;

    SpellEffectValues(uint32_t const& spellId, uint32_t const& spellAttributes, uint8_t const& target, uint8_t const& value1, uint8_t const& value2, uint8_t const& value3, uint8_t const& value4)
        : SpellId(spellId), SpellAttributes(spellAttributes), Target(target), Value1(value1), Value2(value2), Value3(value3), Value4(value4) { }
};

class Player;
typedef bool(*SpellEffectFunc)(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const* effectValues);

class SpellEffect
{
    private:
        static SpellEffectFunc const m_spellEffects[];

        static bool handleDirectDamage(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const* effectValues);
        static bool handleApplyAura(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const* effectValues);
        static bool handleHeal(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const* effectValues);

    public:
        static SpellEffectFunc GetSpellEffectFunc(uint8_t const& spellEffectId);
};
