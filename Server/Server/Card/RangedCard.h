#pragma once
#include "PlayableCard.h"

class RangedCard : public PlayableCard
{
    public:
        RangedCard(uint64_t guid, Card const& card);

        bool CanAttackCard(uint64_t guid, std::vector<PlayableCard*> opponentCards, uint32_t /*position*/) override;
};
