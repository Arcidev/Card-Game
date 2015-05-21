#include "SpellTargetSelector.h"
#include "../Cards/PlayableCard.h"
#include "../Player.h"

SpellTargetSelectorMap SpellTargetSelector::m_spellTargetSelectors =
{
    { TARGET_UNIT_TARGET_ENEMY,     handleTargetUnitTargetEnemy     },
    { TARGET_UNIT_TARGET_FRIEND,    handleTargetUnitTargetFriend    }
};

std::list<PlayableCard*> SpellTargetSelector::getTargetFromDeck(Player* player, uint64_t const& targetGuid)
{
    for (std::vector<PlayableCard*>::const_iterator iter = player->GetCurrentCards().begin(); iter != player->GetCurrentCards().end(); ++iter)
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
