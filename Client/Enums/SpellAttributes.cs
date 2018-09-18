using System;

namespace Client.Enums
{
    [Flags]
    public enum SpellAttributes
    {
        None            = 0x00,
        TargetMelee     = 0x01,
        TargetSelf      = 0x02,
        HideTarget      = 0x04
    };
}
