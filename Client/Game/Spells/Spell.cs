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
        public string Info => SpellData != null ? $"{SpellData.Name}: {SpellData.Description} (Costs: {ManaCost}mana)" : "";

        public Spell(UInt32 id, byte manaCost, SpellEffect[] effets)
        {
            Id = id;
            ManaCost = manaCost;
            spellEffects = effets;
        }

        public IEnumerable<UInt64> GetPossibleTargets(Player player, Player opponent)
        {
            var targets = new List<ulong>();
            foreach (var effect in spellEffects)
                targets.AddRange(effect.GetPossibleTargets(player, opponent));

            return targets;
        }

        
    }
}
