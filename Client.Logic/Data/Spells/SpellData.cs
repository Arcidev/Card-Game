using System;

namespace Client.Logic.Data.Spells
{
    public class SpellData
    {
        public UInt32 SpellId { get; }
        public string Name { get; }
        public string Description { get; }

        public SpellData(UInt32 spellId, string name, string description)
        {
            SpellId = spellId;
            Name = name;
            Description = description;
        }
    }
}
