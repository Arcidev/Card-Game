using System;

namespace Client.Logic.Enums
{
    [Flags]
    public enum SpellAttributes
    {
        None                = 0x00,
        TargetMelee         = 0x01,
        TargetExcludeSelf   = 0x02,
        ApplyDefense        = 0x04,
        AuraExclusive       = 0x08,
        ContinueMove        = 0x10
    };
}
