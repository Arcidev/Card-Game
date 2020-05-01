using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Resources;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.UI.ViewModels.MainGame
{
    public class ChatWindowViewModel : NotifyPropertyViewModel
    {
        private static readonly string[] commands =
        {
            "game",
            "global",
            "help",
            "whisper"
        };

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
            HandleChatCommandCmd = new AsyncCommandHandler(HandleChatCommand);
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
                await parent.Game.Chat.SendMessage(Message, ChatType, ChatType == ChatType.Whisper ? new[] { Name } : new string[0]);
            else
                await HandleCommand(Message.Substring(1));

            Message = null;
        }

        private async Task HandleCommand(string command)
        {
            if (string.IsNullOrWhiteSpace(command))
            {
                ListCommands();
                return;
            }

            int commandDelimiter = command.IndexOf(" ");
            var cmd = commandDelimiter > 0 ? command.Substring(0, commandDelimiter).ToLower() : command;

            switch (commands.FirstOrDefault(x => x.StartsWith(cmd)))
            {
                case "game":
                    parent.SetActiveChat(ChatType.Game, null);
                    if (commandDelimiter > 0)
                        await parent.Game.Chat.SendMessage(command.Substring(commandDelimiter).Trim(), ChatType.Game);
                    break;
                case "global":
                    parent.SetActiveChat(ChatType.Global, null);
                    if (commandDelimiter > 0)
                        await parent.Game.Chat.SendMessage(command.Substring(commandDelimiter).Trim(), ChatType.Global);
                    break;
                case "help":
                    ListCommands();
                    break;
                case "whisper":
                    await HandleWhisperCommand(commandDelimiter > 0 ? command.Substring(commandDelimiter).Trim() : "");
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
                text.AppendLine(string.Format(Texts.InvalidSyntax, "/whisper [name] [message]"));
                OnPropertyChanged(nameof(Text));
            }

            int commandDelimiter = arg.IndexOf(" ");
            if (string.IsNullOrWhiteSpace(arg) || (commandDelimiter <= 0))
            {
                FormatSyntaxError();
                return;
            }

            string receiverName = arg.Substring(0, commandDelimiter);
            string message = arg.Substring(commandDelimiter).Trim();

            if (!message.Any())
            {
                FormatSyntaxError();
                return;
            }

            parent.SetActiveChat(ChatType.Whisper, receiverName, true);
            await parent.Game.Chat.SendMessage(message, ChatType.Whisper, receiverName);
        }
    }
}
