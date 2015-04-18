using Client.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

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
        public static string LineSeparator { get { return "\u2028"; } }

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

            for (int i = 0; i < commands.Length; i++ )
            {
                if (commands[i].StartsWith(cmd))
                {
                    cmd = commands[i];
                    break;
                }
            }

            switch (cmd)
            {
                case "game":
                    game.Chat.SetActiveChat(ChatTypes.Game);
                    if (commandDelimiter > 0)
                        game.SendChatMessage(command.Substring(commandDelimiter).Trim(), ChatTypes.Game);
                    break;
                case "global":
                    game.Chat.SetActiveChat(ChatTypes.Global);
                    if (commandDelimiter > 0)
                        game.SendChatMessage(command.Substring(commandDelimiter).Trim(), ChatTypes.Global);
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
            game.Chat.Write(string.Format("Invalid syntax. Use: {0}", correctSyntax), ChatTypes.Info);
        }

        // Writes invalid command into chat
        private static void WriteInvalidCommand(ClientGame game)
        {
            game.Chat.Write("Invalid command", ChatTypes.Info);
        }

        // Lists all commands in chat
        private static void ListCommands(ClientGame game)
        {
            game.Chat.Write(string.Format("Possible commands:{0}{1}", LineSeparator, string.Join(LineSeparator, commands)), ChatTypes.Info);
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

            game.SendChatMessage(message, ChatTypes.Whisper, receiverName);
        }
    }
}
