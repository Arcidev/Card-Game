using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Logic.Data.Cards
{
    public class MeleeCard : PlayableCard
    {
        public MeleeCard(UInt64 guid, Card card, Player player) : base(guid, card, player) { }

        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex)
        {
            var target = enemyCards.ElementAt(currentCardIndex);
            return target != null ? [target.Guid] : [];
        }
    }
}
