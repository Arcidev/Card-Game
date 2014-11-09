#pragma once
#include <cstdint>

enum CardType
{
    CARD_TYPE_MELEE,
    CARD_TYPE_RANGED,
    CARD_TYPE_DEFENSE
};

class Card
{
    private:
        CardType m_type;
        uint64_t m_guid;
        uint32_t m_damage;
        uint32_t m_defense;
        uint32_t m_hp;
        bool m_isDefending;

    public:
        Card(CardType type);

        void Die();
        void Defend();
        void StopDefending();
        bool IsDefending() const { return m_isDefending; }
        bool IsAlive() const { return m_hp > 0; }
        uint32_t GetDamage() const { return m_damage; }
        uint32_t GetDefense() const { return m_defense; }
        uint32_t GetHealth() const { return m_hp; }
        void DealDamage(uint32_t damage) { m_hp -= damage; }
};

