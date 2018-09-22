#pragma once
#include <cstdint>

class PlayableCard;

typedef void(*SpellAuraEffectTickHandlerFunc)(PlayableCard* card, uint8_t value, uint32_t spellAttributes);

class SpellAuraEffectTickHandler
{
    private:
        static SpellAuraEffectTickHandlerFunc const m_spellAuraEffectHandlers[];

        static void handleDamageOnTick(PlayableCard* card, uint8_t damage, uint32_t spellAttributes);
        static void handleHealOnTick(PlayableCard* card, uint8_t damage, uint32_t /*spellAttributes*/);

    public:
        static SpellAuraEffectTickHandlerFunc GetAuraEffectTickHandler(uint8_t spellAuraEffect);
};
