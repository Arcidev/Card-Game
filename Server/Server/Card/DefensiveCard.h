#pragma once
#include "PlayableCard.h"

class DefensiveCard : public PlayableCard
{
    public:
        DefensiveCard(uint64_t guid, Card const& card);

        bool CanAttackCard(uint64_t /*guid*/, std::vector<PlayableCard*> /*opponentCards*/, uint32_t /*position*/) override { return false; }
};
