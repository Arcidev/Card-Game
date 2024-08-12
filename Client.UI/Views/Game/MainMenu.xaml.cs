using Client.UI.Views.Settings;
using System.Windows;
using System.Windows.Controls;

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

        private void LogoutButton_Click(object sender, RoutedEventArgs e) => NavigationService.GoBack();

        private void ChangePassword_Click(object sender, RoutedEventArgs e)
        {
            var changePasswordWindow = new ChangePasswordWindow();
            changePasswordWindow.ShowDialog();
        }
    }
}
