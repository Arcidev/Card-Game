using Client.Logic;
using Client.UI.ViewModels.Cards;
using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace Client.UI.ViewModels.MainGame
{
    public class PlayingGameViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;

        public ObservableCollection<PlayableCardViewModel> PlayerCards { get; }

        public ObservableCollection<PlayableCardViewModel> OpponentCards { get; }

        public PlayingGameViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            if (game.Player == null || game.Opponent == null)
                throw new InvalidOperationException("Both players must exist at this point");

            PlayerCards = new ObservableCollection<PlayableCardViewModel>(game.Player.CardDeck.Select(x => new PlayableCardViewModel(x)));
            OpponentCards = new ObservableCollection<PlayableCardViewModel>(game.Opponent.CardDeck.Select(x => new PlayableCardViewModel(x)));

            game.Player.CardDeckChanged += () => OnDeckChanged(game.Player, PlayerCards);
            game.Opponent.CardDeckChanged += () => OnDeckChanged(game.Opponent, PlayerCards);
        }

        private void OnDeckChanged(Player player, ObservableCollection<PlayableCardViewModel> cards)
        {
        }
    }
}
