using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Resources;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.User
{
    public class LoginViewModel : NotifyPropertyViewModel
    {
        private string errorMessage;

        public string Email { get; set; }

        public string ErrorMessage
        {
            get => errorMessage;
            set
            {
                if (errorMessage == value)
                    return;

                errorMessage = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(HasError));
            }
        }

        public bool HasError => !string.IsNullOrEmpty(ErrorMessage);

        public IEnumerable<string> ServerList => Game.Servers;

        public string Server { get; set; }

        public LoginViewModel()
        {
            Server = ServerList.FirstOrDefault();
        }

        public async Task<Game> Login(string password, Action<UInt16, Game> callback)
        {
            return await UserOperation(new Packet(CMSGPackets.UserLogIn).Builder().Write(Email).Write(password).Build(), callback);
        }

        protected async Task<Game> UserOperation(Packet packet, Action<UInt16, Game> callback)
        {
            var game = App.GetGame();
            if (game == null)
            {
                game = await Game.CreateAsync(Server);
                if (game == null)
                {
                    ErrorMessage = Texts.UnableToConnect;
                    return null;
                }
                App.SetGame(game);
            }

            game.ErrorOccured += (error) => ErrorMessage = error;
            game.PacketProcessed += callback;
            game.SendPacket(packet);
            return game;
        }
    }
}
