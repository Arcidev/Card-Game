using System;
using System.Collections.Generic;

namespace Client.Game
{
    public class Spell
    {
        private readonly SpellEffect[] spellEffects;
        
        public UInt32 Id { get; }
        public byte ManaCost { get; }
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
            var targets = new List<UInt64>();
            foreach (var effect in spellEffects)
                targets.AddRange(effect.GetPossibleTargets(player, opponent));

            return targets;
        }

        
    }
}
