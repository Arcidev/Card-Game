using Client.Logic.Enums;
using Client.UI.ViewModels.User;
using Client.UI.Views.Game;
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

            Loaded += (obj, args) => App.SetGame(null);
        }

        private async void LoginBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if (DataContext is not LoginViewModel vm)
                return;

            LoginBtn.IsEnabled = false;
            var game = await vm.Login(Password.Password, (opcodeNumber) =>
            {
                var game = App.GetGame();
                if (game == null)
                    return;

                if (opcodeNumber == (UInt16)SMSGPackets.UserResult)
                {
                    if (game.Player != null)
                        Dispatcher.Invoke(() => NavigationService.Navigate(new GamePage()));
                    else
                        Dispatcher.Invoke(() => LoginBtn.IsEnabled = true);
                }
            });
            
            if (game == null)
                LoginBtn.IsEnabled = true;
        }
    }
}
