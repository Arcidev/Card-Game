using Client.Logic.Attributes;
using Client.Logic.Resources;

namespace Client.Logic.Enums
{
    public enum SelectCardFailReason
    {
        [LocalizedDescription(nameof(Texts.InvalidCardCount))]
        InvalidCardCount = 1,
        [LocalizedDescription(nameof(Texts.InvalidCardId))]
        InvalidCardId
    }
}
