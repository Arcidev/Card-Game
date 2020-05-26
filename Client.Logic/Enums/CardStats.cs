using Client.Logic.Attributes;
using Client.Logic.Resources;

namespace Client.Logic.Enums
{
    public enum CardStat
    {
        [LocalizedDescription(nameof(Texts.Defense))]
        Defense = 1,

        [LocalizedDescription(nameof(Texts.Damage))]
        Damage,

        [LocalizedDescription(nameof(Texts.Health))]
        Health,

        [LocalizedDescription(nameof(Texts.Mana))]
        Mana
    }
}
