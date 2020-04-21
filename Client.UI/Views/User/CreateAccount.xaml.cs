using Client.Logic.Enums;
using Client.UI.Resources;
using Client.UI.ViewModels.User;
using Client.UI.Views.Game;
using System;
using System.Windows.Controls;

namespace Client.UI.Views.User
{
    /// <summary>
    /// Interaction logic for CreateAccount.xaml
    /// </summary>
    public partial class CreateAccount : Page
    {
        public CreateAccount()
        {
            InitializeComponent();
        }

        private async void CreateAccountBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if (!(DataContext is CreateAccountViewModel vm))
                return;

            if (Password.Password != PasswordAgain.Password)
            {
                vm.ErrorMessage = Texts.PasswordMismatch;
                return;
            }

            CreateAccountBtn.IsEnabled = false;
            var game = await vm.CreateAccount(Password.Password, (opcodeNumber, game) =>
            {
                if (opcodeNumber == (UInt16)SMSGPackets.UserResult)
                {
                    if (game.Player != null)
                        NavigationService.Navigate(new MainMenu());
                    else
                        CreateAccountBtn.IsEnabled = true;
                }
            });

            if (game == null)
                CreateAccountBtn.IsEnabled = true;
        }
    }
}
