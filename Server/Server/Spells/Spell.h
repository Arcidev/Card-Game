#pragma once
#include <list>
#include "SpellDefines.h"
#include "SpellEffect.h"

typedef std::pair<SpellEffectFunc, SpellEffectValues> SpellEffectPair;

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

        SpellCastResult Cast(Player* caster, Player* victim, uint64_t selectedCardGuid) const;
        uint8_t GetManaCost() const { return m_manaCost; }
        uint32_t GetId() const { return m_id; }
        std::list<SpellEffectPair> const& GetSpellEffects() const { return m_spellEffects; }
};
