using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Views.Game;
using System;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.MainGame
{
    public class MainMenuViewModel : NotifyPropertyViewModel
    {
        public Game Game { get; }

        public AsyncCommandHandler StartGameCommand { get; }

        public MainMenuViewModel()
        {
            Game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            Game.UnsubscribeAllHandlers();

            StartGameCommand = new AsyncCommandHandler(StartGameAsync);
        }

        private async Task StartGameAsync()
        {
            await Game.SendPacketAsync(new Packet(CMSGPackets.StartGame));
        }
    }
}
