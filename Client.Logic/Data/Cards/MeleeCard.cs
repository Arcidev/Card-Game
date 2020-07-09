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
            return target != null ? new List<UInt64>() { target.Guid } : new List<UInt64>();
        }
    }
}
