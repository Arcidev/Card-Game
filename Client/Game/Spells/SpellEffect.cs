using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public class SpellEffect
    {
        private Func<Player, Player, IEnumerable<UInt64>> targetSelector;

        public SpellEffect(byte targetId)
        {
            targetSelector = SpellTargetSelector.GetTargetSelector(targetId);
        }

        public IEnumerable<UInt64> GetPossibleTargets(Player player, Player opponent)
        {
            if (targetSelector == null)
                return new List<UInt64>();

            return targetSelector(player, opponent);
        }
    }
}
