using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.UI.ViewModels.User;
using System.Windows;
using System.Windows.Controls;

namespace Client.UI.Views.User
{
    /// <summary>
    /// Interaction logic for UserListWindow.xaml
    /// </summary>
    public partial class UserListWindow : Window
    {
        public UserListWindow()
        {
            InitializeComponent();
            Loaded += (_, _) =>
            {
                if (DataContext is not UserListViewModel vm)
                    return;

                vm.AddFriendCmd.OnExecuted += () => AddFriendTextBox.Clear();
                vm.BlockUserCmd.OnExecuted += () => BlockUserTextBox.Clear();
            };
        }

        private async void TabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.Source is not TabControl)
                return;

            AddFriendTextBox.Clear();
            BlockUserTextBox.Clear();

            var packet = new Packet(CMSGPackets.GetUserList);
            if (FriendsTab.IsSelected)
                packet.Write((byte)UserListType.Friends);
            else if (FriendRequestsTab.IsSelected)
                packet.Write((byte)UserListType.Requests);
            else if (BlockedTab.IsSelected)
                packet.Write((byte)UserListType.Blocked);

            await App.GetGame()?.SendPacketAsync(packet);
        }
    }
}
