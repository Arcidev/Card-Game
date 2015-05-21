using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public class SpellData
    {
        public UInt32 SpellId { get; private set; }
        public string Name { get; private set; }
        public string Description { get; private set; }
        public string SpellEffectPath { get; private set; }

        public SpellData(UInt32 spellId, string name, string description, string spellEffectPath)
        {
            SpellId = spellId;
            Name = name;
            Description = description;
            SpellEffectPath = spellEffectPath;
        }
    }
}
