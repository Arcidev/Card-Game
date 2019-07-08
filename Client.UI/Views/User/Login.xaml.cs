using Client.UI.ViewModels.User;
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

        private void LoginBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if (!(DataContext is LoginViewModel vm))
                return;

            if (vm.Login(Password.Password))
                ; // redirect to game
        }
    }
}
