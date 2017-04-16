using Arci.Networking.Data;
using Client.Enums;
using Client.Game;
using System;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle SMSG_ACTIVE_PLAYER packet
        private static void HandleActivePlayer(Packet packet, ClientGame game)
        {
            var cardGuid = new PacketGuid();
            packet.ReadGuidBitStreamInOrder(cardGuid, 7, 1, 2, 0, 3, 5, 4, 6);

            packet.ReadGuidByteStreamInOrder(cardGuid, 7, 5, 4, 2, 6);
            var activePlayerId = packet.ReadUInt32();
            Player activePlayer = (game.Player.Id == activePlayerId) ? game.Player : game.Opponent;
            Player nonActivePlayer = (game.Player.Id == activePlayerId) ? game.Opponent : game.Player;

            packet.ReadGuidByteStreamInOrder(cardGuid, 1, 0, 3);
            nonActivePlayer.SetWaitingState();
            activePlayer.SetActiveState(cardGuid);
            game.SetActiveCardActionGrid(game.Player.Id == activePlayerId);
        }

        // Handle SMSG_PLAYER_DISCONNECTED packet
        private static void HandlePlayerDisconnected(Packet packet, ClientGame game)
        {
            game.Chat.Write(string.Format("Player \"{0}\" has disconnected", game.Opponent.Name), ChatTypes.Info);
        }

        // Handle SMSG_END_GAME packet
        private static void HandleEndGame(Packet packet, ClientGame game)
        {
            UInt32 winnerId = packet.ReadUInt32();
            game.EndGame(game.Player.Id == winnerId);
        }
    }
}
