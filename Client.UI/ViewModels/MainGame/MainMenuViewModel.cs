using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using System;

namespace Client.UI.ViewModels.MainGame
{
    public class MainMenuViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;

        public AsyncCommandHandler StartGameCommand { get; }

        public AsyncCommandHandler AchievementsCommand { get; }

        public MainMenuViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            StartGameCommand = new(async () => await game.SendPacketAsync(new(CMSGPackets.StartGame)));
            AchievementsCommand = new(async () => await game.SendPacketAsync(new(CMSGPackets.GetAchievements)));
        }
    }
}
