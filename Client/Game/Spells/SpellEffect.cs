using Client.Logic.Enums;
using System;
using System.Collections.Generic;

namespace Client.Game
{
    public class SpellEffect
    {
        private readonly Func<Player, Player, SpellAttributes, IEnumerable<UInt64>> targetSelector;

        public SpellAttributes SpellAttributes { get; }

        public SpellEffect(byte targetId, SpellAttributes spellAttributes)
        {
            targetSelector = SpellTargetSelector.GetTargetSelector(targetId);
            SpellAttributes = spellAttributes;
        }

        public IEnumerable<UInt64> GetPossibleTargets(Player player, Player opponent)
        {
            return targetSelector?.Invoke(player, opponent, SpellAttributes) ?? new List<UInt64>();
        }
    }
}
