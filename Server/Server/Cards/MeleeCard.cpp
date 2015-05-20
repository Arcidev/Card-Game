#include "MeleeCard.h"

MeleeCard::MeleeCard(uint64_t const& guid, Card const* card, Player* owner) : PlayableCard(guid, card, owner) { }

bool MeleeCard::CanAttackCard(uint64_t const& guid, std::vector<PlayableCard*> const& opponentCards, uint32_t const& position)
{
    for (uint32_t i = 0; i < opponentCards.size(); ++i)
        if (opponentCards[i]->GetGuid() == guid)
            return i == position;

    return false;
}
