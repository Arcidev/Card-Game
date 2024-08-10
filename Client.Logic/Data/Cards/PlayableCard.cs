using Client.Logic.Data.Spells;
using Client.Logic.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Logic.Data.Cards
{
    public abstract class PlayableCard : Card
    {
        private readonly List<SpellData> auras = [];

        /// <summary>
        /// Called when the whole card changes (f.e. with morph)
        /// </summary>
        public event Action CardChanged;

        /// <summary>
        /// Called when auras on card changes (both add/remove)
        /// </summary>
        public event Action SpellAurasChanged;

        public UInt64 Guid { get; }

        public Player Player { get; internal set; }

        public IEnumerable<SpellData> Auras => auras;

        protected PlayableCard(UInt64 guid, Card card, Player player) : base (card.Id, card.Type, card.Health, card.Damage, card.Mana, card.Defense, card.Spell)
        {
            ImageUri = card.ImageUri;
            Name = card.Name;
            Guid = guid;
            Player = player;
        }

        public static PlayableCard Create(UInt64 guid, Card card, Player player)
        {
            return card.Type switch
            {
                CreatureType.Melee => new MeleeCard(guid, card, player),
                CreatureType.Ranged => new RangedCard(guid, card, player),
                CreatureType.Defensive => new DefensiveCard(guid, card, player),
                _ => throw new ArgumentOutOfRangeException($"Invalid value {card.Type} for {nameof(CreatureType)}"),
            };
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

        internal void Morph(Card card, byte mana)
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

        internal void ApplyAura(SpellData spellData)
        {
            auras.Add(spellData);
            SpellAurasChanged?.Invoke();
        }

        internal SpellData RemoveAura(uint spellId)
        {
            var aura = auras.FirstOrDefault(x => x.SpellId == spellId);
            if (aura == null)
                return null;

            auras.Remove(aura);
            SpellAurasChanged?.Invoke();
            return aura;
        }
    }
}
