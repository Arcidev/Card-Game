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
            var login = new Packet(CMSGPackets.UserLogIn).Builder()
                .Write(game.MainWindow.UserNameBox.Text)
                .Write(game.MainWindow.PasswordBox.Password).Build();

            game.SendPacket(login);
        }
    }
}
