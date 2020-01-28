using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.Game;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle HandleGameInfo packet
        private static void HandleGameInfo(Packet packet, ClientGame game)
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

        // Handle SMSG_ACTIVE_PLAYER packet
        private static void HandleActivePlayer(Packet packet, ClientGame game)
        {
            var cardGuid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(cardGuid, 7, 1, 2, 0, 3, 5, 4, 6);

            packet.ReadGuidByteStreamInOrder(cardGuid, 7, 5, 4, 2, 6);
            var activePlayerId = packet.ReadUInt32();
            var activePlayer = (game.Player.Id == activePlayerId) ? game.Player : game.Opponent;
            var nonActivePlayer = (game.Player.Id == activePlayerId) ? game.Opponent : game.Player;

            packet.ReadGuidByteStreamInOrder(cardGuid, 1, 0, 3);
            nonActivePlayer.SetWaitingState();
            activePlayer.SetActiveState(cardGuid);
            game.SetActiveCardActionGrid(game.Player.Id == activePlayerId);
        }

        // Handle SMSG_PLAYER_DISCONNECTED packet
        private static void HandlePlayerDisconnected(Packet packet, ClientGame game)
        {
            game.Chat.Write($"Player \"{game.Opponent.Name}\" has disconnected", ChatTypes.Info);
        }

        // Handle SMSG_END_GAME packet
        private static void HandleEndGame(Packet packet, ClientGame game)
        {
            var winnerId = packet.ReadUInt32();
            game.EndGame(game.Player.Id == winnerId);
        }
    }
}
