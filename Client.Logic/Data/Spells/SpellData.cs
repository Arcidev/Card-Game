using System;

namespace Client.Logic.Data.Spells
{
    public class SpellData
    {
        public UInt32 SpellId { get; }
        public string Name { get; }
        public string Description { get; }
        public string AuraText { get; }
        public string AuraImagePath { get; }

        public SpellData(UInt32 spellId, string name, string description, string auraText, string auraImagePath)
        {
            SpellId = spellId;
            Name = name;
            Description = description;
            AuraText = auraText;
            AuraImagePath = auraImagePath;
        }
    }
}
