#pragma once
#include <cstdint>

class PlayableCard;
class Player;
class SpellAuraEffect;
struct SpellEffectValues;

typedef void(*SpellAuraEffectApplyHandlerFunc)(SpellEffectValues const& aura, Player* caster, PlayableCard* targetCard);
typedef void(*SpellAuraEffectRemoveHandlerFunc)(SpellAuraEffect const& aura, PlayableCard* card);
typedef void(*SpellAuraEffectTickHandlerFunc)(PlayableCard* card, uint8_t value, uint32_t spellAttributes, PlayableCard const* caster);

struct SpellAuraEffectFuncWrapper
{
    SpellAuraEffectApplyHandlerFunc Apply;
    SpellAuraEffectRemoveHandlerFunc Remove;
    SpellAuraEffectTickHandlerFunc Tick;
};

class SpellAuraEffectHandler
{
    private:
        static SpellAuraEffectFuncWrapper const m_spellAuraEffectHandlers[];

        static void defaultApplyHandler(SpellEffectValues const& effectValues, Player* caster, PlayableCard* targetCard);

        // SPELL_AURA_EFFECT_DAMAGE
        static void handleDamageOnTick(PlayableCard* card, uint8_t damage, uint32_t spellAttributes, PlayableCard const* caster);

        // SPELL_AURA_EFFECT_MODIFY_STAT 
        static void statChangedApplyHandler(SpellEffectValues const& effectValues, Player* caster, PlayableCard* targetCard);
        static void statChangedRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card);

        // SPELL_AURA_EFFECT_HEAL
        static void handleHealOnTick(PlayableCard* card, uint8_t damage, uint32_t /*spellAttributes*/, PlayableCard const* caster);

        // SPELL_AURA_EFFECT_MORPH
        static void morphApplyHandler(SpellEffectValues const& effectValues, Player* caster, PlayableCard* targetCard);
        static void morphRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card);

        // Misc
        static SpellAuraEffect applyAuraEffect(SpellEffectValues const& effectValues, PlayableCard* targetCard, PlayableCard const* caster);

    public:
        static SpellAuraEffectApplyHandlerFunc GetApplyHandler(uint8_t spellAuraEffect);
        static SpellAuraEffectRemoveHandlerFunc GetRemoveHandler(uint8_t spellAuraEffect);
        static SpellAuraEffectTickHandlerFunc GetAuraEffectTickHandler(uint8_t spellAuraEffect);
};
