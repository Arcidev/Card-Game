using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Game
{
    public class SpellTargetSelector
    {
        private static readonly Func<Player, Player, SpellAttributes, IEnumerable<UInt64>>[] targetSelectors =
        {
            HandleTargetUnitTargetEnemy,    // TARGET_UNIT_TARGET_ENEMY
            HandleTargetUnitFriend,         // TARGET_UNIT_TARGET_FRIEND
            HandleTargetUnitCleaveEnemy     // TARGET_UNIT_CLEAVE_ENEMY
        };

        public static Func<Player, Player, SpellAttributes, IEnumerable<UInt64>> GetTargetSelector(byte target)
        {
            if (target >= (byte)SpellTargets.MaxSpellTarget)
                return null;

            return targetSelectors[target];
        }

        private static IEnumerable<UInt64> HandleTargetUnitTargetEnemy(Player player, Player enemy, SpellAttributes attributes)
        {
            if ((attributes & SpellAttributes.TargetMelee) == SpellAttributes.None)
                return enemy.CardDeck.Where(x => x != null).Select(x => x.Guid);

            var card = enemy.CardDeck.ElementAtOrDefault(player.ActiveCardPosition);
            return card != null ? new List<UInt64>() { card.Guid } : new List<UInt64>();
        }

        private static IEnumerable<UInt64> HandleTargetUnitFriend(Player player, Player enemy, SpellAttributes attributes)
        {
            if ((attributes & SpellAttributes.TargetSelf) == SpellAttributes.None)
                return player.CardDeck.Where(x => x != null).Select(x => x.Guid);

            var card = player.ActiveCard;
            return card != null ? new List<UInt64>() { card.Guid } : new List<UInt64>();
        }

        private static IEnumerable<UInt64> HandleTargetUnitCleaveEnemy(Player player, Player enemy, SpellAttributes attributes) => HandleTargetUnitTargetEnemy(player, enemy, attributes);
    }
}
