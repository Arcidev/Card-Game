using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public class SpellTargetSelector
    {
        private static readonly Func<Player, Player, IEnumerable<UInt64>>[] targetSelectors =
        {
            handleTargetUnitCaster,         // TARGET_UNIT_CASTER
            handleTargetUnitTargetEnemy,    // TARGET_UNIT_TARGET_ENEMY
            handleTargetUnitFriend          // TARGET_UNIT_TARGET_FRIEND
        };

        private static IEnumerable<UInt64> handleTargetUnitCaster(Player player, Player enemy)
        {
            return new[] { player.ActiveCard.Guid };
        }

        private static IEnumerable<UInt64> handleTargetUnitTargetEnemy(Player player, Player enemy)
        {
            return enemy.CardDeck.Where(x => x != null).Select(x => x.Guid);
        }

        private static IEnumerable<UInt64> handleTargetUnitFriend(Player player, Player enemy)
        {
            return player.CardDeck.Where(x => x != null).Select(x => x.Guid);
        }

        public static Func<Player, Player, IEnumerable<UInt64>> GetTargetSelector(byte target)
        {
            if (target > (byte)SpellTargets.MAX_SPELL_EFFECT_TARGET)
                return null;

            return targetSelectors[target];
        }
    }
}
