#pragma once
#include <vector>
#include "Card.h"

class Player;

class PlayableCard : public Card
{
    private:
        uint64_t m_guid;
        int8_t m_defenseModifier;
        bool m_isDefending;
        Player const* m_owner;

    protected:
        PlayableCard(uint64_t guid, Card const* card, Player const* owner);

    public:
        virtual ~PlayableCard() { }
        static PlayableCard* Create(uint64_t m_id, Card const* card, Player const* owner);

        void DealDamage(uint32_t damage) { m_hp = (damage < m_hp ? m_hp - damage : 0); }
        void SetDefendState(bool defend);
        
        bool IsAlive() const { return GetHealth() > 0; }
        uint8_t GetModifiedDefense() const;
        uint64_t GetGuid() const { return m_guid; }
        virtual bool CanAttackCard(uint64_t guid, std::vector<PlayableCard*> opponentCards, uint32_t position) = 0;
};
