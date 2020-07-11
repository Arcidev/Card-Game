using Client.Logic.Attributes;
using Client.Logic.Resources;

namespace Client.Logic.Enums
{
    public enum SpellCastResult
    {
        Success = 0,
        [LocalizedDescription(nameof(Texts.CantCastSpell))]
        FailCantCastSpell,
        [LocalizedDescription(nameof(Texts.NotEnoughMana))]
        FailNotEnoughMana,
        [LocalizedDescription(nameof(Texts.InvalidSpellTarget))]
        FailInvalidTarget
    };
}
