using System.ComponentModel;

namespace Client.Logic.Enums
{
    public enum SelectCardFailReason
    {
        [Description("Invalid card count")]
        InvalidCardCount = 1,
        [Description("Server does not support every card you've selected")]
        InvalidCardId
    }
}
