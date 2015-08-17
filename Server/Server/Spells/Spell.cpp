#include "Spell.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

Spell::Spell(uint32_t id, uint8_t manaCost) : m_id(id), m_manaCost(manaCost) { }

uint8_t Spell::Cast(Player* caster, Player* victim, uint64_t const& selectedCardGuid) const
{
    if (!caster || !victim)
        return SPELL_CAST_RESULT_FAIL_CANT_CAST_SPELLS;

    PlayableCard const* casterCard = caster->GetCurrentCard();
    if (!casterCard)
        return SPELL_CAST_RESULT_FAIL_CANT_CAST_SPELLS;

    if (casterCard->GetMana() < m_manaCost)
        return SPELL_CAST_RESULT_FAIL_NOT_ENOUGH_MANA;

    bool failed = true;
    for (SpellEffectPair const& effectPair : m_spellEffects)
        if (effectPair.first(caster, victim, selectedCardGuid, &effectPair.second) && failed)
            failed = false;

    return failed ? SPELL_CAST_RESULT_FAIL_INVALID_TARGET : SPELL_CAST_RESULT_SUCCESS;
}
