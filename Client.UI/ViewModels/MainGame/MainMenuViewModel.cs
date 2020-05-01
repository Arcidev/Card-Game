using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using System;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.MainGame
{
    public class MainMenuViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;

        public AsyncCommandHandler StartGameCommand { get; }

        public MainMenuViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            StartGameCommand = new AsyncCommandHandler(StartGameAsync);
        }

        private async Task StartGameAsync()
        {
            await game.SendPacketAsync(new Packet(CMSGPackets.StartGame));
        }
    }
}
