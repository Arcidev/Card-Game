using Client.UI.Utilities;
using Client.UI.Views.Settings;
using Client.UI.Views.User;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace Client.UI.Views.Game
{
    /// <summary>
    /// Interaction logic for MainMenu.xaml
    /// </summary>
    public partial class MainMenu : Page
    {
        public MainMenu()
        {
            InitializeComponent();
            Loaded += (_, _) => NavigationService.RemoveBackEntry();
        }

        private void LogoutButton_Click(object sender, RoutedEventArgs e)
        {
            this.FindParent<GamePage>()?.NavigationService.Navigate(new Login());
        }

        private void ChangePassword_Click(object sender, RoutedEventArgs e)
        {
            var changePasswordWindow = new ChangePasswordWindow();
            changePasswordWindow.ShowDialog();
        }
    }
}
