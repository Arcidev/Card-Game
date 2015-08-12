using Client.Enums;
using Client.Network;
using System;
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
            Brush brush = GetChatColor(chatType);
            RichTextBox rtb = clientGame.MainWindow.GeneralChatTab;
            InvokeToChat(message, rtb, brush);
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

        // Writes message into combat log
        public void WriteLog(string message, CombatLogTypes combatLogType)
        {
            Brush brush = GetCombatLogColor(combatLogType);
            RichTextBox rtb = clientGame.MainWindow.CombatLogChatTab;
            InvokeToChat(message, rtb, brush);
        }

        // Write info about damage into combat log chat tab
        public void LogDamage(CombatLogTypes combatLogType, PlayableCard attacker, PlayableCard victim, byte damage, bool alive)
        {
            var message = alive ? string.Format("{0} dealt {1} damage to {2}", attacker.Name, damage, victim.Name) : string.Format("{0} killed {1} with {2}", attacker.Name, victim.Name, damage);
            WriteLog(message, combatLogType);
        }

        // Write info about periodic damage into combat log chat tab
        public void LogPeriodicDamage(PlayableCard victim, byte damage, bool alive)
        {
            var message = alive ? string.Format("{0} suffered {1} damage", victim.Name, damage) : string.Format("{0} has been killed with {1} damage", victim.Name, damage);
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs stat change into chat
        public void LogStatChange(CardStats stat, PlayableCard card, sbyte value)
        {
            var message = string.Format("{0}'s {1} has been changed by {2}", card.Name, stat.GetDescription(), value);
            WriteLog(message, CombatLogTypes.StatChange);
        }

        // Logs apply aura
        public void LogApplyAura(PlayableCard card, SpellData spell)
        {
            var message = string.Format("{0} is now aflicted by {1}", card.Name, spell.Name);
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs heal from spell
        public void LogHeal(PlayableCard card, byte amount)
        {
            var message = string.Format("{0} has been healed by {1}", card.Name, amount);
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs consuming mana from spell
        public void LogManaConsume(PlayableCard card, SpellData spellData, byte manaCost)
        {
            var message = string.Format("{0} consumes {1} mana from {2}", spellData.Name, manaCost, card.Name);
            WriteLog(message, CombatLogTypes.SpellUsage);
        }

        // Logs mana replenishment
        public void LogManaReplenishment(Player player, byte mana)
        {
            var message = string.Format("{0}'s cards have replenished {1} mana", player.Name, mana);
            WriteLog(message, CombatLogTypes.StatChange);
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
            clientGame.Invoke(new Action(delegate()
            {
                TextRange tr = new TextRange(rtb.Document.ContentEnd, rtb.Document.ContentEnd);
                tr.Text = string.Format("{0}{1}", message, CommandHandler.LineSeparator);
                tr.ApplyPropertyValue(TextElement.ForegroundProperty, brush);
                rtb.ScrollToEnd();
            }));
        }
    }
}
