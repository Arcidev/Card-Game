#include "RangedCard.h"

RangedCard::RangedCard(uint64_t const& guid, Card const* card, Player* owner) : PlayableCard(guid, card, owner) { }

bool RangedCard::CanAttackCard(uint64_t const& guid, std::vector<PlayableCard*> const& opponentCards, uint32_t const& /*position*/)
{
    bool canAttack = true;
    for (std::vector<PlayableCard*>::const_iterator iter = opponentCards.begin(); iter != opponentCards.end(); ++iter)
    {
        if ((*iter)->GetGuid() == guid)
        {
            if ((*iter)->GetType() == CARD_TYPE_DEFENSE)
                return true;
        }
        else if ((*iter)->GetType() == CARD_TYPE_DEFENSE)
            canAttack = false;
    }

    return canAttack;
}
