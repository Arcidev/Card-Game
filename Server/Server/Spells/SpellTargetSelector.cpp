#include "SpellTargetSelector.h"
#include "../Cards/PlayableCard.h"
#include "../Player.h"

SpellTargetSelectorFunc const SpellTargetSelector::m_spellTargetSelectors[] =
{
    handleTargetUnitTargetEnemy,    // TARGET_UNIT_TARGET_ENEMY
    handleTargetUnitTargetFriend    // TARGET_UNIT_TARGET_FRIEND
};

std::list<PlayableCard*> SpellTargetSelector::getTargetFromDeck(Player* player, uint64_t targetGuid)
{
    for (PlayableCard* card : player->GetCurrentCards())
    {
        if (targetGuid == card->GetGuid())
            return std::list<PlayableCard*>{ card };
    }

    return std::list<PlayableCard*>();
}

std::list<PlayableCard*> SpellTargetSelector::GetTargets(uint8_t spellTarget, Player* attacker, Player* victim, uint64_t targetGuid)
{
    return spellTarget < MAX_SPELL_EFFECT_TARGET ? m_spellTargetSelectors[spellTarget](attacker, victim, targetGuid) : std::list<PlayableCard*>();
}
