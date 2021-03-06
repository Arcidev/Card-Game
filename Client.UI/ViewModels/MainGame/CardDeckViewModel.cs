﻿using Client.Logic;
using Client.UI.ViewModels.Cards;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;

namespace Client.UI.ViewModels.MainGame
{
    public class CardDeckViewModel : NotifyPropertyViewModel
    {
        private readonly Player player;

        public ObservableCollection<PlayableCardViewModel> Cards { get; }

        public CardDeckViewModel(Player player)
        {
            this.player = player;
            Cards = new (player.CardDeck.Where(x => x != null).Select(x => new PlayableCardViewModel(x)));

            player.CardDeckChanged += OnDeckChanged;
        }

        private void OnDeckChanged()
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                Cards.Clear();
                foreach (var card in player.CardDeck.Where(x => x != null).Select(x => new PlayableCardViewModel(x)))
                {
                    Cards.Add(card);
                }
            });
        }
    }
}
