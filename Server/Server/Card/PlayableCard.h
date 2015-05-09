#pragma once
#include <vector>
#include "Card.h"

class PlayableCard : public Card
{
    private:
        uint64_t m_guid;

    protected:
        PlayableCard(uint64_t guid, Card const& card);

    public:
        virtual ~PlayableCard() { }
        static PlayableCard* Create(uint64_t m_id, Card const& card);
        void DealDamage(uint32_t damage) { m_hp = (damage < m_hp ? m_hp - damage : 0); }

        uint64_t GetGuid() const { return m_guid; }
        virtual bool CanAttackCard(uint64_t guid, std::vector<PlayableCard*> opponentCards, uint32_t position) = 0;
};
