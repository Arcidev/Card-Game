using Arci.Networking.Data;
using Client.Logic.Enums;
using System;
using System.Threading.Tasks;

namespace Client.Logic
{
    public class Chat
    {
        private readonly Game game;

        public event Action<ChatType, string, string> MessageReceived;

        public event Action<ChatType, string, string> ErrorOccured;

        public Chat(Game game)
        {
            this.game = game;
        }

        public async Task SendMessage(string text, ChatType chatType, params string[] customParams)
        {
            if (string.IsNullOrWhiteSpace(text))
                return;

            var packet = new Packet(CMSGPackets.ChatMessage);
            packet.Write((byte)chatType);
            packet.Write(text.Trim());
            foreach (var p in customParams)
                packet.Write(p);

            await game.SendPacketAsync(packet);
        }

        public void OnMessageReceived(ChatType type, string sender, string msg) => MessageReceived?.Invoke(type, sender, msg);

        public void OnError(ChatType type, string sender, string error) => ErrorOccured?.Invoke(type, sender, error);

        public void UnsubscribeAllHandlers()
        {
            if (ErrorOccured != null)
            {
                foreach (var handler in ErrorOccured.GetInvocationList())
                    ErrorOccured -= handler as Action<ChatType, string, string>;
            }

            if (MessageReceived != null)
            {
                foreach (var handler in MessageReceived.GetInvocationList())
                    MessageReceived -= handler as Action<ChatType, string, string>;
            }
        }
    }
}
