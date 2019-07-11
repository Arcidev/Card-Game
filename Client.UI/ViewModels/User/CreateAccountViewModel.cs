using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using System;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.User
{
    public class CreateAccountViewModel : LoginViewModel
    {
        public string Username { get; set; }

        public async Task<Game> CreateAccount(string password, Action<UInt16> callback)
        {
            return await UserOperation(new Packet(CMSGPackets.UserCreate).Builder().Write(Username).Write(Email).Write(password).Build(), callback);
        }
    }
}
