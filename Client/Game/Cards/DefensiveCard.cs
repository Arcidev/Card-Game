using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
