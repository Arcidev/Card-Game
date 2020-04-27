using Client.Logic.Enums;
using Client.UI.ViewModels.MainGame;
using System;
using System.Windows;
using System.Windows.Controls;

namespace Client.UI.Views.Game
{
    /// <summary>
    /// Interaction logic for GamePage.xaml
    /// </summary>
    public partial class GamePage : Page
    {
        public GamePage()
        {
            InitializeComponent();

            Loaded += GamePageLoaded;
        }

        private void GamePageLoaded(object sender, RoutedEventArgs e)
        {
            if (!(DataContext is GameViewModel vm))
                return;

            vm.Game.UnsubscribeAllHandlers();
            vm.Game.PacketProcessed += OnPacketProcessed;
        }

        private void OnPacketProcessed(UInt16 packet)
        {
            if (packet == (UInt16)SMSGPackets.AvailableCards)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    // Let's get rid of main menu from navigation bar
                    GameFrame.Navigated += GameFrame_Navigated;
                    GameFrame.Navigate(new SelectCards());
                });
            }
        }

        private void GameFrame_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            NavigationService.RemoveBackEntry();
            GameFrame.Navigated -= GameFrame_Navigated;
        }
    }
}
