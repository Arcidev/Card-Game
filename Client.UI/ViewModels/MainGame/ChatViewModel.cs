using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Resources;
using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace Client.UI.ViewModels.MainGame
{
    public class ChatViewModel : NotifyPropertyViewModel
    {
        private ChatWindowViewModel selectedChat;

        public Game Game { get; }

        public ObservableCollection<ChatWindowViewModel> ChatWindows { get; }

        public ChatWindowViewModel ActiveChat
        {
            get => selectedChat;
            set
            {
                if (selectedChat == value)
                    return;

                selectedChat = value;
                OnPropertyChanged();
            }
        }

        public ChatViewModel()
        {
            Game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            ChatWindows = new ObservableCollection<ChatWindowViewModel>()
            {
                new ChatWindowViewModel(Texts.GlobalChat, ChatType.Global, this)
            };

            ActiveChat = ChatWindows.First();

            Game.Chat.UnsubscribeAllHandlers();
            Game.Chat.MessageReceived += Chat_MessageReceived;
        }

        private void Chat_MessageReceived(ChatType chatType, string sender, string msg)
        {
            switch(chatType)
            {
                case ChatType.Global:
                case ChatType.Game:
                    ChatWindows.FirstOrDefault(x => x.ChatType == chatType)?.Write(sender, msg);
                    break;
                case ChatType.Whisper:
                case ChatType.WhisperResponse:
                    ChatWindows.FirstOrDefault(x => x.ChatType == ChatType.Whisper && x.Name == sender)?.Write(sender, msg);
                    break;
            }
        }

        public void SetActiveChat(ChatType type, string name, bool create = false)
        {
            var chat = ChatWindows.FirstOrDefault(x => x.ChatType == type && (string.IsNullOrEmpty(name) || x.Name == name));
            if (chat == null)
            {
                if (!create)
                    return;

                chat = new ChatWindowViewModel(name, type, this);
                ChatWindows.Add(chat);
            }

            ActiveChat = chat;
        }
    }
}
