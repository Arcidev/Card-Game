#include <iostream>
#include "SpellValidator.h"
#include "SpellDefines.h"
#include "SpellEffect.h"

SpellValidator::SpellTargetValidatorFunc const SpellValidator::m_targetValidators[] =
{
    validateTargetUnitTargetEnemy,  // TARGET_UNIT_TARGET_ENEMY
    validateTargetUnitTargetFriend, // TARGET_UNIT_TARGET_FRIEND
    validateTargetUnitCleaveEnemy,  // TARGET_UNIT_CLEAVE_ENEMY
    validateTargetUnitSelf          // TARGET_UNIT_SELF
};

SpellValidator::SpellEffectValidatorFunc const SpellValidator::m_effectValidators[] =
{
    defaultEffectValidator,         // SPELL_EFFECT_DIRECT_DAMAGE
    defaultEffectValidator,         // SPELL_EFFECT_APPLY_AURA
    defaultEffectValidator,         // SPELL_EFFECT_HEAL
    defaultEffectValidator,         // SPELL_EFFECT_DRAIN
    defaultEffectValidator,         // SPELL_EFFECT_REMOVE_AURA_TYPE
    defaultEffectValidator          // SPELL_EFFECT_SWAP
};

SpellValidator::SpellAuraValidatorFunc const SpellValidator::m_auraValidators[] =
{
    nullptr,                // SPELL_AURA_EFFECT_DAMAGE
    nullptr,                // SPELL_AURA_EFFECT_MODIFY_STAT
    nullptr,                // SPELL_AURA_EFFECT_HEAL
    validateSpellAuraMorph  // SPELL_AURA_EFFECT_MORPH
};

void SpellValidator::writeTargetValidationErrorMessage(uint32_t spellId, uint32_t spellEffectId, uint32_t target, uint32_t attribute, std::string_view targetName, std::string_view attrName)
{
    std::cerr << "SpellId: " << spellId << ", SpellEffectId : " << spellEffectId << " - Attribute: " << attribute
        << "(" << attrName << ") has no effect on target: " << target << "(" << targetName << ")." << std::endl;
}

void SpellValidator::validateTargetUnitTargetEnemy(uint32_t spellEffectId, SpellEffectValues const& values)
{
    if (values.SpellAttributes & SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF)
        writeTargetValidationErrorMessage(values.SpellId, spellEffectId, values.Target, SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF, "TARGET_UNIT_TARGET_ENEMY", "SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF");
}

void SpellValidator::validateTargetUnitTargetFriend(uint32_t spellEffectId, SpellEffectValues const& values)
{
    if (values.SpellAttributes & SPELL_ATTRIBUTE_TARGET_MELEE)
        writeTargetValidationErrorMessage(values.SpellId, spellEffectId, values.Target, SPELL_ATTRIBUTE_TARGET_MELEE, "TARGET_UNIT_TARGET_FRIEND", "SPELL_ATTRIBUTE_TARGET_MELEE");
}

void SpellValidator::validateTargetUnitCleaveEnemy(uint32_t spellEffectId, SpellEffectValues const& values)
{
    if (values.SpellAttributes & SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF)
        writeTargetValidationErrorMessage(values.SpellId, spellEffectId, values.Target, SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF, "TARGET_UNIT_CLEAVE_ENEMY", "SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF");
}

void SpellValidator::validateTargetUnitSelf(uint32_t spellEffectId, SpellEffectValues const& values)
{
    if (values.SpellAttributes & SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF)
        writeTargetValidationErrorMessage(values.SpellId, spellEffectId, values.Target, SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF, "TARGET_UNIT_SELF", "SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF");

    if (values.SpellAttributes & SPELL_ATTRIBUTE_TARGET_MELEE)
        writeTargetValidationErrorMessage(values.SpellId, spellEffectId, values.Target, SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF, "TARGET_UNIT_SELF", "SPELL_ATTRIBUTE_TARGET_MELEE");
}

void SpellValidator::defaultEffectValidator(uint32_t spellEffectId, SpellEffectValues const& values)
{
    if (spellEffectId != SPELL_EFFECT_APPLY_AURA)
    {
        if (values.SpellAttributes & SPELL_ATTRIBUTE_AURA_EXCLUSIVE)
            writeAuraAttributeValidationErrorMessage(values.SpellId, spellEffectId, SPELL_ATTRIBUTE_AURA_EXCLUSIVE, "SPELL_ATTRIBUTE_AURA_EXCLUSIVE");
        
        if (values.SpellAttributes & SPELL_ATTRIBUTE_AURA_UNREMOVABLE)
            writeAuraAttributeValidationErrorMessage(values.SpellId, spellEffectId, SPELL_ATTRIBUTE_AURA_UNREMOVABLE, "SPELL_ATTRIBUTE_AURA_UNREMOVABLE");
    }
}

void SpellValidator::validateSpellAuraMorph(SpellEffectValues const& values)
{
    if (values.Target == TARGET_UNIT_TARGET_FRIEND && !(values.SpellAttributes & SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF))
    {
        std::cerr << "SpellId: " << values.SpellId << ", SpellAuraEffectId: " << SPELL_AURA_EFFECT_MORPH << " - Target: " << TARGET_UNIT_TARGET_FRIEND
            << "(TARGET_UNIT_TARGET_FRIEND) should contain attribute: " << SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF << "(SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF)." << std::endl;
    }

    if (values.Target == TARGET_UNIT_SELF)
    {
        std::cerr << "SpellId: " << values.SpellId << ", SpellAuraEffectId: " << SPELL_AURA_EFFECT_MORPH << " - Target " << TARGET_UNIT_SELF
            << "(TARGET_UNIT_SELF) makes no sense on SPELL_AURA_EFFECT_MORPH." << std::endl;
    }

    if (values.SpellAttributes & SPELL_ATTRIBUTE_AURA_EXCLUSIVE)
    {
        std::cerr << "SpellId: " << values.SpellId << ", SpellAuraEffectId: " << SPELL_AURA_EFFECT_MORPH << " - Attribute " << SPELL_ATTRIBUTE_AURA_EXCLUSIVE
            << " (SPELL_ATTRIBUTE_AURA_EXCLUSIVE) is not required on aura type SPELL_AURA_EFFECT_MORPH as it is always exclusive" << std::endl;
    }
}

void SpellValidator::writeAuraAttributeValidationErrorMessage(uint32_t spellId, uint32_t spellEffectId, uint32_t attribute, std::string_view attrName)
{
    std::cerr << "SpellId: " << spellId << ", SpellEffectId: " << spellEffectId << " - Attribute "
        << attribute << " (" << attrName << ") is only valid for SPELL_EFFECT_APPLY_AURA" << std::endl;
}

void SpellValidator::ValidateSpellEffect(uint32_t spellEffectId, SpellEffectValues const& values)
{
    if (spellEffectId < MAX_SPELL_EFFECT_VALUE)
        m_effectValidators[spellEffectId](spellEffectId, values);
    else
        std::cerr << "SpellId: " << values.SpellId << " has invalid SpellEffectId: " << spellEffectId << std::endl;

    if (values.Target < MAX_SPELL_EFFECT_TARGET)
        m_targetValidators[values.Target](spellEffectId, values);
    else
        std::cerr << "SpellId: " << values.SpellId << " has invalid target: " << (int)values.Target << " for SpellEffectId: " << spellEffectId << std::endl;

    if (spellEffectId == SPELL_EFFECT_APPLY_AURA)
    {
        if (values.Value1 >= MAX_SPELL_AURA_VALUE)
            std::cerr << "SpellId: " << values.SpellId << " has invalid SpellAuraEffectId: " << (int)values.Value1 << " for SpellEffectId:" << spellEffectId << std::endl;
        else if (m_auraValidators[values.Value1])
            m_auraValidators[values.Value1](values);
    }
}
