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

    Packet packet(SMSG_SPELL_DAMAGE);
    packet << (uint8_t)targets.size();

    for (std::list<PlayableCard*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
        packet.WriteGuidBitStreamInOrder((*iter)->GetGuid(), std::vector<uint8_t> { 6, 3, 1, 7, 0, 2, 5, 4 });

    packet.FlushBits();
    packet << attacker->GetId();
    for (std::list<PlayableCard*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
    {
        (*iter)->DealDamage(effectValues->Value1);

        packet.WriteGuidByteStreamInOrder((*iter)->GetGuid(), std::vector<uint8_t> { 4, 3, 5 });
        packet << effectValues->Value1;
        packet.WriteGuidByteStreamInOrder((*iter)->GetGuid(), std::vector<uint8_t> { 2, 0, 1, 6, 7 });
    }

    attacker->GetGame()->BroadcastPacket(&packet);
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
