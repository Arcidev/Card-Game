using System.ComponentModel;

namespace Client.Logic.Enums
{
    public enum SpellCastResult
    {
        Success = 0,
        [Description("Current card has no spell")]
        FailCantCastSpell,
        [Description("Current card has not enough mana")]
        FailNotEnoughMana,
        [Description("Current card cannot use spell on that target")]
        FailInvalidTarget
    };
}
