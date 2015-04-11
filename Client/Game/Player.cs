using CardGameWPF.Data;
using Client.Misc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace CardGameWPF.Game
{
    public class Player
    {
        private ClientGame game;
        private Dictionary<UInt64, Card> cards;
        private Pair<Card, Image>[] cardDeck;

        public UInt32 Id { get; set; }

        public string Name { get; set; }

        public Player(ClientGame clientGame, params Image[] imageLocations)
        {
            game = clientGame;
            cardDeck = new Pair<Card, Image>[imageLocations.Length];
            for (int i = 0; i < imageLocations.Length; i++)
            {
                cardDeck[i] = new Pair<Card, Image>();
                cardDeck[i].Second = imageLocations[i];
            }
        }

        // Adds cards to player card deck
        public void AddCards(IEnumerable<Card> cardsData)
        {
            cards = DataHolder.Cards.Join(cardsData, x => x.Id, y => y.Id, (x, y) => new Card
                (y.Guid, y.Id, y.Type, y.Hp, y.Damage, y.Mana, y.Defense) 
                { ImageUri = x.ImageUri, Name = x.Name }).ToDictionary(x => x.Guid);
        }

        // Invokes action to UI thread
        public void Invoke(Action action)
        {
            game.Invoke(action);
        }

        // Put card on deck
        public void PutCardOnDeck(UInt64 cardGuid, byte position)
        {
            Card card;
            if (cards.TryGetValue(cardGuid, out card))
            {
                cardDeck[position].First = card;
                Invoke(new Action(delegate() 
                {
                    cardDeck[position].Second.Source = card.Image;
                }));
            }
        }

        public void SetActivateState(UInt64 cardGuid)
        {
            foreach(var c in cardDeck)
            {
                var select = false;
                if (c.First.Guid == cardGuid)
                    select = true;

                if (c.First.Selected != select)
                {
                    c.First.Selected = select;
                    Invoke(new Action(delegate()
                    {
                        c.Second.Source = c.First.Image;
                    }));
                }
            }
        }

        public void DeselectAllCards()
        {
            foreach (var c in cardDeck)
            {
                if (c.First.Selected)
                {
                    c.First.Selected = false;
                    Invoke(new Action(delegate()
                    {
                        c.Second.Source = c.First.Image;
                    }));
                }
            }
        }
    }
}
