using System;
using System.Collections.Generic;

namespace Client.Game
{
    public class SpellEffect
    {
        private readonly Func<Player, Player, IEnumerable<UInt64>> targetSelector;

        public SpellEffect(byte targetId)
        {
            targetSelector = SpellTargetSelector.GetTargetSelector(targetId);
        }

        public IEnumerable<UInt64> GetPossibleTargets(Player player, Player opponent)
        {
            return targetSelector?.Invoke(player, opponent) ?? new List<UInt64>();
        }
    }
}
