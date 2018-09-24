#include <iostream>
#include "SpellValidator.h"
#include "SpellDefines.h"
#include "SpellEffect.h"

SpellAttributeValidatorFunc const SpellValidator::m_validators[] =
{
    validateTargetUnitTargetEnemy,  // TARGET_UNIT_TARGET_ENEMY
    validateTargetUnitTargetFriend, // TARGET_UNIT_TARGET_FRIEND
    validateTargetUnitCleaveEnemy   // TARGET_UNIT_CLEAVE_ENEMY
};

void SpellValidator::writeTargetValidationErrorMessage(uint32_t spellId, uint32_t spellEffectId, uint32_t target, uint32_t attribute, std::string_view targetName, std::string_view attrName)
{
    std::cerr << "SpellId: " << spellId << ", SpellEffectId : " << spellEffectId << " - Attribute " << attribute
        << "(" << attrName << ") has no effect on target " << target << "(" << targetName << ")." << std::endl;
}

void SpellValidator::validateTargetUnitTargetEnemy(uint32_t spellEffectId, SpellEffectValues const* values)
{
    if (values->SpellAttributes & SPELL_ATTRIBUTE_TARGET_SELF)
        writeTargetValidationErrorMessage(values->SpellId, spellEffectId, values->Target, SPELL_ATTRIBUTE_TARGET_SELF, "TARGET_UNIT_TARGET_ENEMY", "SPELL_ATTRIBUTE_TARGET_SELF");
}

void SpellValidator::validateTargetUnitTargetFriend(uint32_t spellEffectId, SpellEffectValues const* values)
{
    if (values->SpellAttributes & SPELL_ATTRIBUTE_TARGET_MELEE)
        writeTargetValidationErrorMessage(values->SpellId, spellEffectId, values->Target, SPELL_ATTRIBUTE_TARGET_MELEE, "TARGET_UNIT_TARGET_FRIEND", "SPELL_ATTRIBUTE_TARGET_MELEE");
}

void SpellValidator::validateTargetUnitCleaveEnemy(uint32_t spellEffectId, SpellEffectValues const* values)
{
    if (values->SpellAttributes & SPELL_ATTRIBUTE_TARGET_SELF)
        writeTargetValidationErrorMessage(values->SpellId, spellEffectId, values->Target, SPELL_ATTRIBUTE_TARGET_SELF, "TARGET_UNIT_CLEAVE_ENEMY", "SPELL_ATTRIBUTE_TARGET_SELF");
}

void SpellValidator::ValidateSpellEffect(uint32_t spellEffectId, SpellEffectValues const* values)
{
    if (values->Target >= MAX_SPELL_EFFECT_TARGET)
    {
        std::cerr << "Spell id " << values->SpellId << " has invalid target for spell effect " << spellEffectId << std::endl;
        return;
    }

    m_validators[values->Target](spellEffectId, values);
}
