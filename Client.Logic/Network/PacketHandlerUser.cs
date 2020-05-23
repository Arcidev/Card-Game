using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.Logic;
using Client.Logic.Resources;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        /// <summary>
        /// Handles SMSG_USER_RESULT
        /// </summary>
        /// <param name="packet">Packet to be processed</param>
        /// <param name="game">Instance of game</param>
        private static void HandleUserResult(Packet packet, Game game)
        {
            var result = (UserResult)packet.ReadByte();

            switch(result)
            {
                case UserResult.LogedIn:
                    game.Player = new Player(packet.ReadUInt32(), packet.ReadString(), game);
                    return;
                case UserResult.PasswordChanged:
                    game.OnErrorOccured(Texts.PasswordChanged);
                    return;
                case UserResult.InvalidCredentials:
                    game.OnErrorOccured(Texts.InvalidCredentials);
                    return;
                case UserResult.EmailInUse:
                    game.OnErrorOccured(Texts.EmailInUse);
                    return;
                case UserResult.UsernameInUse:
                    game.OnErrorOccured(Texts.UsernameInUse);
                    return;
                case UserResult.UnknownError:
                    game.OnErrorOccured(Texts.UnknownError);
                    return;
            }
        }
    }
}
