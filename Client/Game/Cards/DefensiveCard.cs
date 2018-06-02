using System;
using System.Collections.Generic;

namespace Client.Game
{
    public class DefensiveCard : PlayableCard
    {
        public DefensiveCard(UInt64 guid, Card card) : base(guid, card) { }

        // Can't attack => no targets
        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex) => new List<UInt64>();
    }
}
