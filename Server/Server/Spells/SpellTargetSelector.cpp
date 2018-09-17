#include "SpellTargetSelector.h"
#include "../Cards/PlayableCard.h"
#include "../Player.h"

SpellTargetSelectorFunc const SpellTargetSelector::m_spellTargetSelectors[] =
{
    handleTargetUnitTargetEnemy,    // TARGET_UNIT_TARGET_ENEMY
    handleTargetUnitTargetFriend,   // TARGET_UNIT_TARGET_FRIEND
    handleTargetUnitCleaveEnemy     // TARGET_UNIT_CLEAVE_ENEMY
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

std::list<PlayableCard*> SpellTargetSelector::handleTargetUnitCleaveEnemy(Player* /*attacker*/, Player* victim, uint64_t targetGuid)
{
    auto cards = victim->GetCurrentCards();
    auto result = std::list<PlayableCard*>();
    
    for (size_t i = 0; i < cards.size(); i++)
    {
        if (cards[i]->GetGuid() != targetGuid)
            continue;

        result.push_back(cards[i]);
        
        // Cleave target to left
        if (i > 0)
            result.push_back(cards[i - 1]);

        // Cleave target to right
        if (i < cards.size() - 1)
            result.push_back(cards[i + 1]);

        break;
    }

    return result;
}

std::list<PlayableCard*> SpellTargetSelector::GetTargets(uint8_t spellTarget, Player* attacker, Player* victim, uint64_t targetGuid)
{
    return spellTarget < MAX_SPELL_EFFECT_TARGET ? m_spellTargetSelectors[spellTarget](attacker, victim, targetGuid) : std::list<PlayableCard*>();
}
