using Client.UI.Resources;
using Client.UI.ViewModels.Settings;
using Client.UI.Views.User;
using System.Windows;
using System.Windows.Controls;

namespace Client.UI.Views.Settings
{
    /// <summary>
    /// Interaction logic for SettingsPage.xaml
    /// </summary>
    public partial class SettingsPage : Page
    {
        private SettingsViewModel ViewModel => DataContext as SettingsViewModel;

        public SettingsPage()
        {
            InitializeComponent();
        }

        private void Save_Click(object sender, RoutedEventArgs e)
        {
            ViewModel?.Save();
            NavigationService.Navigate(new Login());
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            var vm = ViewModel;
            if (vm != null)
            {
                if (vm.IsModified)
                {
                    var result = MessageBox.Show(Texts.SettingsModified, string.Empty, MessageBoxButton.YesNoCancel, MessageBoxImage.Question);
                    if (result == MessageBoxResult.Cancel)
                    {
                        SettingsViewModel.Cancel();
                        return;
                    }
                    if (result == MessageBoxResult.Yes)
                        vm.Save();
                }
            }

            NavigationService.Navigate(new Login());
        }
    }
}
