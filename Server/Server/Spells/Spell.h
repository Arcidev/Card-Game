#pragma once
#include "SpellEffect.h"

typedef std::pair<SpellEffectFunc, SpellEffectValues> SpellEffectPair;

enum SpellFailReason
{
    SPELL_FAIL_REASON_NONE = 0,
    SPELL_FAIL_CANT_CAST_SPELLS,
    SPELL_FAIL_REASON_NOT_ENOUGH_MANA,
    SPELL_FAIL_REASON_INVALID_TARGET
};

class Spell
{
    friend class DataHolder;

    private:
        uint32_t m_id;
        uint8_t m_manaCost;
        std::list<SpellEffectPair> m_spellEffects;

        void addSpellEffect(SpellEffectPair spellEffect) { m_spellEffects.push_back(spellEffect); }

    public:
        Spell(uint32_t id, uint8_t manaCost);

        uint8_t Cast(Player* caster, Player* victim, uint64_t const& selectedCardGuid) const;
        uint8_t const& GetManaCost() const { return m_manaCost; }
        uint32_t const& GetId() const { return m_id; }
        std::list<SpellEffectPair> const& GetSpellEffects() const { return m_spellEffects; }
};
