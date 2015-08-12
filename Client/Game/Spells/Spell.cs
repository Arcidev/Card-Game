using System;
using System.Collections.Generic;

namespace Client.Game
{
    public class Spell
    {
        private SpellEffect[] spellEffects;
        
        public UInt32 Id { get; private set; }
        public byte ManaCost { get; private set; }
        public SpellData SpellData { get; set; }

        public string Info
        {
            get
            {
                return SpellData != null ? string.Format("{0}: {1} (Costs: {2}mana)", SpellData.Name, SpellData.Description, ManaCost) : "";
            }
        }

        public Spell(UInt32 id, byte manaCost, SpellEffect[] effets)
        {
            Id = id;
            ManaCost = manaCost;
            spellEffects = effets;
        }

        public IEnumerable<UInt64> GetPossibleTargets(Player player, Player opponent)
        {
            List<UInt64> targets = new List<ulong>();
            foreach (var effect in spellEffects)
                targets.AddRange(effect.GetPossibleTargets(player, opponent));

            return targets;
        }

        
    }
}
