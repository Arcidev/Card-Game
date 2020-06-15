using Client.Logic;
using System;

namespace Client.UI.ViewModels.MainGame
{
    public class PlayingGameViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;

        public CardDeckViewModel Player { get; }

        public CardDeckViewModel Opponent { get; }

        public PlayingGameViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            if (game.Player == null || game.Opponent == null)
                throw new InvalidOperationException("Both players must exist at this point");

            Player = new CardDeckViewModel(game.Player);
            Opponent = new CardDeckViewModel(game.Opponent);
        }
    }
}
