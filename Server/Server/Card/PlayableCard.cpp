#include <stdexcept>
#include "MeleeCard.h"
#include "RangedCard.h"
#include "DefensiveCard.h"

PlayableCard::PlayableCard(uint64_t guid, Card const& card) : Card(card), m_guid(guid) { }

PlayableCard* PlayableCard::Create(uint64_t guid, Card const& card)
{
    switch (card.GetType())
    {
        case CARD_TYPE_MELEE:
            return new MeleeCard(guid, card);
        case CARD_TYPE_RANGED:
            return new RangedCard(guid, card);
        case CARD_TYPE_DEFENSE:
            return new DefensiveCard(guid, card);
        default:
            throw std::invalid_argument("Card type not exists");
    }
}
