#pragma once
#include "PlayableCard.h"

class DefensiveCard : public PlayableCard
{
    public:
        DefensiveCard(uint64_t const& guid, Card const* card, Player* owner);

        bool CanAttackCard(uint64_t const& /*guid*/, std::vector<PlayableCard*> const& /*opponentCards*/, uint32_t const& /*position*/) override { return false; }
};
