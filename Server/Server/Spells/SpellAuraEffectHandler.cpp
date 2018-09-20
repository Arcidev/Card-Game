#include "SpellAuraEffectHandler.h"
#include "SpellEffect.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

SpellAuraEffectHandlerFunc const SpellAuraEffectHandler::m_spellAuraEffectHandlers[] =
{
    handleDamageOnTick, // SPELL_AURA_EFFECT_DAMAGE
    nullptr,            // SPELL_AURA_EFFECT_MODIFY_STAT
    handleHealOnTick    // SPELL_AURA_EFFECT_HEAL
};

void SpellAuraEffectHandler::handleDamageOnTick(PlayableCard* card, uint8_t damage, uint32_t spellAttributes)
{
    card->GetOwner()->DealPeriodicDamage(card, damage, spellAttributes & SPELL_ATTRIBUTE_APPLY_DEFENSE);
}

void SpellAuraEffectHandler::handleHealOnTick(PlayableCard* card, uint8_t amount, uint32_t spellAttributes)
{
    card->Heal(amount);
}

SpellAuraEffectHandlerFunc SpellAuraEffectHandler::GetAuraEffectTickHandler(uint8_t spellAuraEffect)
{
    return spellAuraEffect < MAX_SPELL_AURA_VALUE ? m_spellAuraEffectHandlers[spellAuraEffect] : nullptr;
}
