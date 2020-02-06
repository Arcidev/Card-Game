#pragma once
#include <cstdint>

constexpr uint8_t MAX_CARDS_COUNT = 5;
constexpr uint8_t MAX_CARDS_ON_DECK = 4;

enum CardType
{
    CARD_TYPE_MELEE = 1,
    CARD_TYPE_RANGED,
    CARD_TYPE_DEFENSE
};

enum CardActions
{
    CARD_ACTION_NONE = 0,
    CARD_ACTION_DEFEND,
    CARD_ACTION_BASIC_ATTACK,
    CARD_ACTION_SPELL_USE
};

enum FailReson
{
    INVALID_CARD_COUNT = 1,
    INVALID_CARD_ID
};

enum CardStats
{
    CARD_STAT_DEFENSE               = 0x01,
    CARD_STAT_DAMAGE                = 0x02,
    CARD_STAT_DAMAGE_TAKEN          = 0x04,
    CARD_STAT_SPELL_DAMAGE          = 0x08,
    CARD_STAT_SPELL_DAMAGE_TAKEN    = 0x10,
    CARD_STAT_SPELL_HEAL            = 0x20,
    CARD_STAT_SPELL_HEAL_TAKEN      = 0x40
};

class Spell;
class Card
{
    private:
        uint32_t const m_id;
        uint8_t const m_type;
        uint8_t const m_price;
        uint8_t const m_damage;
        uint8_t const m_defense;
        Spell const* m_spell;

    protected:
        uint8_t m_hp;
        uint8_t m_mana;

    public:
        Card(uint32_t id, uint8_t type, uint8_t hp, uint8_t damage, uint8_t mana, uint8_t defense, uint8_t price, Spell const* spell);
        virtual ~Card() { }

        uint32_t GetId() const { return m_id; }
        uint8_t GetType() const { return m_type; }
        uint8_t GetDamage() const { return m_damage; }
        uint8_t GetDefense() const { return m_defense; }
        uint8_t GetMana() const { return m_mana; }
        uint8_t GetHealth() const { return m_hp; }
        uint8_t GetPrice() const { return m_price; }
        virtual Spell const* GetSpell() const { return m_spell; }
};

