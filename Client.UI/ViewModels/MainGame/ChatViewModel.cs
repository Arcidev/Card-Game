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
        private ChatWindowViewModel activeChat;

        public Game Game { get; }

        public ObservableCollection<ChatWindowViewModel> ChatWindows { get; }

        public ChatWindowViewModel ActiveChat
        {
            get => activeChat;
            set
            {
                if (activeChat == value)
                    return;

                activeChat = value;
                OnPropertyChanged();

                if (activeChat != null)
                    activeChat.PendingMessages = false;
            }
        }

        public ChatViewModel()
        {
            Game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            ChatWindows = new ObservableCollection<ChatWindowViewModel>()
            {
                new ChatWindowViewModel(Texts.GlobalChat, ChatType.Global, this) { Picture = "/Images/GeneralChat.PNG" }
            };

            ActiveChat = ChatWindows.First();

            Game.Chat.UnsubscribeAllHandlers();
            Game.Chat.MessageReceived += Chat_MessageReceived;
        }

        private void Chat_MessageReceived(ChatType chatType, string sender, string msg)
        {
            ChatWindowViewModel chatVm;
            switch(chatType)
            {
                case ChatType.Global:
                case ChatType.Game:
                    chatVm = ChatWindows.FirstOrDefault(x => x.ChatType == chatType);
                    break;
                case ChatType.Whisper:
                case ChatType.WhisperResponse:
                    chatVm = ChatWindows.FirstOrDefault(x => x.ChatType == ChatType.Whisper && string.Compare(x.Name, sender, true) == 0);
                    if (chatVm == null)
                    {
                        SetActiveChat(ChatType.Whisper, sender, true);
                        chatVm = activeChat;
                    }
                    break;
                default:
                    chatVm = null;
                    break;
            }

            if (chatVm == null)
                return;

            chatVm.Write(sender, msg);
            if (chatVm != ActiveChat)
                chatVm.PendingMessages = true;
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
