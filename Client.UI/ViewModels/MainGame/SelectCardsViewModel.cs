using Client.Logic;
using System;

namespace Client.UI.ViewModels.MainGame
{
    public class SelectCardsViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;

        public SelectCardsViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            game.UnsubscribeAllHandlers();

        }
    }
}
