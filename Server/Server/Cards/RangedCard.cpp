#include "RangedCard.h"

RangedCard::RangedCard(uint64_t const& guid, Card const* card, Player* owner) : PlayableCard(guid, card, owner) { }

bool RangedCard::CanAttackCard(uint64_t const& guid, std::vector<PlayableCard*> const& opponentCards, uint32_t const& /*position*/)
{
    bool canAttack = true;
    for (PlayableCard const* card : opponentCards)
    {
        if (card->GetGuid() == guid)
        {
            if (card->GetType() == CARD_TYPE_DEFENSE)
                return true;
        }
        else if (card->GetType() == CARD_TYPE_DEFENSE)
            canAttack = false;
    }

    return canAttack;
}
