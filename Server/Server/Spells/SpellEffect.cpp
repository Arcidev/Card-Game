#include <algorithm>
#include "SpellEffect.h"
#include "SpellAuraEffectHandler.h"
#include "SpellDefines.h"
#include "SpellTargetSelector.h"
#include "../Cards/PlayableCard.h"
#include "../Player.h"

SpellEffectFunc const SpellEffect::m_spellEffects[] =
{
    handleDirectDamage,     // SPELL_EFFECT_DIRECT_DAMAGE
    handleApplyAura,        // SPELL_EFFECT_APPLY_AURA
    handleHeal,             // SPELL_EFFECT_HEAL
    handleDrain,            // SPELL_EFFECT_DRAIN
    handleRemoveAuraType,   // SPELL_EFFECT_REMOVE_AURA_TYPE
    handleSwap              // SPELL_EFFECT_SWAP
};

bool SpellEffect::handleDirectDamage(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const& effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues.Target, attacker, victim, targetGuid, effectValues.SpellAttributes);
    if (targets.empty())
        return false;

    attacker->SpellAttack(targets, effectValues.Value1, effectValues.SpellAttributes & SPELL_ATTRIBUTE_APPLY_DEFENSE);
    return true;
}

bool SpellEffect::handleApplyAura(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const& effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues.Target, attacker, victim, targetGuid, effectValues.SpellAttributes);
    if (targets.empty())
        return false;

    SpellAuraEffectApplyHandlerFunc applyHandler = SpellAuraEffectHandler::GetApplyHandler(effectValues.Value1);
    if (!applyHandler)
        return false;

    for (PlayableCard* target : targets)
        applyHandler(effectValues, attacker, target);

    return true;
}

bool SpellEffect::handleHeal(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const& effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues.Target, attacker, victim, targetGuid, effectValues.SpellAttributes);
    if (targets.empty())
        return false;

    uint8_t attackerModifier = attacker->GetCurrentCard()->GetStatModifierValue(CardStats::CARD_STAT_SPELL_HEAL);
    for (PlayableCard* target : targets)
    {
        uint8_t amount = (std::max)(effectValues.Value1 + attackerModifier + target->GetStatModifierValue(CardStats::CARD_STAT_SPELL_HEAL_TAKEN), 0);
        target->Heal(amount);
    }

    return true;
}

bool SpellEffect::handleDrain(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const& effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues.Target, attacker, victim, targetGuid, effectValues.SpellAttributes);
    if (targets.empty())
        return false;

    for (PlayableCard* target : targets)
        attacker->Drain(target, effectValues.Value1, effectValues.Value2, effectValues.Value3, effectValues.Value4, effectValues.SpellAttributes & SPELL_ATTRIBUTE_APPLY_DEFENSE);

    return true;
}

bool SpellEffect::handleRemoveAuraType(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const& effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues.Target, attacker, victim, targetGuid, effectValues.SpellAttributes);
    if (targets.empty())
        return false;

    for (PlayableCard* target : targets)
        target->RemoveAurasByType(effectValues.Value1, effectValues.Value2);

    return true;
}

bool SpellEffect::handleSwap(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const& effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues.Target, attacker, victim, targetGuid, effectValues.SpellAttributes);
    if (targets.empty())
        return false;

    return attacker->SwapCards(attacker->GetCurrentCard(), targets.front());
}

SpellEffectFunc SpellEffect::GetSpellEffectFunc(uint8_t spellEffectId)
{
    return spellEffectId < MAX_SPELL_EFFECT_VALUE ? m_spellEffects[spellEffectId] : nullptr;
}
