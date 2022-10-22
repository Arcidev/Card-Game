using Arci.Networking.Data;
using Client.Logic.Enums;
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
        }

        private async void TabControl_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.Source is not TabControl)
                return;

            var packet = new Packet(CMSGPackets.GetUserList);
            if (FriendsTab.IsSelected)
                packet.Write((byte)UserListType.Friends);
            else if (FriendRequestsTab.IsSelected)
                packet.Write((byte)UserListType.Friends);
            else if (BlockedTab.IsSelected)
                packet.Write((byte)UserListType.Blocked);

            await App.GetGame()?.SendPacketAsync(packet);
        }
    }
}
