using Client.Logic.Data.Cards;
using Client.Logic.Enums;
using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.Logic
{
    public class Player
    {
        private readonly Game game;
        private readonly PlayableCard[] cardDeck;

        public UInt32 Id { get; set; }

        public string Name { get; }

        public IEnumerable<PlayableCard> CardDeck => cardDeck;

        public int ActiveCardPosition { get; private set; }

        public PlayableCard ActiveCard => cardDeck[ActiveCardPosition];

        public Player(UInt32 id, string name, Game game)
        {
            Id = id;
            Name = name;
            this.game = game;

            cardDeck = new PlayableCard[4];
        }

        internal void ModifyCardStat(UInt64 cardGuid, CardStat cardStat, sbyte value)
        {
            var card = cardDeck.FirstOrDefault(x => x.Guid == cardGuid);
            card?.ApplyModifier(cardStat, value);

            game.CombatLog.LogStatChange(cardStat, card.Name, value);
        }

        internal void HealCard(UInt64 cardGuid, byte health, byte amount)
        {
            var card = cardDeck.FirstOrDefault(x => x.Guid == cardGuid);
            if (card == null)
                return;

            card.Health = health;
            game.CombatLog.LogHeal(card.Name, amount);
        }
    }
}
