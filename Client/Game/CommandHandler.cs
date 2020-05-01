using Client.Logic.Enums;
using System.Linq;

namespace Client.Game
{
    public static class CommandHandler
    {
        private static readonly string[] commands = 
        { 
            "game",
            "global",
            "help",
            "whisper"
        };

        // Line separator
        public static string LineSeparator => "\u2028";

        // Handle command written in chat
        public static void HandleCommand(string command, ClientGame game)
        {
            if (string.IsNullOrWhiteSpace(command))
            {
                ListCommands(game);
                return;
            }

            int commandDelimiter = command.IndexOf(" ");
            var cmd = commandDelimiter > 0 ? command.Substring(0, commandDelimiter).ToLower() : command;

            switch (commands.FirstOrDefault(x => x.StartsWith(cmd)))
            {
                case "game":
                    game.Chat.SetActiveChat(ChatType.Game);
                    if (commandDelimiter > 0)
                        game.SendChatMessage(command.Substring(commandDelimiter).Trim(), ChatType.Game);
                    break;
                case "global":
                    game.Chat.SetActiveChat(ChatType.Global);
                    if (commandDelimiter > 0)
                        game.SendChatMessage(command.Substring(commandDelimiter).Trim(), ChatType.Global);
                    break;
                case "help":
                    ListCommands(game);
                    break;
                case "whisper":
                    HandleWhisperCommand(game, commandDelimiter > 0 ? command.Substring(commandDelimiter).Trim() : "");
                    break;
                default:
                    WriteInvalidCommand(game);
                    break;
            }
        }

        // Writes syntax error into chat
        private static void FormatSyntaxError(ClientGame game, string correctSyntax)
        {
            game.Chat.Write($"Invalid syntax. Use: {correctSyntax}", ChatType.Info);
        }

        // Writes invalid command into chat
        private static void WriteInvalidCommand(ClientGame game)
        {
            game.Chat.Write("Invalid command", ChatType.Info);
        }

        // Lists all commands in chat
        private static void ListCommands(ClientGame game)
        {
            game.Chat.Write($"Possible commands:{LineSeparator}{string.Join(LineSeparator, commands)}", ChatType.Info);
        }

        // Sends whisper message to server
        private static void HandleWhisperCommand(ClientGame game, string arg)
        {
            int commandDelimiter = arg.IndexOf(" ");
            if (string.IsNullOrWhiteSpace(arg) || (commandDelimiter <= 0))
            {
                FormatSyntaxError(game, "/whisper [name] [message]");
                return;
            }

            string receiverName = arg.Substring(0, commandDelimiter);
            string message = arg.Substring(commandDelimiter).Trim();

            if (!message.Any())
            {
                FormatSyntaxError(game, "/whisper [name] [message]");
                return;
            }

            game.SendChatMessage(message, ChatType.Whisper, receiverName);
        }
    }
}
