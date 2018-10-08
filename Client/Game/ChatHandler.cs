using Arci.Networking.Data;
using Client.Enums;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace Client.Game
{
    public class ChatHandler
    {
        private ClientGame clientGame;

        // Currently active chat
        public ChatTypes ActiveChat { get; set; }

        public ChatHandler(ClientGame game)
        {
            clientGame = game;
            ActiveChat = ChatTypes.Global;
        }

        // Writes message into chat
        public void Write(string message, ChatTypes chatType)
        {
            InvokeToChat(message, clientGame.MainWindow.GeneralChatTab, GetChatColor(chatType));
        }

        // Writes message as channel message into chat
        public void WriteChannelMessage(ChatTypes chatType, string message, string sender)
        {
            Write($"[{chatType.GetDescription()}] {sender}> {message}", chatType);
        }

        // Sends chat message to server
        public void SendChatMessage(string text, ChatTypes chatType, params string[] customParams)
        {
            if (string.IsNullOrWhiteSpace(text))
                return;

            var packet = new Packet(CMSGPackets.CMSG_CHAT_MESSAGE);
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
            clientGame.Invoke(() =>
            {
                clientGame.MainWindow.ChatActiveLabel.Content = chatType;
                clientGame.MainWindow.ChatActiveLabel.Foreground = GetChatColor(chatType);
            });
        }

        // Writes message into combat log
        public void WriteLog(string message, CombatLogTypes combatLogType)
        {
            InvokeToChat(message, clientGame.MainWindow.CombatLogChatTab, GetCombatLogColor(combatLogType));
        }

        // Write info about damage into combat log chat tab
        public void LogDamage(CombatLogTypes combatLogType, PlayableCard attacker, PlayableCard victim, byte damage, bool alive)
        {
            var message = alive ? $"{attacker.Name} dealt {damage} damage to {victim.Name}" : $"{attacker.Name} killed {victim.Name} with {damage}";
            WriteLog(message, combatLogType);
        }

        // Write info about periodic damage into combat log chat tab
        public void LogPeriodicDamage(PlayableCard victim, byte damage, bool alive)
        {
            var message = alive ? $"{victim.Name} suffered {damage} damage" : $"{victim.Name} has been killed with {damage} damage";
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs stat change into chat
        public void LogStatChange(CardStats stat, PlayableCard card, sbyte value)
        {
            var message = $"{card.Name}'s {stat.GetDescription()} has been changed by {value}";
            WriteLog(message, CombatLogTypes.StatChange);
        }

        // Logs apply aura
        public void LogApplyAura(PlayableCard card, SpellData spell)
        {
            var message = $"{card.Name} is now aflicted by {spell.Name}";
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs apply aura
        public void LogExpireAura(PlayableCard card, SpellData spell)
        {
            var message = $"{spell.Name} has expired from {card.Name}";
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs heal from spell
        public void LogHeal(PlayableCard card, byte amount)
        {
            var message = $"{card.Name} has been healed by {amount}";
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs consuming mana from spell
        public void LogManaConsume(PlayableCard card, SpellData spellData, byte manaCost)
        {
            var message = $"{spellData.Name} consumes {manaCost} mana from {card.Name}";
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs mana replenishment
        public void LogManaReplenishment(Player player, byte mana)
        {
            var message = $"{player.Name}'s cards have replenished {mana} mana";
            WriteLog(message, CombatLogTypes.StatChange);
        }

        // Logs morph
        public void LogMorph(Card card, Card morph, bool isMorph)
        {
            var message = isMorph ? $"{card.Name} has been morphed to {morph.Name}" : $"{morph.Name} has been demorphed from {card.Name}";
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Returns chat color
        private Brush GetChatColor(ChatTypes chatType)
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

        // Returns combat log color
        private Brush GetCombatLogColor(CombatLogTypes combatLogType)
        {
            switch (combatLogType)
            {
                case CombatLogTypes.BasicDamage:
                    return Brushes.Red;
                case CombatLogTypes.SpellUsage:
                    return Brushes.Blue;
                case CombatLogTypes.StatChange:
                    return Brushes.Orange;
                default:
                    return Brushes.Black;
            }
        }

        // Invokes chat message to UI thread
        private void InvokeToChat(string message, RichTextBox rtb, Brush brush)
        {
            clientGame.Invoke(() =>
            {
                new TextRange(rtb.Document.ContentEnd, rtb.Document.ContentEnd)
                {
                    Text = $"{message}{CommandHandler.LineSeparator}"
                }.ApplyPropertyValue(TextElement.ForegroundProperty, brush);

                rtb.ScrollToEnd();
            });
        }
    }
}
