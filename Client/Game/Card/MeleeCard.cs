using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public class MeleeCard : PlayableCard
    {
        public MeleeCard(UInt64 guid, UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense)
            : base(guid, id, type, hp, damage, mana, defense) { }

        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex)
        {
            var target = enemyCards.ElementAt(currentCardIndex);
            return target != null ? new List<UInt64>() { target.Guid } : new List<UInt64>();
        }
    }
}
