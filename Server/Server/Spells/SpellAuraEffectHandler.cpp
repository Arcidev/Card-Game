#include "SpellAuraEffectHandler.h"
#include "SpellDefines.h"
#include "../DataHolder.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

SpellAuraEffectFuncWrapper const SpellAuraEffectHandler::m_spellAuraEffectHandlers[] =
{
    { defaultApplyHandler,      defaultRemoveHandler,       handleDamageOnTick  },  // SPELL_AURA_EFFECT_DAMAGE
    { statChangedApplyHandler,  statChangedRemoveHandler,   nullptr             },  // SPELL_AURA_EFFECT_MODIFY_STAT
    { defaultApplyHandler,      defaultRemoveHandler,       handleHealOnTick    },  // SPELL_AURA_EFFECT_HEAL
    { morphApplyHandler,        morphRemoveHandler,         nullptr             }   // SPELL_AURA_EFFECT_MORPH
};

void SpellAuraEffectHandler::defaultApplyHandler(SpellAuraEffect const& aura, Player* /*caster*/, PlayableCard* targetCard)
{
    targetCard->ApplyAura(aura);
}

void SpellAuraEffectHandler::defaultRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card)
{
    card->GetOwner()->SendAuraExpired(card->GetGuid(), &aura);
}

void SpellAuraEffectHandler::handleDamageOnTick(PlayableCard* card, uint8_t damage, uint32_t spellAttributes)
{
    card->GetOwner()->DealPeriodicDamage(card, damage, spellAttributes & SPELL_ATTRIBUTE_APPLY_DEFENSE);
}

void SpellAuraEffectHandler::statChangedApplyHandler(SpellAuraEffect const& aura, Player* /*caster*/, PlayableCard* targetCard)
{
    targetCard->ApplyAura(aura);
    targetCard->GetOwner()->SendCardStatChanged(targetCard, aura.GetValue1());
}

void SpellAuraEffectHandler::statChangedRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card)
{
    card->GetOwner()->SendCardStatChanged(card, aura.GetValue1());
}

void SpellAuraEffectHandler::handleHealOnTick(PlayableCard* card, uint8_t amount, uint32_t /*spellAttributes*/)
{
    card->Heal(amount);
}

void SpellAuraEffectHandler::morphApplyHandler(SpellAuraEffect const& aura, Player* caster, PlayableCard* targetCard)
{
    PlayableCard* card = caster->GetCurrentCard();
    card->Morph(DataHolder::GetCard(targetCard->GetId()));
    card->SetMana(uint8_t(card->GetMorph()->GetMana() * aura.GetValue1() / 100.f));

    caster->SendMorphInfo(card);
}

void SpellAuraEffectHandler::morphRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card)
{
    card->Morph(nullptr);
    card->SetMana(uint8_t(DataHolder::GetCard(card->GetId())->GetMana() * aura.GetValue2() / 100.f));

    card->GetOwner()->SendMorphInfo(card);
}

SpellAuraEffectApplyHandlerFunc SpellAuraEffectHandler::GetApplyHandler(uint8_t spellAuraEffect)
{
    return spellAuraEffect < MAX_SPELL_AURA_VALUE ? m_spellAuraEffectHandlers[spellAuraEffect].Apply : nullptr;
}

SpellAuraEffectRemoveHandlerFunc SpellAuraEffectHandler::GetRemoveHandler(uint8_t spellAuraEffect)
{
    return spellAuraEffect < MAX_SPELL_AURA_VALUE ? m_spellAuraEffectHandlers[spellAuraEffect].Remove : nullptr;
}

SpellAuraEffectTickHandlerFunc SpellAuraEffectHandler::GetAuraEffectTickHandler(uint8_t spellAuraEffect)
{
    return spellAuraEffect < MAX_SPELL_AURA_VALUE ? m_spellAuraEffectHandlers[spellAuraEffect].Tick : nullptr;
}
