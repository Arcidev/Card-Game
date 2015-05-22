#include "SpellAuraEffectHandler.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

SpellAuraEffectTickHandlersMap SpellAuraEffectHandler::m_spellAuraEffectHandlers =
{
    { SPELL_AURA_EFFECT_DAMAGE,         handleDamageOnTick  },
    { SPELL_AURA_EFFECT_MODIFY_STAT,    nullptr             },
    { SPELL_AURA_EFFECT_HEAL,           handleHealOnTick    }
};

void SpellAuraEffectHandler::handleDamageOnTick(PlayableCard* card, uint8_t const& damage)
{
    card->GetOwner()->DealPeriodicDamage(card, damage);
}

void SpellAuraEffectHandler::handleHealOnTick(PlayableCard* card, uint8_t const& amount)
{
    card->Heal(amount);
}

SpellAuraEffectHandlerFunc SpellAuraEffectHandler::GetAuraEffectTickHandler(uint8_t spellAuraEffect)
{
    SpellAuraEffectTickHandlersMap::const_iterator iter = m_spellAuraEffectHandlers.find(spellAuraEffect);
    return iter != m_spellAuraEffectHandlers.end() ? iter->second : nullptr;
}
