using System;
using System.Collections.Generic;

namespace Client.Game
{
    public class DefensiveCard : PlayableCard
    {
        public DefensiveCard(UInt64 guid, Card card)
            : base(guid, card) { }

        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex)
        {
            // Can't attack => no targets
            return new List<UInt64>();
        }
    }
}
