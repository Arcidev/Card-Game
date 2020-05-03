using Client.Logic;
using System;

namespace Client.UI.ViewModels.MainGame
{
    public class GameViewModel : NotifyPropertyViewModel
    {
        public Game Game { get; }

        public ChatViewModel ChatViewModel { get; }

        public GameViewModel()
        {
            Game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            ChatViewModel = new ChatViewModel(Game);
        }
    }
}
