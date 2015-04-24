#include "Card.h"

Card::Card(uint32_t id, uint8_t type, uint8_t hp, uint8_t damage, uint8_t mana, uint8_t defense)
    : m_id(id), m_type(type), m_hp(hp), m_damage(damage), m_mana(mana), m_defense(defense), m_isDefending(false), m_onDeck(false) { }

// Card turns on the defend state
void Card::Defend()
{
    m_isDefending = true;
    m_defense += 2;
}

// Card turns off the defend state
void Card::StopDefending()
{
    if (!m_isDefending)
        return;

    m_isDefending = false;
    m_defense -= 2;
}

// Card has no hitpoints
void Card::Die()
{

}
