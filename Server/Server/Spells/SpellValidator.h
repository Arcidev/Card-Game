#pragma once
#include <cstdint>
#include <string_view>

struct SpellEffectValues;

typedef void(*SpellAttributeValidatorFunc)(uint32_t spellEffectId, SpellEffectValues const* values);

class SpellValidator
{
    private:
        static SpellAttributeValidatorFunc const m_validators[];

        static void writeTargetValidationErrorMessage(uint32_t spellId, uint32_t spellEffectId, uint32_t target, uint32_t attribute, std::string_view targetName, std::string_view attrName);

        static void validateTargetUnitTargetEnemy(uint32_t spellEffectId, SpellEffectValues const* values);
        static void validateTargetUnitTargetFriend(uint32_t spellEffectId, SpellEffectValues const* values);
        static void validateTargetUnitCleaveEnemy(uint32_t spellEffectId, SpellEffectValues const* values);

    public:
        static void ValidateSpellEffect(uint32_t spellEffectId, SpellEffectValues const* values);
};
