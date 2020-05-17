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
        }

        private void LogoutButton_Click(object sender, System.Windows.RoutedEventArgs e) => NavigationService.GoBack();
    }
}
