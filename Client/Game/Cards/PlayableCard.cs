using Client.Logic.Enums;
using System;
using System.Collections.Generic;

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

        public UInt64 Guid { get; }

        protected PlayableCard(UInt64 guid, Card card) : base(card.Id, card.Type, card.Hp, card.Damage, card.Mana, card.Defense, card.Spell)
        {
            ImageUri = card.ImageUri;
            Name = card.Name;
            Guid = guid;
        }

        public static PlayableCard Create(UInt64 guid, Card card)
        {
            return Activator.CreateInstance(derrivedClasses[card.Type], guid, card) as PlayableCard;
        }

        public abstract IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex);

        public void ApplyModifier(CardStat cardStat, sbyte value)
        {
            switch(cardStat)
            {
                case CardStat.Defense:
                    DefenseModifier = value;
                    return;
                case CardStat.Damage:
                    DamageModifier = value;
                    return;
                default:
                    return;
            }
        }
    }
}
