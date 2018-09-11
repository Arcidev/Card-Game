#include "RangedCard.h"

RangedCard::RangedCard(uint64_t guid, Card const* card, Player* owner) : PlayableCard(guid, card, owner) { }

bool RangedCard::CanAttackCard(uint64_t guid, std::vector<PlayableCard*> const& opponentCards, uint32_t /*position*/)
{
    bool canAttack = true;
    for (PlayableCard const* card : opponentCards)
    {
        if (card->GetType() == CARD_TYPE_DEFENSE)
        {
            if (card->GetGuid() == guid)
                return true;

            if (canAttack)
                canAttack = false;
        }
    }

    return canAttack;
}
