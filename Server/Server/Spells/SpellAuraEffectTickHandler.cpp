#include "SpellAuraEffectTickHandler.h"
#include "SpellDefines.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

SpellAuraEffectTickHandlerFunc const SpellAuraEffectTickHandler::m_spellAuraEffectHandlers[] =
{
    handleDamageOnTick, // SPELL_AURA_EFFECT_DAMAGE
    nullptr,            // SPELL_AURA_EFFECT_MODIFY_STAT
    handleHealOnTick    // SPELL_AURA_EFFECT_HEAL
};

void SpellAuraEffectTickHandler::handleDamageOnTick(PlayableCard* card, uint8_t damage, uint32_t spellAttributes)
{
    card->GetOwner()->DealPeriodicDamage(card, damage, spellAttributes & SPELL_ATTRIBUTE_APPLY_DEFENSE);
}

void SpellAuraEffectTickHandler::handleHealOnTick(PlayableCard* card, uint8_t amount, uint32_t /*spellAttributes*/)
{
    card->Heal(amount);
}

SpellAuraEffectTickHandlerFunc SpellAuraEffectTickHandler::GetAuraEffectTickHandler(uint8_t spellAuraEffect)
{
    return spellAuraEffect < MAX_SPELL_AURA_VALUE ? m_spellAuraEffectHandlers[spellAuraEffect] : nullptr;
}
