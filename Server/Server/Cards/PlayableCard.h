#pragma once
#include <map>
#include <vector>
#include "Card.h"
#include "../Spells/SpellAuraEffect.h"


class Player;

class PlayableCard : public Card
{
    private:
        typedef std::map<std::pair<uint32_t, uint32_t>, SpellAuraEffect> SpellAuraEffectsMap;

        uint64_t m_guid;
        bool m_isDefending;
        Player* m_owner;
        SpellAuraEffectsMap m_auras;
        Card const* m_morph;

        void removeExclusiveAura(uint8_t auraTypeId);

    protected:
        PlayableCard(uint64_t guid, Card const* card, Player* owner);

    public:
        virtual ~PlayableCard() { }
        static PlayableCard* Create(uint64_t guid, Card const* card, Player* owner);

        void DealDamage(uint8_t damage) { m_hp = (damage < m_hp ? m_hp - damage : 0); }
        void SetDefendState(bool defend);
        SpellAuraEffect const& ApplyAura(SpellAuraEffect const& aura);
        void RemoveAurasByType(uint8_t auraTypeId, size_t toRemove);
        void Heal(uint8_t amount);
        void AddHealth(uint8_t amount);
        void AddMana(uint8_t amount);
        void SubtractMana(uint8_t amount) { m_mana = (amount < m_mana ? m_mana - amount : 0); }
        void SetMana(uint8_t mana) { m_mana = mana; }
        void HandleTickOnAuras();
        void Morph(Card const* morph) { m_morph = morph; }
        void SetOwner(Player* player) { if (player) m_owner = player; }
        bool IsAlive() const { return GetHealth() > 0; }
        bool HasAuras() const { return !m_auras.empty(); }
        uint8_t GetModifiedDefense() const;
        uint8_t GetModifiedDamage(uint8_t additional_modifier = 0) const;
        int8_t GetStatModifierValue(CardStats stat) const;
        uint64_t GetGuid() const { return m_guid; }
        Player* GetOwner() const { return m_owner; }
        Card const* GetMorph() const { return m_morph; }
        Spell const* GetSpell() const override { return m_morph ? m_morph->GetSpell() : Card::GetSpell(); }

        virtual bool CanAttackCard(uint64_t guid, std::vector<PlayableCard*> const& opponentCards, uint32_t position) = 0;
};
