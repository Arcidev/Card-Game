#include "MeleeCard.h"

MeleeCard::MeleeCard(uint64_t guid, Card const* card, Player const* owner) : PlayableCard(guid, card, owner) { }

bool MeleeCard::CanAttackCard(uint64_t guid, std::vector<PlayableCard*> opponentCards, uint32_t position)
{
    for (uint32_t i = 0; i < opponentCards.size(); ++i)
        if (opponentCards[i]->GetGuid() == guid)
            return i == position;

    return false;
}
