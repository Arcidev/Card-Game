using Client.Data;
using Client.Enums;
using Client.Misc;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace Client.Game
{
    public class Player
    {
        private ClientGame game;
        private Dictionary<UInt64, PlayableCard> cards;
        private Pair<PlayableCard, Image>[] cardDeck;

        public UInt32 Id { get; set; }
        public string Name { get; set; }
        public bool IsActive { get; private set; }
        public int ActiveCardPosition { get; private set; }
        public PlayableCard ActiveCard { get { return cardDeck[ActiveCardPosition].First; } }
        public IEnumerable<PlayableCard> CardDeck { get { return cardDeck.Select(x => x.First); } }

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

        // put cards on deck
        public void PutCardsOnDeck(UInt64[] cardGuids)
        {
            for (int i = 0; i < cardGuids.Length; i++)
            {
                PlayableCard card;
                if (cards.TryGetValue(cardGuids[i], out card))
                {
                    Invoke(new Action(delegate()
                    {
                        cardDeck[i].First = card;
                        cardDeck[i].Second.Source = card.Image;
                    }));
                }
            }

            var availableCardsCount = cards.Count;
            for (int i = cardGuids.Length; i < cardDeck.Length; i++)
            {
                Invoke(new Action(delegate()
                {
                    cardDeck[i].First = null;
                    cardDeck[i].Second.Source = new BitmapImage(new Uri(i < availableCardsCount ? "Assets/CardBack.png" : "Assets/CardBackGrayscale.png", UriKind.Relative));
                }));
            }
        }

        // Selects possible targets attackable by basic attack on opponent deck for current player
        public void SetPossibleTargets(IEnumerable<UInt64> targetableCards, SelectionType selection)
        {
            if (!game.IsOpponent(Id))
                return;

            foreach (var card in cardDeck.Where(x => (x.First != null) && targetableCards.Contains(x.First.Guid)))
            {
                Invoke(new Action(delegate()
                {
                    card.First.SelectionType = selection;
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

                var select = SelectionType.None;
                if (c.First.Guid == cardGuid)
                {
                    ActiveCardPosition = i;
                    select = SelectionType.Selected;
                }

                if (c.First.SelectionType != select)
                {
                    Invoke(new Action(delegate()
                    {
                        c.First.SelectionType = select;
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
                if ((c.First != null) && (c.First.SelectionType != SelectionType.None))
                {
                    Invoke(new Action(delegate()
                    {
                        c.First.SelectionType = SelectionType.None;
                        c.Second.Source = c.First.Image;
                    }));
                }
            }
        }

        public PlayableCard GetCardByImageControlName(string name)
        {
            return cardDeck.First(x => x.Second.Name == name).First;
        }

        public void AttackCard(UInt64 guid, byte damage)
        {
            var cardPair = cardDeck.FirstOrDefault(x => x.First.Guid == guid);
            if (cardPair == null)
                return;

            game.Chat.LogDamage(CombatLogTypes.MeleeDamage, game.GetOpponent(Id).ActiveCard, cardPair.First, damage, true);
            Invoke(new Action(delegate()
            {
                cardPair.First.Hp -= damage;
                cardPair.Second.Source = cardPair.First.Image;
            }));
        }

        public void DestroyCard(UInt64 guid, byte damage)
        {
            PlayableCard card = null;
            if (cards.TryGetValue(guid, out card))
            {
                game.Chat.LogDamage(CombatLogTypes.MeleeDamage, game.GetOpponent(Id).ActiveCard, card, damage, false);
                cards.Remove(guid);
            }
        }
    }
}
