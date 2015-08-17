#include "SpellEffect.h"
#include "../Cards/PlayableCard.h"
#include "../PacketHandlers/Packet.h"
#include "../Player.h"

SpellEffectFunc const SpellEffect::m_spellEffects[] =
{
    handleDirectDamage, // SPELL_EFFECT_DIRECT_DAMAGE
    handleApplyAura,    // SPELL_EFFECT_APPLY_AURA
    handleHeal          // SPELL_EFFECT_HEAL
};

bool SpellEffect::handleDirectDamage(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const* effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues->Target, attacker, victim, targetGuid);
    if (targets.empty())
        return false;

    attacker->SpellAttack(targets, effectValues->Value1);
    return true;
}

bool SpellEffect::handleApplyAura(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const* effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues->Target, attacker, victim, targetGuid);
    if (targets.empty())
        return false;

    for (PlayableCard* target : targets)
    {
        SpellAuraEffect auraEffect(target, effectValues->SpellId, effectValues->Value1, effectValues->Value2, effectValues->Value3, effectValues->Value4);
        target->ApplyAura(auraEffect);
    }

    return true;
}

bool SpellEffect::handleHeal(Player* attacker, Player* victim, uint64_t targetGuid, SpellEffectValues const* effectValues)
{
    std::list<PlayableCard*> targets = SpellTargetSelector::GetTargets(effectValues->Target, attacker, victim, targetGuid);
    if (targets.empty())
        return false;

    for (PlayableCard* target : targets)
        target->Heal(effectValues->Value1);

    return true;
}

SpellEffectFunc SpellEffect::GetSpellEffectFunc(uint8_t const& spellEffectId)
{
    return spellEffectId < MAX_SPELL_EFFECT_VALUE ? m_spellEffects[spellEffectId] : nullptr;
}
