using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Resources;
using Client.UI.Sql;
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

        public async Task<Game> Login(string password, Action<UInt16> callback)
        {
            return await UserOperation(new Packet(CMSGPackets.UserLogIn).Builder().Write(Email).Write(password).Build(), callback);
        }

        protected async Task<Game> UserOperation(Packet packet, Action<UInt16> callback)
        {
            var game = App.GetGame();
            if (game == null)
            {
                game = await Game.CreateAsync(Server, new DataHolder());
                if (game == null)
                {
                    ErrorMessage = Texts.UnableToConnect;
                    return null;
                }
                App.SetGame(game);
            }

            game.MessageReceived += SetErrorMessage;
            game.PacketProcessed += callback;
            await game.SendPacketAsync(packet);
            return game;
        }

        private void SetErrorMessage(MessageType type, string msg)
        {
            if (type == MessageType.Error)
                ErrorMessage = msg;
        }
    }
}
