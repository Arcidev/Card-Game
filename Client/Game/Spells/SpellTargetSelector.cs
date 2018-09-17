using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Game
{
    public class SpellTargetSelector
    {
        private static readonly Func<Player, Player, IEnumerable<UInt64>>[] targetSelectors =
        {
            HandleTargetUnitTargetEnemy,    // TARGET_UNIT_TARGET_ENEMY
            HandleTargetUnitFriend,         // TARGET_UNIT_TARGET_FRIEND
            HandleTargetUnitCleaveEnemy     // TARGET_UNIT_CLEAVE_ENEMY
        };

        public static Func<Player, Player, IEnumerable<UInt64>> GetTargetSelector(byte target)
        {
            if (target >= (byte)SpellTargets.MAX_SPELL_EFFECT_TARGET)
                return null;

            return targetSelectors[target];
        }

        private static IEnumerable<UInt64> HandleTargetUnitTargetEnemy(Player player, Player enemy)
        {
            return enemy.CardDeck.Where(x => x != null).Select(x => x.Guid);
        }

        private static IEnumerable<UInt64> HandleTargetUnitFriend(Player player, Player enemy)
        {
            return player.CardDeck.Where(x => x != null).Select(x => x.Guid);
        }

        private static IEnumerable<UInt64> HandleTargetUnitCleaveEnemy(Player player, Player enemy) => HandleTargetUnitTargetEnemy(player, enemy);
    }
}
