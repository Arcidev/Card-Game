#include <stdexcept>
#include "MeleeCard.h"
#include "RangedCard.h"
#include "DefensiveCard.h"
#include "../Player.h"
#include "../PlayerDefines.h"

PlayableCard::PlayableCard(uint64_t guid, Card const* card, Player const* owner) : Card(*card), m_guid(guid), m_defenseModifier(0), m_isDefending(false), m_owner(owner) { }

PlayableCard* PlayableCard::Create(uint64_t guid, Card const* card, Player const* owner)
{
    switch (card->GetType())
    {
        case CARD_TYPE_MELEE:
            return new MeleeCard(guid, card, owner);
        case CARD_TYPE_RANGED:
            return new RangedCard(guid, card, owner);
        case CARD_TYPE_DEFENSE:
            return new DefensiveCard(guid, card, owner);
        default:
            throw std::invalid_argument("Card type not exists");
    }
}

void PlayableCard::SetDefendState(bool defend)
{
    if (m_isDefending == defend)
        return;

    m_isDefending = defend;
    m_defenseModifier += defend ? DEFENSE_BONUS_ON_DEFEND : -DEFENSE_BONUS_ON_DEFEND;
    m_owner->SendCardStatChanged(m_guid, CARD_STAT_DEFENSE, m_defenseModifier);
}

uint8_t PlayableCard::GetModifiedDefense() const
{
    if (m_defenseModifier < 0)
        if (m_defense < -m_defenseModifier)
            return 0;

    return m_defense + m_defenseModifier;
}
