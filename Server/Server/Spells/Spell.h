#pragma once
#include <list>
#include "SpellEffect.h"

typedef std::pair<SpellEffectFunc, SpellEffectValues> SpellEffectPair;

enum SpellCastResult
{
    SPELL_CAST_RESULT_SUCCESS = 0,
    SPELL_CAST_RESULT_FAIL_CANT_CAST_SPELLS,
    SPELL_CAST_RESULT_FAIL_NOT_ENOUGH_MANA,
    SPELL_CAST_RESULT_FAIL_INVALID_TARGET
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

        uint8_t Cast(Player* caster, Player* victim, uint64_t selectedCardGuid) const;
        uint8_t GetManaCost() const { return m_manaCost; }
        uint32_t GetId() const { return m_id; }
        std::list<SpellEffectPair> const& GetSpellEffects() const { return m_spellEffects; }
};
