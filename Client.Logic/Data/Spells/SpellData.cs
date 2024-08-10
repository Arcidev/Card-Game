using System;

namespace Client.Logic.Data.Spells
{
    public record SpellData(UInt32 SpellId, string Name, string Description, string AuraText, string AuraImagePath)
    {
      
    }
}
