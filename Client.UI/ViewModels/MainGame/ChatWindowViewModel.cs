using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Enums;
using Client.UI.Resources;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

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
        private string message;
        private string picture;
        private bool pendingMessages;

        public string Name { get; }

        public ChatType ChatType { get; }

        public AsyncCommandHandler HandleChatCommandCmd { get; }

        public ObservableCollection<ChatMessageViewModel> Messages { get; } = [];

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

            HandleChatCommandCmd = new (HandleChatCommand);
        }

        public void Write(string name, string msg)
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                Messages.Add(new ChatMessageViewModel(name, msg, ChatMessageType.Message));
            });
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
                    Messages.Clear();
                    break;
                default:
                    HandleInvalidCommand();
                    break;
            }
        }

        private void ListCommands()
        {
            var commandsStr = new StringBuilder();
            commandsStr.Append(Texts.PossibleCommands).Append(':');
            foreach (var command in commands)
            {
                commandsStr.AppendLine();
                commandsStr.Append(command);
            }

            Application.Current.Dispatcher.Invoke(() =>
            {
                Messages.Add(new ChatMessageViewModel(string.Empty, commandsStr.ToString(), ChatMessageType.System));
            });
        }

        private void HandleInvalidCommand()
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                Messages.Add(new ChatMessageViewModel(string.Empty, Texts.InvalidCommand, ChatMessageType.System));
            });
        }

        private Task HandleWhisperCommand(string arg)
        {
            void FormatSyntaxError()
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    Messages.Add(new ChatMessageViewModel(string.Empty, string.Format(Texts.InvalidSyntax, $"/{whisper} [name] [message]"), ChatMessageType.System));
                });
            }

            int commandDelimiter = arg.IndexOf(' ');
            if (string.IsNullOrWhiteSpace(arg) || (commandDelimiter <= 0))
            {
                FormatSyntaxError();
                return Task.CompletedTask;
            }

            string receiverName = arg[..commandDelimiter];
            string message = arg[commandDelimiter..].Trim();

            if (message.Length == 0)
            {
                FormatSyntaxError();
                return Task.CompletedTask;
            }

            parent.SetActiveChat(ChatType.Whisper, receiverName, true);
            return parent.Game.Chat.SendMessage(message, ChatType.Whisper, receiverName);
        }

        private Task HandleFriendCommand(string arg)
        {
            const string addFriend = "add";
            const string acceptFriend = "accept";
            const string denyFriend = "deny";
            const string removeFriend = "remove";

            void FormatSyntaxError()
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    Messages.Add(new ChatMessageViewModel(string.Empty, string.Format(Texts.InvalidSyntax, $"/{friend} [{addFriend}/{acceptFriend}/{denyFriend}/{removeFriend}] [name]"), ChatMessageType.System));
                });
            }

            int commandDelimiter = arg.IndexOf(' ');
            if (string.IsNullOrWhiteSpace(arg) || (commandDelimiter <= 0))
            {
                FormatSyntaxError();
                return Task.CompletedTask;
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
                    return Task.CompletedTask;
            }

            var packet = new Packet((UInt16)CMSGPackets.UserRelation).Builder()
                .Write(name).Write((byte)action).Build();

            return parent.Game.SendPacketAsync(packet);
        }

        private Task HandleBlockCommand(string name, bool block)
        {
            if (string.IsNullOrWhiteSpace(name))
                return Task.CompletedTask;

            var packet = new Packet((UInt16)CMSGPackets.UserRelation).Builder()
                .Write(name).Write((byte)(block ? UserRelationAction.BlockUser : UserRelationAction.RemoveBlockedUser)).Build();

            return parent.Game.SendPacketAsync(packet);
        }
    }
}
