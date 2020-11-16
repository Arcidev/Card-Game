using Client.UI.Resources;
using Client.UI.ViewModels.MainGame;
using System.Windows;
using System.Windows.Controls;

namespace Client.UI.Views.Game
{
    /// <summary>
    /// Interaction logic for PlayingGame.xaml
    /// </summary>
    public partial class PlayingGame : Page
    {
        private bool loaded;

        public PlayingGame()
        {
            InitializeComponent();

            Loaded += PlayingGame_Loaded;
        }

        private void PlayingGame_Loaded(object sender, RoutedEventArgs e)
        {
            if (loaded || DataContext is not PlayingGameViewModel vm)
                return;

            loaded = true;
            vm.Game.GameEnded += Game_GameEnded;
        }

        private void Game_GameEnded(bool hasWon)
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                MessageBox.Show(hasWon ? Texts.WonGame : Texts.LostGame, string.Empty, MessageBoxButton.OK, MessageBoxImage.None);

                NavigationService.Navigate(new MainMenu());
            });
        }
    }
}
