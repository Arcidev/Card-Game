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
        private Dictionary<UInt64, PlayableCard> cards;
        private Pair<PlayableCard, Image>[] cardDeck;

        public UInt32 Id { get; set; }
        public string Name { get; set; }
        public bool IsActive { get; private set; }

        public Player(ClientGame clientGame, params Image[] imageLocations)
        {
            game = clientGame;
            cardDeck = new Pair<PlayableCard, Image>[imageLocations.Length];
            for (int i = 0; i < imageLocations.Length; i++)
            {
                cardDeck[i] = new Pair<PlayableCard, Image>();
                cardDeck[i].Second = imageLocations[i];
            }
        }

        // Adds cards to player card deck
        public void AddCards(IEnumerable<PlayableCard> cardsData)
        {
            foreach(var card in cardsData)
            {
                var cardTemplate = DataHolder.Cards.First(x => x.Id == card.Id);
                card.ImageUri = cardTemplate.ImageUri;
                card.Name = cardTemplate.Name;
            }

            cards = cardsData.ToDictionary(x => x.Guid);
        }

        // Invokes action to UI thread
        public void Invoke(Action action)
        {
            game.Invoke(action);
        }

        // Put card on deck
        public void PutCardOnDeck(UInt64 cardGuid, byte position)
        {
            PlayableCard card;
            if (cards.TryGetValue(cardGuid, out card))
            {
                cardDeck[position].First = card;
                Invoke(new Action(delegate() 
                {
                    cardDeck[position].Second.Source = card.Image;
                }));
            }
        }

        // Selects possible targets on opponent deck for current player
        public void SetPossibleTargets(IEnumerable<UInt64> targetableCards)
        {
            if (!game.IsOpponent(Id))
                return;

            foreach (var card in cardDeck.Where(x => (x.First != null) && targetableCards.Contains(x.First.Guid)))
            {
                card.First.Selected = true;
                Invoke(new Action(delegate()
                {
                    card.Second.Source = card.First.Image;
                }));
            }
        }

        // Set active state
        public void SetActiveState(UInt64 cardGuid)
        {
            IsActive = true;
            for(int i = 0; i < cardDeck.Length; i++)
            {
                var c = cardDeck[i];
                if (c.First == null)
                    continue;

                var select = false;
                if (c.First.Guid == cardGuid)
                {
                    var opponent = game.GetOpponent(Id);
                    if (opponent != null)
                        opponent.SetPossibleTargets(c.First.GetPossibleTargets(opponent.cardDeck.Select(x => x.First), i));
                    select = true;
                }

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

        // Sets waiting state
        public void SetWaitingState()
        {
            IsActive = false;
            foreach (var c in cardDeck)
            {
                if ((c.First != null) && c.First.Selected)
                {
                    c.First.Selected = false;
                    Invoke(new Action(delegate()
                    {
                        c.Second.Source = c.First.Image;
                    }));
                }
            }
        }

        public PlayableCard GetCardByImageControlName(string name)
        {
            return cardDeck.First(x => x.Second.Name == name).First;
        }
    }
}
