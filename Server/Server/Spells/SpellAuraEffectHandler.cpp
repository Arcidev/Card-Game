#include "SpellAuraEffectHandler.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

SpellAuraEffectTickHandlersMap SpellAuraEffectHandler::m_spellAuraEffectHandlers =
{
    { SPELL_AURA_EFFECT_DAMAGE,         handleDamageOnTick  },
    { SPELL_AURA_EFFECT_MODIFY_STAT,    nullptr             }
};

void SpellAuraEffectHandler::handleDamageOnTick(PlayableCard* card, uint8_t const& damage)
{
    card->GetOwner()->DealPeriodicDamage(card, damage);
}

SpellAuraEffectHandlerFunc SpellAuraEffectHandler::GetAuraEffectTickHandler(uint8_t spellAuraEffect)
{
    SpellAuraEffectTickHandlersMap::const_iterator iter = m_spellAuraEffectHandlers.find(spellAuraEffect);
    return iter != m_spellAuraEffectHandlers.end() ? iter->second : nullptr;
}
