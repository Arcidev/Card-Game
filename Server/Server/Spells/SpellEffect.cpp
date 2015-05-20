#include "SpellEffect.h"
#include "../Cards/PlayableCard.h"
#include "../PacketHandlers/Packet.h"
#include "../Player.h"

SpellEffectMap SpellEffect::m_spellEffects = 
{
    { SPELL_EFFECT_DIRECT_DAMAGE,   handleDirectDamage  },
    { SPELL_EFFECT_APPLY_AURA,      handleApplyAura     }
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

    for (std::list<PlayableCard*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
    {
        SpellAuraEffect auraEffect(*iter, effectValues->SpellId, effectValues->Value1, effectValues->Value2, effectValues->Value3, effectValues->Value4);
        (*iter)->ApplyAura(auraEffect);
    }

    return true;
}

SpellEffectFunc SpellEffect::GetSpellEffectFunc(uint8_t const& spellEffectId)
{
    SpellEffectMap::const_iterator iter = m_spellEffects.find(spellEffectId);
    return iter != m_spellEffects.end() ? iter->second : nullptr;
}
