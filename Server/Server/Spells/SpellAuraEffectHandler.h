#pragma once
#include <cstdint>

class PlayableCard;
class Player;
class SpellAuraEffect;

typedef void(*SpellAuraEffectApplyHandlerFunc)(SpellAuraEffect const& aura, Player* caster, PlayableCard* targetCard);
typedef void(*SpellAuraEffectRemoveHandlerFunc)(SpellAuraEffect const& aura, PlayableCard* card);

struct SpellAuraEffectFuncWrapper
{
    SpellAuraEffectApplyHandlerFunc Apply;
    SpellAuraEffectRemoveHandlerFunc Remove;
};

class SpellAuraEffectHandler
{
    private:
        static SpellAuraEffectFuncWrapper const m_spellAuraEffectHandlers[];

        static void defaultApplyHandler(SpellAuraEffect const& aura, Player* /*caster*/, PlayableCard* targetCard);
        static void defaultRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card);

        static void statChangedApplyHandler(SpellAuraEffect const& aura, Player* /*caster*/, PlayableCard* targetCard);
        static void statChangedRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card);

        static void morphApplyHandler(SpellAuraEffect const& aura, Player* caster, PlayableCard* targetCard);
        static void morphRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card);

    public:
        static SpellAuraEffectApplyHandlerFunc GetApplyHandler(uint8_t spellAuraEffect);
        static SpellAuraEffectRemoveHandlerFunc GetRemoveHandler(uint8_t spellAuraEffect);
};
