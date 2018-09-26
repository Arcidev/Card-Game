#include "SpellAuraEffectHandler.h"
#include "SpellDefines.h"
#include "../DataHolder.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

SpellAuraEffectFuncWrapper const SpellAuraEffectHandler::m_spellAuraEffectHandlers[] =
{
    { defaultApplyHandler,      defaultRemoveHandler        },  // SPELL_AURA_EFFECT_DAMAGE
    { statChangedApplyHandler,  statChangedRemoveHandler    },  // SPELL_AURA_EFFECT_MODIFY_STAT
    { defaultApplyHandler,      defaultRemoveHandler        },  // SPELL_AURA_EFFECT_HEAL
    { morphApplyHandler,        morphRemoveHandler          }   // SPELL_AURA_MORPH
};

void SpellAuraEffectHandler::defaultApplyHandler(SpellAuraEffect const& aura, Player* /*caster*/, PlayableCard* targetCard)
{
    targetCard->ApplyAura(aura);
}

void SpellAuraEffectHandler::defaultRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card)
{
    card->GetOwner()->SendAuraExpired(card->GetGuid(), &aura);
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

void SpellAuraEffectHandler::morphApplyHandler(SpellAuraEffect const& aura, Player* caster, PlayableCard* targetCard)
{
    PlayableCard* card = caster->GetCurrentCard();
    card->Morph(DataHolder::GetCard(targetCard->GetId()));
    card->SetMana(card->GetMorph()->GetMana() * aura.GetValue1());

    caster->SendMorphInfo(card);
}

void SpellAuraEffectHandler::morphRemoveHandler(SpellAuraEffect const& aura, PlayableCard* card)
{
    card->Morph(nullptr);
    card->SetMana(DataHolder::GetCard(card->GetId())->GetMana() * aura.GetValue2());

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
