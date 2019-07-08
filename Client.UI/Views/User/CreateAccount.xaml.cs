using Client.UI.Resources;
using Client.UI.ViewModels.User;
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

        private void CreateAccountBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if (!(DataContext is CreateAccountViewModel vm))
                return;

            if (Password.Password != PasswordAgain.Password)
            {
                vm.ErrorMessage = Texts.PasswordMismatch;
                return;
            }

            if (vm.CreateAccount(Password.Password))
                ; // redirect to game
        }
    }
}
