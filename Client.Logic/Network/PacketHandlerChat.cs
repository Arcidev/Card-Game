using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.Logic;
using Client.Logic.Resources;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle SMSG_CHAT_MESSAGE packet
        private static void HandleChatMessage(Packet packet, Game game)
        {
            var chatType = (ChatType)packet.ReadByte();
            var senderName = packet.ReadString();
            var message = packet.ReadString();

            game.Chat.OnMessageReceived(chatType, senderName, message);
        }

        // Handle SMSG_WHISPER_FAILED packet
        private static void HandleWhisperFailed(Packet packet, Game game)
        {
            var playerName = packet.ReadString();
            game.Chat.OnError(ChatType.Whisper, playerName, string.Format(Texts.PlayerNotFound, playerName));
        }
    }
}
