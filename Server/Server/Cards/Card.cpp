#include "Card.h"

Card::Card(uint32_t id, uint8_t type, uint8_t hp, uint8_t damage, uint8_t mana, uint8_t defense, Spell const* spell)
    : m_id(id), m_type(type), m_spell(spell), m_hp(hp), m_damage(damage), m_mana(mana), m_defense(defense) { }
