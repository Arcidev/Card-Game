#include "Spell.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

Spell::Spell(uint32_t id, uint8_t manaCost) : m_id(id), m_manaCost(manaCost) { }

uint8_t Spell::Cast(Player* caster, Player* victim, uint64_t const& selectedCardGuid) const
{
    if (!caster || !victim)
        return SPELL_FAIL_CANT_CAST_SPELLS;

    PlayableCard const* casterCard = caster->GetCurrentCard();
    if (!casterCard)
        return SPELL_FAIL_CANT_CAST_SPELLS;

    if (casterCard->GetMana() < m_manaCost)
        return SPELL_FAIL_REASON_NOT_ENOUGH_MANA;

    bool failed = true;
    for (std::list<SpellEffectPair>::const_iterator iter = m_spellEffects.begin(); iter != m_spellEffects.end(); ++iter)
        if (iter->first(caster, victim, selectedCardGuid, &iter->second) && failed)
            failed = false;

    return failed ? SPELL_FAIL_REASON_INVALID_TARGET : SPELL_FAIL_REASON_NONE;
}
