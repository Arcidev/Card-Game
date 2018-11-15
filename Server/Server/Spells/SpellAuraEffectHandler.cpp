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

void SpellAuraEffectHandler::defaultApplyHandler(SpellEffectValues const& effectValues, Player* /*caster*/, PlayableCard* targetCard)
{
    applyAuraEffect(effectValues, targetCard);
}

void SpellAuraEffectHandler::defaultRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card)
{
    card->GetOwner()->SendAuraExpired(card->GetGuid(), aura);
}

void SpellAuraEffectHandler::handleDamageOnTick(PlayableCard* card, uint8_t damage, uint32_t spellAttributes)
{
    card->GetOwner()->DealPeriodicDamage(card, damage, spellAttributes & SPELL_ATTRIBUTE_APPLY_DEFENSE);
}

void SpellAuraEffectHandler::statChangedApplyHandler(SpellEffectValues const& effectValues, Player* /*caster*/, PlayableCard* targetCard)
{
    SpellAuraEffect const& aura = applyAuraEffect(effectValues, targetCard);

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

void SpellAuraEffectHandler::morphApplyHandler(SpellEffectValues const& effectValues, Player* caster, PlayableCard* targetCard)
{
    PlayableCard* card = caster->GetCurrentCard();
    card->RemoveAurasByType(SPELL_AURA_EFFECT_MORPH, true); // There should be only 1 active morph so no need to check all auras

    SpellAuraEffect const& aura = applyAuraEffect(effectValues, card);

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

SpellAuraEffect SpellAuraEffectHandler::applyAuraEffect(SpellEffectValues const& effectValues, PlayableCard* targetCard)
{
    SpellAuraEffect auraEffect(targetCard, effectValues.SpellId, effectValues.Value1, effectValues.Value2, effectValues.Value3, effectValues.Value4, effectValues.SpellAttributes);
    if (effectValues.SpellAttributes & SPELL_ATTRIBUTE_AURA_EXCLUSIVE)
    {
        std::list<uint32_t> removedSpellIds = targetCard->RemoveAurasByType(auraEffect.GetId(), false);
        targetCard->GetOwner()->SendAurasRemoved(targetCard->GetGuid(), removedSpellIds);
    }

    return targetCard->ApplyAura(auraEffect);
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
