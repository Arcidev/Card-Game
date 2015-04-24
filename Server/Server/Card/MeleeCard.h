#pragma once
#include "PlayableCard.h"

class MeleeCard : public PlayableCard
{
    public:
        MeleeCard(uint64_t guid, Card const& card);

        bool CanAttackCard(uint64_t guid, std::vector<PlayableCard*> opponentCards, uint32_t position) override;
};
