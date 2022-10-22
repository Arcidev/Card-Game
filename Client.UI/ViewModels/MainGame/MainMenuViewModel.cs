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

        public AsyncCommandHandler FriendsCommand { get; }

        public MainMenuViewModel()
        {
            game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            StartGameCommand = new(async () => await game.SendPacketAsync(new(CMSGPackets.StartGame)));
            AchievementsCommand = new(async () => await game.SendPacketAsync(new(CMSGPackets.GetAchievements)));
            FriendsCommand = new(async () =>
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

                var packet = new Packet(CMSGPackets.GetUserList);
                packet.Write((byte)UserListType.Friends);

                await game.SendPacketAsync(packet);
            });
        }
    }
}
