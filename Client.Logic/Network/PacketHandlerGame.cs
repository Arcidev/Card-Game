using Arci.Networking.Data;
using Client.Logic.Resources;

namespace Client.Logic.Network
{
    public static partial class PacketHandler
    {
        // Handle HandleGameInfo packet
        private static void HandleGameInfo(Packet packet, Game game)
        {
            var hasOpponent = packet.ReadBit();
            var playerId = packet.ReadUInt32();
            game.Player.Id = playerId;

            if (hasOpponent)
            {
                var opponentId = packet.ReadUInt32();
                var opponentName = packet.ReadString();

                game.Opponent = new Player(opponentId, opponentName, game);
                game.OnInformationReceived(string.Format(Texts.PlayerJoinedGame, opponentName));
            }
            else
                game.OnInformationReceived(Texts.WaitingForPlayer);
        }

        // Handle SMSG_ACTIVE_PLAYER packet
        private static void HandleActivePlayer(Packet packet, Game game)
        {
            var cardGuid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(cardGuid, 7, 1, 2, 0, 3, 5, 4, 6);

            packet.ReadGuidByteStreamInOrder(cardGuid, 7, 5, 4, 2, 6);
            var activePlayerId = packet.ReadUInt32();
            var activePlayer = game.GetPlayer(activePlayerId);

            packet.ReadGuidByteStreamInOrder(cardGuid, 1, 0, 3);
            game.IsPlayerTurn = game.Player == activePlayer;
            activePlayer.SetActiveCard(cardGuid);
        }

        // Handle SMSG_PLAYER_DISCONNECTED packet
        private static void HandlePlayerDisconnected(Packet packet, Game game)
        {
            game.OnInformationReceived(string.Format(Texts.PlayerDisconnected, game.Opponent.Name));
        }

        // Handle SMSG_END_GAME packet
        private static void HandleEndGame(Packet packet, Game game)
        {
            //var winnerId = packet.ReadUInt32();
            //game.EndGame(game.Player.Id == winnerId);
        }
    }
}
