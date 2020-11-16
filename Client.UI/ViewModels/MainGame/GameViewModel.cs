using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Resources;
using Notifications.Wpf.Core;
using System;

namespace Client.UI.ViewModels.MainGame
{
    public class GameViewModel : NotifyPropertyViewModel
    {
        public Game Game { get; }

        public ChatViewModel ChatViewModel { get; }

        public bool GameEnabled => !Game.IsGameWaiting;

        public GameViewModel()
        {
            Game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            ChatViewModel = new (Game);

            Game.UnsubscribeAllHandlers();
            Game.MessageReceived += OnMessageReceived;
            Game.PacketProcessed += packet => OnGameEnabledChanged();
        }

        public void OnGameEnabledChanged() => OnPropertyChanged(nameof(GameEnabled));

        private async void OnMessageReceived(MessageType type, string msg)
        {
            if (type == MessageType.Error)
                await new NotificationManager().ShowAsync(new NotificationContent { Title = Texts.Error, Message = msg, Type = NotificationType.Error }, areaName: "NotificationArea");
            else if (type == MessageType.Information)
                await new NotificationManager().ShowAsync(new NotificationContent { Message = msg, Type = NotificationType.Information }, areaName: "NotificationArea");
        }
    }
}
