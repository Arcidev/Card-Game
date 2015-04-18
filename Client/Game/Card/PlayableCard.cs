using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Game
{
    public abstract class PlayableCard : Card
    {
        public UInt64 Guid { get; private set; }

        protected PlayableCard(UInt64 guid, UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense)
            : base(id, type, hp, damage, mana, defense)
        {
            Guid = guid;
        }

        public static PlayableCard Create(UInt64 guid, UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense)
        {
            switch(type)
            {
                case CreatureTypes.Melee:
                    return new MeleeCard(guid, id, type, hp, damage, mana, defense);
                case CreatureTypes.Ranged:
                    return new RangedCard(guid, id, type, hp, damage, mana, defense);
                case CreatureTypes.Defensive:
                    return new DefensiveCard(guid, id, type, hp, damage, mana, defense);
                default:
                    throw new InvalidOperationException("Can't create instance of invalid type");
            }
        }

        public abstract IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex);
    }
}
