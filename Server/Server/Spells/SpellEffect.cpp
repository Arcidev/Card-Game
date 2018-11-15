#include "SpellEffect.h"
#include "SpellAuraEffectHandler.h"
#include "SpellDefines.h"
#include "SpellTargetSelector.h"
#include "../Cards/PlayableCard.h"
#include "../Player.h"

SpellEffectFunc const SpellEffect::m_spellEffects[] =
{
    handleDirectDamage, // SPELL_EFFECT_DIRECT_DAMAGE
    handleApplyAura,    // SPELL_EFFECT_APPLY_AURA
    handleHeal,         // SPELL_EFFECT_HEAL
    handleDrain,        // SPELL_EFFECT_DRAIN
    removeAuraType      // SPELL_EFFECT_REMOVE_AURA_TYPE
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

    for (PlayableCard* target : targets)
        target->Heal(effectValues.Value1);

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

bool SpellEffect::removeAuraType(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const& effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues.Target, attacker, victim, targetGuid, effectValues.SpellAttributes);
    if (targets.empty())
        return false;

    for (PlayableCard* target : targets)
    {
        std::list<uint32_t> removedSpellIds = target->RemoveAurasByType(effectValues.Value1, effectValues.SpellAttributes & SPELL_ATTRIBUTE_APPLY_TO_FIRST_ONLY);
        target->GetOwner()->SendAurasRemoved(target->GetGuid(), removedSpellIds);
    }

    return true;
}

SpellEffectFunc SpellEffect::GetSpellEffectFunc(uint8_t spellEffectId)
{
    return spellEffectId < MAX_SPELL_EFFECT_VALUE ? m_spellEffects[spellEffectId] : nullptr;
}
