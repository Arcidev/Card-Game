using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Resources;
using System;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.MainGame
{
    public class ChatWindowViewModel : NotifyPropertyViewModel
    {
        private const string friend = "friend";
        private const string game = "game";
        private const string global = "global";
        private const string help = "help";
        private const string whisper = "whisper";
        private const string block = "block";
        private const string unblock = "unblock";
        private const string clear = "clear";

        private static readonly string[] commands =
        [
            game,
            global,
            help,
            whisper,
            friend,
            block,
            unblock,
            clear
        ];

        private readonly ChatViewModel parent;
        private readonly StringBuilder text;
        private string message;
        private string picture;
        private bool pendingMessages;

        public string Name { get; }

        public ChatType ChatType { get; }

        public AsyncCommandHandler HandleChatCommandCmd { get; }

        public string Text => text.ToString();

        public bool PendingMessages
        {
            get => pendingMessages;
            set
            {
                if (pendingMessages == value)
                    return;

                pendingMessages = value;
                OnPropertyChanged();
            }
        }

        public string Picture
        {
            get => picture;
            set
            {
                if (picture == value)
                    return;

                picture = value;
                OnPropertyChanged();
            }
        }

        public string Message
        {
            get => message;
            set
            {
                if (message == value)
                    return;

                message = value;
                OnPropertyChanged();
            }
        }

        public ChatWindowViewModel(string name, ChatType chatType, ChatViewModel parent)
        {
            Name = name;
            ChatType = chatType;
            this.parent = parent;

            text = new StringBuilder();
            HandleChatCommandCmd = new (HandleChatCommand);
        }

        public void Write(string name, string msg)
        {
            text.Append(name).Append("> ").AppendLine(msg);
            OnPropertyChanged(nameof(Text));
        }

        private async Task HandleChatCommand()
        {
            if (string.IsNullOrEmpty(Message))
                return;

            if (!Message.StartsWith('/'))
                await parent.Game.Chat.SendMessage(Message, ChatType, ChatType == ChatType.Whisper ? [Name] : []);
            else
                await HandleCommand(Message[1..]);

            Message = null;
        }

        private async Task HandleCommand(string command)
        {
            if (string.IsNullOrWhiteSpace(command))
            {
                ListCommands();
                return;
            }

            int commandDelimiter = command.IndexOf(' ');
            var cmd = commandDelimiter > 0 ? command[..commandDelimiter].ToLower() : command;

            switch (commands.FirstOrDefault(x => x.StartsWith(cmd)))
            {
                case game:
                    parent.SetActiveChat(ChatType.Game, null);
                    if (commandDelimiter > 0)
                        await parent.Game.Chat.SendMessage(command[commandDelimiter..].Trim(), ChatType.Game);
                    break;
                case global:
                    parent.SetActiveChat(ChatType.Global, null);
                    if (commandDelimiter > 0)
                        await parent.Game.Chat.SendMessage(command[commandDelimiter..].Trim(), ChatType.Global);
                    break;
                case help:
                    ListCommands();
                    break;
                case whisper:
                    await HandleWhisperCommand(commandDelimiter > 0 ? command[commandDelimiter..].Trim() : "");
                    break;
                case friend:
                    await HandleFriendCommand(commandDelimiter > 0 ? command[commandDelimiter..].Trim() : "");
                    break;
                case block:
                    if (commandDelimiter > 0)
                        await HandleBlockCommand(command[commandDelimiter..].Trim(), true);
                    break;
                case unblock:
                    if (commandDelimiter > 0)
                        await HandleBlockCommand(command[commandDelimiter..].Trim(), false);
                    break;
                case clear:
                    text.Clear();
                    OnPropertyChanged(nameof(Text));
                    break;
                default:
                    HandleInvalidCommand();
                    break;
            }
        }

        private void ListCommands()
        {
            text.Append(Texts.PossibleCommands).AppendLine(":");
            foreach (var command in commands)
                text.AppendLine(command);

            OnPropertyChanged(nameof(Text));
        }

        private void HandleInvalidCommand()
        {
            text.AppendLine(Texts.InvalidCommand);
            OnPropertyChanged(nameof(Text));
        }

        private async Task HandleWhisperCommand(string arg)
        {
            void FormatSyntaxError()
            {
                text.AppendLine(string.Format(Texts.InvalidSyntax, $"/{whisper} [name] [message]"));
                OnPropertyChanged(nameof(Text));
            }

            int commandDelimiter = arg.IndexOf(' ');
            if (string.IsNullOrWhiteSpace(arg) || (commandDelimiter <= 0))
            {
                FormatSyntaxError();
                return;
            }

            string receiverName = arg[..commandDelimiter];
            string message = arg[commandDelimiter..].Trim();

            if (message.Length == 0)
            {
                FormatSyntaxError();
                return;
            }

            parent.SetActiveChat(ChatType.Whisper, receiverName, true);
            await parent.Game.Chat.SendMessage(message, ChatType.Whisper, receiverName);
        }

        private async Task HandleFriendCommand(string arg)
        {
            const string addFriend = "add";
            const string acceptFriend = "accept";
            const string denyFriend = "deny";
            const string removeFriend = "remove";

            void FormatSyntaxError()
            {
                text.AppendLine(string.Format(Texts.InvalidSyntax, $"/{friend} [{addFriend}/{acceptFriend}/{denyFriend}/{removeFriend}] [name]"));
                OnPropertyChanged(nameof(Text));
            }

            int commandDelimiter = arg.IndexOf(' ');
            if (string.IsNullOrWhiteSpace(arg) || (commandDelimiter <= 0))
            {
                FormatSyntaxError();
                return;
            }

            string command = arg[..commandDelimiter];
            string name = arg[commandDelimiter..].Trim();

            UserRelationAction action;
            switch (command.ToLower())
            {
                case addFriend:
                    action = UserRelationAction.AddFriend;
                    break;
                case acceptFriend:
                    action = UserRelationAction.AcceptFriend;
                    break;
                case denyFriend:
                    action = UserRelationAction.DenyFriend;
                    break;
                case removeFriend:
                    action = UserRelationAction.RemoveFriend;
                    break;
                default:
                    FormatSyntaxError();
                    return;
            }

            var packet = new Packet((UInt16)CMSGPackets.UserRelation).Builder()
                .Write(name).Write((byte)action).Build();

            await parent.Game.SendPacketAsync(packet);
        }

        private async Task HandleBlockCommand(string name, bool block)
        {
            if (string.IsNullOrWhiteSpace(name))
                return;

            var packet = new Packet((UInt16)CMSGPackets.UserRelation).Builder()
                .Write(name).Write((byte)(block ? UserRelationAction.BlockUser : UserRelationAction.RemoveBlockedUser)).Build();

            await parent.Game.SendPacketAsync(packet);
        }
    }
}
