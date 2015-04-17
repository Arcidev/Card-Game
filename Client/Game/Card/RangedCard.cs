using CardGameWPF.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CardGameWPF.Game
{
    public class RangedCard : PlayableCard
    {
        public RangedCard(UInt64 guid, UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense)
            : base(guid, id, type, hp, damage, mana, defense) { }

        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex)
        {
            enemyCards = enemyCards.Where(x => x != null);
            var possibleTargets = enemyCards.Where(x => x.Type == CreatureTypes.Defensive);
            if (possibleTargets.Any())
                return possibleTargets.Select(x => x.Guid);

            return enemyCards.Select(x => x.Guid);
        }
    }
}
