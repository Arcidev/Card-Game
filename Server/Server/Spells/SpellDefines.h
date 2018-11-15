#pragma once

enum SpellAuraEffects
{
    SPELL_AURA_EFFECT_DAMAGE = 0,
    SPELL_AURA_EFFECT_MODIFY_STAT,
    SPELL_AURA_EFFECT_HEAL,
    SPELL_AURA_EFFECT_MORPH,
    MAX_SPELL_AURA_VALUE
};

enum SpellAttributes
{
    SPELL_ATTRIBUTE_TARGET_MELEE            = 0x01,
    SPELL_ATTRIBUTE_TARGET_EXCLUDE_SELF     = 0x02,
    SPELL_ATTRIBUTE_APPLY_DEFENSE           = 0x04,
    SPELL_ATTRIBUTE_AURA_EXCLUSIVE          = 0x08,
    SPELL_ATTRIBUTE_APPLY_TO_FIRST_ONLY     = 0x10
};

enum SpellTargets
{
    TARGET_UNIT_TARGET_ENEMY = 0,
    TARGET_UNIT_TARGET_FRIEND,
    TARGET_UNIT_CLEAVE_ENEMY,
    TARGET_UNIT_SELF,
    MAX_SPELL_EFFECT_TARGET
};

