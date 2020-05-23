using Client.Logic.Enums;
using System;
using System.Collections.Generic;

namespace Client.Logic.Data.Cards
{
    public abstract class PlayableCard : Card
    {
        /// <summary>
        /// Called when the whole card changes (f.e. with morph)
        /// </summary>
        public event Action CardChanged;

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

        public void Morph(Card card, byte mana)
        {
            id = card.Id;
            ImageUri = card.ImageUri;
            Type = card.Type;
            Spell = card.Spell;
            Name = card.Name;
            Damage = card.Damage;
            Defense = card.Defense;
            Mana = mana;

            CardChanged?.Invoke();
        }
    }
}
