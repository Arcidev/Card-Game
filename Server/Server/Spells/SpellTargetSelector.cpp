#include "SpellTargetSelector.h"
#include "SpellDefines.h"
#include "../Cards/PlayableCard.h"
#include "../Player.h"

SpellTargetSelector::SpellTargetSelectorFunc const SpellTargetSelector::m_spellTargetSelectors[] =
{
    handleTargetUnitTargetEnemy,    // TARGET_UNIT_TARGET_ENEMY
    handleTargetUnitTargetFriend,   // TARGET_UNIT_TARGET_FRIEND
    handleTargetUnitCleaveEnemy,    // TARGET_UNIT_CLEAVE_ENEMY
    handleTargetUnitSelf            // TARGET_UNIT_SELF
};

std::list<PlayableCard*> SpellTargetSelector::handleTargetUnitTargetEnemy(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes)
{
    auto& cards = victim->GetCurrentCards();
    auto result = std::list<PlayableCard*>();

    for (size_t i = 0; i < cards.size(); i++)
    {
        if (cards[i]->GetGuid() != targetGuid)
            continue;

        // The effect can be applied only to melee target
        if (spellAttributes & SPELL_ATTRIBUTE_TARGET_MELEE && i != attacker->GetCurrentCardIndex())
            break;

        result.push_back(cards[i]);
        break;
    }

    return result;
}

std::list<PlayableCard*> SpellTargetSelector::handleTargetUnitTargetFriend(Player const* attacker, Player const* /*victim*/, uint64_t targetGuid, uint32_t spellAttributes)
{
    auto& cards = attacker->GetCurrentCards();
    auto result = std::list<PlayableCard*>();

    for (size_t i = 0; i < cards.size(); i++)
    {
        if (cards[i]->GetGuid() != targetGuid)
            continue;

        // The effect cannot be applied to self
        if (spellAttributes & SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF && i == attacker->GetCurrentCardIndex())
            break;

        result.push_back(cards[i]);
        break;
    }

    return result;
}

std::list<PlayableCard*> SpellTargetSelector::handleTargetUnitCleaveEnemy(Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes)
{
    auto& cards = victim->GetCurrentCards();
    auto result = std::list<PlayableCard*>();
    
    for (size_t i = 0; i < cards.size(); i++)
    {
        if (cards[i]->GetGuid() != targetGuid)
            continue;

        // The effect can be applied only to melee target
        if (spellAttributes & SPELL_ATTRIBUTE_TARGET_MELEE && i != attacker->GetCurrentCardIndex())
            break;

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

std::list<PlayableCard*> SpellTargetSelector::handleTargetUnitSelf(Player const* attacker, Player const* /*victim*/, uint64_t targetGuid, uint32_t /*spellAttributes*/)
{
    auto& cards = attacker->GetCurrentCards();
    auto result = std::list<PlayableCard*>();

    if (cards.size() > attacker->GetCurrentCardIndex())
        if (cards[attacker->GetCurrentCardIndex()]->GetGuid() == targetGuid)
            result.push_back(cards[attacker->GetCurrentCardIndex()]);

    return result;
}

std::list<PlayableCard*> SpellTargetSelector::GetTargets(uint8_t spellTarget, Player const* attacker, Player const* victim, uint64_t targetGuid, uint32_t spellAttributes)
{
    return spellTarget < MAX_SPELL_EFFECT_TARGET ? m_spellTargetSelectors[spellTarget](attacker, victim, targetGuid, spellAttributes) : std::list<PlayableCard*>();
}
