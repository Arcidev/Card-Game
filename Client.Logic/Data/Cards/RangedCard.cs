using Client.Logic.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Logic.Data.Cards
{
    public class RangedCard : PlayableCard
    {
        public RangedCard(UInt64 guid, Card card) : base(guid, card) { }

        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex)
        {
            enemyCards = enemyCards.Where(x => x != null);
            var possibleTargets = enemyCards.Where(x => x.Type == CreatureType.Defensive);
            if (possibleTargets.Any())
                return possibleTargets.Select(x => x.Guid);

            return enemyCards.Select(x => x.Guid);
        }
    }
}
