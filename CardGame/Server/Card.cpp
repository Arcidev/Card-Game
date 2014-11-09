#include "Card.h"

Card::Card(CardType type) : m_type(type), m_isDefending(false)
{
}

void Card::Defend()
{
    m_isDefending = true;
    m_defense += 2;
}

void Card::StopDefending()
{
    if (!m_isDefending)
        return;

    m_isDefending = false;
    m_defense -= 2;
}

void Card::Die()
{

}
