using System.ComponentModel;

namespace Client.Enums
{
    public enum SpellFailReason
    {
        SPELL_FAIL_REASON_NONE = 0,
        [Description("Current card has no spell")]
        SPELL_FAIL_CANT_CAST_SPELLS,
        [Description("Current card has not enough mana")]
        SPELL_FAIL_REASON_NOT_ENOUGH_MANA,
        [Description("Current card cannot use spell on that target")]
        SPELL_FAIL_REASON_INVALID_TARGET
    };
}
