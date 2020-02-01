using Arci.Networking.Data;
using Client.Game;
using Client.Logic.Enums;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle SMSG_INIT_RESPONSE packet
        private static void HandleInitResponse(Packet packet, ClientGame game)
        {
            string email = string.Empty;
            string password = string.Empty;

            game.Invoke(() =>
            {
                email = game.MainWindow.UserNameBox.Text;
                password = game.MainWindow.PasswordBox.Password;
            });

            var login = new Packet(CMSGPackets.UserLogIn).Builder()
                .Write(email)
                .Write(password).Build();

            game.SendPacket(login);
        }
    }
}
