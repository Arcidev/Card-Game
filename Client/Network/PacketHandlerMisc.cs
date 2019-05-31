using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.Game;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle SMSG_INIT_RESPONSE packet
        private static void HandleInitResponse(Packet packet, ClientGame game)
        {
            var hasOpponent = packet.ReadBit();
            var playerId = packet.ReadUInt32();
            game.Player.Id = playerId;

            string message;
            if (hasOpponent)
            {
                var opponentId = packet.ReadUInt32();
                var opponentName = packet.ReadString();
                message = $"{opponentName} has joined the game";

                game.Opponent.Id = opponentId;
                game.Opponent.Name = opponentName;
            }
            else
                message = "Waiting for another player to join the game";

            game.Chat.Write(message, ChatTypes.Info);
        }
    }
}
