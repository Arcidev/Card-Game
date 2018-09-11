#include "MeleeCard.h"

MeleeCard::MeleeCard(uint64_t guid, Card const* card, Player* owner) : PlayableCard(guid, card, owner) { }

bool MeleeCard::CanAttackCard(uint64_t guid, std::vector<PlayableCard*> const& opponentCards, uint32_t position)
{
    return opponentCards.size() > position && opponentCards[position]->GetGuid() == guid;
}
