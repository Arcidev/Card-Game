using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public class Spell
    {
        private SpellEffect[] spellEffects;

        public UInt32 Id { get; set; }
        public byte ManaCost { get; set; }

        public Spell(UInt32 id, byte manaCost, SpellEffect[] effets)
        {
            Id = id;
            ManaCost = manaCost;
            spellEffects = effets;
        }
    }
}
