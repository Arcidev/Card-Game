#include "MeleeCard.h"

MeleeCard::MeleeCard(uint64_t const& guid, Card const* card, Player* owner) : PlayableCard(guid, card, owner) { }

bool MeleeCard::CanAttackCard(uint64_t const& guid, std::vector<PlayableCard*> const& opponentCards, uint32_t const& position)
{
    return opponentCards.size() > position && opponentCards[position]->GetGuid() == guid;
}
