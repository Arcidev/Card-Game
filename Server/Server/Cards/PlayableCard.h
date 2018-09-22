#pragma once
#include <map>
#include <vector>
#include "Card.h"
#include "../Spells/SpellAuraEffect.h"

typedef std::map<uint32_t, SpellAuraEffect> SpellAuraEffectsMap;

class Player;

class PlayableCard : public Card
{
    private:
        uint64_t m_guid;
        bool m_isDefending;
        Player* m_owner;
        SpellAuraEffectsMap m_auras;

    protected:
        PlayableCard(uint64_t guid, Card const* card, Player* owner);

    public:
        virtual ~PlayableCard() { }
        static PlayableCard* Create(uint64_t m_id, Card const* card, Player* owner);

        void DealDamage(uint8_t damage) { m_hp = (damage < m_hp ? m_hp - damage : 0); }
        void SetDefendState(bool defend);
        void ApplyAura(SpellAuraEffect const& aura);
        void Heal(uint8_t amount);
        void AddHealth(uint8_t amount);
        void AddMana(uint8_t amount);
        void SubtractMana(uint8_t amount) { m_mana = (amount < m_mana ? m_mana - amount : 0); }
        void HandleTickOnAuras();

        bool IsAlive() const { return GetHealth() > 0; }
        bool HasAuras() const { return !m_auras.empty(); }
        uint8_t GetModifiedDefense() const;
        uint8_t GetModifiedDamage() const;
        int8_t GetStatModifierValue(uint8_t stat) const;
        uint64_t GetGuid() const { return m_guid; }
        Player* GetOwner() const { return m_owner; }
        virtual bool CanAttackCard(uint64_t guid, std::vector<PlayableCard*> const& opponentCards, uint32_t position) = 0;
};
