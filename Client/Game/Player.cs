using CardGameWPF.Data;
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
        private Image[] cardDeck;

        public UInt32 Id { get; set; }

        public string Name { get; set; }

        public Player(ClientGame clientGame, Image card1, Image card2, Image card3, Image card4)
        {
            game = clientGame;
            cardDeck = new Image[4];
            cardDeck[0] = card1;
            cardDeck[1] = card2;
            cardDeck[2] = card3;
            cardDeck[3] = card4;
        }

        public void AddCards(IEnumerable<Card> cardsData)
        {
            cards = DataHolder.Cards.Join(cardsData, x => x.Id, y => y.Id, (x, y) => new Card
                (y.Guid, y.Id, y.Type, y.Hp, y.Damage, y.Mana, y.Defense) 
                { ImageUri = x.ImageUri, Name = x.Name }).ToDictionary(x => x.Guid);
        }

        public void Invoke(Action action)
        {
            game.Invoke(action);
        }

        public void PutCardOnDeck(UInt64 cardGuid, byte position)
        {
            Card card;
            if (cards.TryGetValue(cardGuid, out card))
            {
                Invoke(new Action(delegate() 
                {
                    cardDeck[position].Source = card.Image;
                }));
            }
        }
    }
}
