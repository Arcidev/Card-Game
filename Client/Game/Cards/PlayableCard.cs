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
        private static readonly Dictionary<CreatureTypes, Type> derrivedClasses = new Dictionary<CreatureTypes, Type>()
        {
            { CreatureTypes.Melee,      typeof(MeleeCard)       },
            { CreatureTypes.Ranged,     typeof(RangedCard)      },
            { CreatureTypes.Defensive,  typeof(DefensiveCard)   }
        };

        public UInt64 Guid { get; private set; }

        protected PlayableCard(UInt64 guid, UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense, Spell spell)
            : base(id, type, hp, damage, mana, defense, spell)
        {
            Guid = guid;
        }

        public static PlayableCard Create(UInt64 guid, UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense, Spell spell)
        {
            return Activator.CreateInstance(derrivedClasses[type], guid, id, type, hp, damage, mana, defense, spell) as PlayableCard;
        }

        public abstract IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex);

        public void ApplyModifier(CardStats cardStat, sbyte value)
        {
            switch(cardStat)
            {
                case CardStats.Defense:
                    DefenseModifier = value;
                    return;
                case CardStats.Damage:
                    DamageModifier = value;
                    return;
                default:
                    return;
            }
        }
    }
}
