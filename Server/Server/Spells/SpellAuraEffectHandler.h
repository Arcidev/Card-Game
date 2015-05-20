#pragma once
#include <map>
#include <cstdint>

class PlayableCard;

typedef void(*SpellAuraEffectHandlerFunc)(PlayableCard* card, uint8_t const& value);
typedef std::map<uint8_t, SpellAuraEffectHandlerFunc> SpellAuraEffectTickHandlersMap;

enum SpellAuraEffects
{
    SPELL_AURA_EFFECT_DAMAGE = 1,
    SPELL_AURA_EFFECT_MODIFY_STAT
};

class SpellAuraEffectHandler
{
    private:
        static SpellAuraEffectTickHandlersMap m_spellAuraEffectHandlers;

        static void handleDamageOnTick(PlayableCard* card, uint8_t const& damage);

    public:
        static SpellAuraEffectHandlerFunc GetAuraEffectTickHandler(uint8_t spellAuraEffect);
};
