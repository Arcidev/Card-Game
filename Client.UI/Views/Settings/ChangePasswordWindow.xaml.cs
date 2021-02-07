using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.UI.Resources;
using Client.UI.ViewModels.Settings;
using System.Windows;

namespace Client.UI.Views.Settings
{
    /// <summary>
    /// Interaction logic for ChangePasswordWindow.xaml
    /// </summary>
    public partial class ChangePasswordWindow : Window
    {
        public ChangePasswordWindow()
        {
            InitializeComponent();
        }

        private async void OkButton_Click(object sender, RoutedEventArgs e)
        {
            if (DataContext is not ChangePasswordViewModel vm)
                return;

            if (Password.Password != PasswordAgain.Password)
            {
                vm.ErrorMessage = Texts.PasswordMismatch;
                return;
            }

            OkButton.IsEnabled = false;
            var packet = new Packet(CMSGPackets.UserChangePassword);
            packet.Write(PasswordCurrent.Password);
            packet.Write(Password.Password);

            await vm.Game.SendPacketAsync(packet);
            DialogResult = true;
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
        }
    }
}
