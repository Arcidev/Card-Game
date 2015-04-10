using CardGameWPF.Enums;
using CardGameWPF.Network;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace CardGameWPF.Game
{
    public class ChatHandler
    {
        private ClientGame clientGame;

        public ChatHandler(ClientGame game)
        {
            clientGame = game;
            ActiveChat = ChatTypes.Global;
        }

        // Currently active chat
        public ChatTypes ActiveChat { get; set; }

        // Returns chat color
        public Brush GetChatColor(ChatTypes chatType)
        {
            switch (chatType)
            {
                case ChatTypes.Game:
                    return Brushes.Blue;
                case ChatTypes.Global:
                    return Brushes.Green;
                case ChatTypes.Whisper:
                case ChatTypes.WhisperResponse:
                    return Brushes.Purple;
                case ChatTypes.Info:
                    return Brushes.Orange;
                default:
                    return Brushes.Black;
            }
        }

        // Writes message into chat
        public void Write(string message, ChatTypes chatType)
        {
            Brush brush = GetChatColor(chatType);
            RichTextBox rtb = clientGame.MainWindow.GeneralChatTab;
            clientGame.Invoke(new Action(delegate()
            {
                TextRange tr = new TextRange(rtb.Document.ContentEnd, rtb.Document.ContentEnd);
                tr.Text = string.Format("{0}{1}", message, CommandHandler.LineSeparator);
                tr.ApplyPropertyValue(TextElement.ForegroundProperty, brush);
                rtb.ScrollToEnd();
            }));
        }

        // Writes message as channel message into chat
        public void WriteChannelMessage(ChatTypes chatType, string message, string sender)
        {
            Write(string.Format("[{0}] {1}> {2}", chatType.GetDescription(), sender, message), chatType);
        }

        // Sends chat message to server
        public void SendChatMessage(string text, ChatTypes chatType, params string[] customParams)
        {
            if (string.IsNullOrWhiteSpace(text))
                return;

            Packet packet = new Packet(CMSGPackets.CMSG_CHAT_MESSAGE);
            packet.Write((byte)chatType);
            packet.Write(text.Trim());
            foreach (var p in customParams)
                packet.Write(p);

            clientGame.SendPacket(packet);
        }

        // Sets active chat
        public void SetActiveChat(ChatTypes chatType)
        {
            ActiveChat = chatType;
            clientGame.Invoke(new Action(delegate()
            {
                clientGame.MainWindow.ChatActiveLabel.Content = chatType;
                clientGame.MainWindow.ChatActiveLabel.Foreground = GetChatColor(chatType);
            }));
        }
    }
}
