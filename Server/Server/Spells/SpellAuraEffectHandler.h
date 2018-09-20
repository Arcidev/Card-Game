#pragma once
#include <cstdint>

class PlayableCard;

typedef void(*SpellAuraEffectHandlerFunc)(PlayableCard* card, uint8_t value, uint32_t spellAttributes);

enum SpellAuraEffects
{
    SPELL_AURA_EFFECT_DAMAGE = 0,
    SPELL_AURA_EFFECT_MODIFY_STAT,
    SPELL_AURA_EFFECT_HEAL,
    MAX_SPELL_AURA_VALUE
};

class SpellAuraEffectHandler
{
    private:
        static SpellAuraEffectHandlerFunc const m_spellAuraEffectHandlers[];

        static void handleDamageOnTick(PlayableCard* card, uint8_t damage, uint32_t spellAttributes);
        static void handleHealOnTick(PlayableCard* card, uint8_t damage, uint32_t /*spellAttributes*/);

    public:
        static SpellAuraEffectHandlerFunc GetAuraEffectTickHandler(uint8_t spellAuraEffect);
};
