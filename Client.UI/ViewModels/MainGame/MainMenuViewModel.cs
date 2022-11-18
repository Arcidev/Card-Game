using Arci.Networking.Data;
using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;
using Client.UI.Views.User;
using System;

namespace Client.UI.ViewModels.MainGame
{
    public class MainMenuViewModel : NotifyPropertyViewModel
    {
        private readonly Game game;
        private UserListWindow usersWindow;

        public AsyncCommandHandler StartGameCommand { get; }

        public AsyncCommandHandler AchievementsCommand { get; }

        public CommandHandler FriendsCommand { get; }

        public MainMenuViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            StartGameCommand = new(async () => await game.SendPacketAsync(new(CMSGPackets.StartGame)));
            AchievementsCommand = new(async () => await game.SendPacketAsync(new(CMSGPackets.GetAchievements)));
            FriendsCommand = new(() =>
            {
                if (usersWindow != null)
                {
                    usersWindow.Focus();
                    return;
                }

                usersWindow = new UserListWindow();
                usersWindow.Closed += (obj, args) =>
                {
                    (usersWindow.DataContext as IDisposable)?.Dispose();
                    usersWindow = null;
                };
                usersWindow.Show();
            });
        }
    }
}
