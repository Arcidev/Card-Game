using Arci.Networking.Data;
using Client.Enums;
using Client.Game;

namespace Client.Network
{
    public static partial class PacketHandler
    {
        // Handle SMSG_CHAT_MESSAGE packet
        private static void HandleChatMessage(Packet packet, ClientGame game)
        {
            var chatType = (ChatTypes)packet.ReadByte();
            var senderName = packet.ReadString();
            var message = packet.ReadString();

            game.Chat.WriteChannelMessage(chatType, message, senderName);
        }

        // Handle SMSG_WHISPER_FAILED packet
        private static void HandleWhisperFailed(Packet packet, ClientGame game)
        {
            game.Chat.Write($"Player \"{packet.ReadString()}\" not found", ChatTypes.Info);
        }
    }
}
