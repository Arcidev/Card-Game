using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Game
{
    public class MeleeCard : PlayableCard
    {
        public MeleeCard(UInt64 guid, Card card)
            : base(guid, card) { }

        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex)
        {
            var target = enemyCards.ElementAt(currentCardIndex);
            return target != null ? new List<UInt64>() { target.Guid } : new List<UInt64>();
        }
    }
}
