using Client.Logic.Enums;
using Client.UI.ViewModels.User;
using System;
using System.Windows.Controls;

namespace Client.UI.Views.User
{
    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    public partial class Login : Page
    {
        public Login()
        {
            InitializeComponent();
        }

        private async void LoginBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if (!(DataContext is LoginViewModel vm))
                return;

            LoginBtn.IsEnabled = false;
            var game = await vm.Login(Password.Password, (opcodeNumber) =>
            {
                if (opcodeNumber == (UInt16)SMSGPackets.UserResult)
                {
                    if (false) // if player logged in
                        ; // redirect to game
                    else
                        LoginBtn.IsEnabled = true;
                }
            });
            
            if (game == null)
                LoginBtn.IsEnabled = true;
        }
    }
}
