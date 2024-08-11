#pragma once
#include <cstdint>
#include <string_view>

struct SpellEffectValues;

class SpellValidator
{
    private:
        typedef void(*SpellTargetValidatorFunc)(uint32_t spellEffectId, SpellEffectValues const& values);
        typedef void(*SpellEffectValidatorFunc)(uint32_t spellEffectId, SpellEffectValues const& values);
        typedef void(*SpellAuraValidatorFunc)(SpellEffectValues const& values);

        static SpellTargetValidatorFunc const m_targetValidators[];
        static SpellEffectValidatorFunc const m_effectValidators[];
        static SpellAuraValidatorFunc const m_auraValidators[];

        // Target validators
        static void writeTargetValidationErrorMessage(uint32_t spellId, uint32_t spellEffectId, uint32_t target, uint32_t attribute, std::string_view targetName, std::string_view attrName);
        static void validateTargetUnitTargetEnemy(uint32_t spellEffectId, SpellEffectValues const& values);
        static void validateTargetUnitTargetFriend(uint32_t spellEffectId, SpellEffectValues const& values);
        static void validateTargetUnitCleaveEnemy(uint32_t spellEffectId, SpellEffectValues const& values);
        static void validateTargetUnitSelf(uint32_t spellEffectId, SpellEffectValues const& values);

        // Effect validators
        static void defaultEffectValidator(uint32_t spellEffectId, SpellEffectValues const& values);

        // Aura validators
        static void validateSpellAuraMorph(SpellEffectValues const& values);
        static void writeAuraAttributeValidationErrorMessage(uint32_t spellId, uint32_t spellEffectId, uint32_t attribute, std::string_view attrName);

    public:
        static void ValidateSpellEffect(uint32_t spellEffectId, SpellEffectValues const& values);
};
