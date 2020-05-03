using Client.Logic.Data.Cards;
using System;
using System.Collections.Generic;

namespace Client.Logic
{
    public class Player
    {
        private readonly PlayableCard[] cardDeck;

        public UInt32 Id { get; }

        public string Name { get; }

        public IEnumerable<PlayableCard> CardDeck => cardDeck;

        public int ActiveCardPosition { get; private set; }

        public PlayableCard ActiveCard => cardDeck[ActiveCardPosition];

        public Player(UInt32 id, string name)
        {
            Id = id;
            Name = name;
            cardDeck = new PlayableCard[4];
        }
    }
}
