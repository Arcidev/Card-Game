using Arci.Networking.Data;
using Client.Enums;
using Client.Game;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle SMSG_USER_RESULT packet
        private static void HandleUserResult(Packet packet, ClientGame game)
        {
            var result = (UserResult)packet.ReadByte();
            if (result == UserResult.LogedIn)
            {
                game.Player.Id = packet.ReadUInt32();
                game.Player.Name = packet.ReadString();

                /// TODO: proceed with login

                return;
            }

            if (result == UserResult.PasswordChanged)
            {
                game.Chat.Write("Password changed successfully", ChatTypes.Info);
                return;
            }

            if (result == UserResult.InvalidCredentials)
            {
                if (game.Player.Id != 0)
                    game.Chat.Write("Invalid credentials", ChatTypes.Info);

                /// TODO: Show error on login page

                return;
            }

            if (result == UserResult.EmailInUse)
            {
                /// TODO: Show error on login page
                return;
            }

            if (result == UserResult.UserNameInUse)
            {
                /// TODO: Show error on login page
                return;
            }

            if (result == UserResult.UnknownError)
                game.Chat.Write("Unknown error occured", ChatTypes.Info);
        }
    }
}
