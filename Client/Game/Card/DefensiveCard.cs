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
        public DefensiveCard(UInt64 guid, UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense)
            : base(guid, id, type, hp, damage, mana, defense) { }

        public override IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex)
        {
            // Can't attack => no targets
            return new List<UInt64>();
        }
    }
}
