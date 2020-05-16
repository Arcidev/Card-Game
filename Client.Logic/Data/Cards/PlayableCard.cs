using Client.Logic.Enums;
using System;
using System.Collections.Generic;

namespace Client.Logic.Data.Cards
{
    public abstract class PlayableCard : Card
    {
        public UInt64 Guid { get; }

        public PlayableCard(UInt64 guid, Card card) : base (card.Id, card.Type, card.Health, card.Damage, card.Mana, card.Defense, card.Spell)
        {
            ImageUri = card.ImageUri;
            Name = card.Name;
            Guid = guid;

        }

        private static readonly Dictionary<CreatureType, Type> derrivedClasses = new Dictionary<CreatureType, Type>()
        {
            { CreatureType.Melee,      typeof(MeleeCard)       },
            { CreatureType.Ranged,     typeof(RangedCard)      },
            { CreatureType.Defensive,  typeof(DefensiveCard)   }
        };

        public static PlayableCard Create(UInt64 guid, Card card)
        {
            return Activator.CreateInstance(derrivedClasses[card.Type], guid, card) as PlayableCard;
        }

        public abstract IEnumerable<UInt64> GetPossibleTargets(IEnumerable<PlayableCard> enemyCards, int currentCardIndex);

        public void ApplyModifier(CardStat cardStat, sbyte value)
        {
            switch (cardStat)
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
