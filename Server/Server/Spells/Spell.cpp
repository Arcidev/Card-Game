#include "Spell.h"
#include "../Player.h"
#include "../Cards/PlayableCard.h"

Spell::Spell(uint32_t id, uint8_t manaCost) : m_id(id), m_manaCost(manaCost) { }

SpellCastResult Spell::Cast(Player* caster, Player* victim, uint64_t selectedCardGuid) const
{
    if (!caster || !victim)
        return SpellCastResult::SPELL_CAST_RESULT_FAIL_CANT_CAST_SPELLS;

    PlayableCard const* casterCard = caster->GetCurrentCard();
    if (!casterCard)
        return SpellCastResult::SPELL_CAST_RESULT_FAIL_CANT_CAST_SPELLS;

    if (casterCard->GetMana() < m_manaCost)
        return SpellCastResult::SPELL_CAST_RESULT_FAIL_NOT_ENOUGH_MANA;

    SpellCastResult result = SpellCastResult::SPELL_CAST_RESULT_FAIL_INVALID_TARGET;
    for (SpellEffectPair const& effectPair : m_spellEffects)
    {
        if (effectPair.first(caster, victim, selectedCardGuid, effectPair.second) && result != SpellCastResult::SPELL_CAST_RESULT_SUCCESS)
            result = effectPair.second.SpellAttributes & SpellAttributes::SPELL_ATTRIBUTE_CONTINUE_MOVE ? SpellCastResult::SPELL_CAST_RESULT_SUCCESS_CONTINUE_MOVE : SpellCastResult::SPELL_CAST_RESULT_SUCCESS;
    }

    return result;
}
