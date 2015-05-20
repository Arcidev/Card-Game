#include "SpellTargetSelector.h"
#include "../Cards/PlayableCard.h"
#include "../Player.h"

SpellTargetSelectorMap SpellTargetSelector::m_spellTargetSelectors =
{
    { TARGET_UNIT_CASTER,       handleTargetUnitCaster      },
    { TARGET_UNIT_TARGET_ENEMY, handleTargetUnitTargetEnemy }
};

std::list<PlayableCard*> SpellTargetSelector::handleTargetUnitCaster(Player* attacker, Player* /*victim*/, uint64_t const& /*targetGuid*/)
{
    return std::list<PlayableCard*> { attacker->GetCurrentCard() };
}

std::list<PlayableCard*> SpellTargetSelector::handleTargetUnitTargetEnemy(Player* /*attacker*/, Player* victim, uint64_t const& targetGuid)
{
    for (std::vector<PlayableCard*>::const_iterator iter = victim->GetCurrentCards().begin(); iter != victim->GetCurrentCards().end(); ++iter)
    {
        if (targetGuid == (*iter)->GetGuid())
            return std::list<PlayableCard*>{ (*iter) };
    }

    return std::list<PlayableCard*>();
}

std::list<PlayableCard*> SpellTargetSelector::GetTargets(uint8_t const& spellTarget, Player* attacker, Player* victim, uint64_t  const& targetGuid)
{
    SpellTargetSelectorMap::const_iterator iter = m_spellTargetSelectors.find(spellTarget);
    if (iter == m_spellTargetSelectors.end())
        return std::list<PlayableCard*>();

    return iter->second(attacker, victim, targetGuid);
}
