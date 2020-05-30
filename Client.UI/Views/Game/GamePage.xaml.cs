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
            Initialized += GamePageInitialized;
        }

        private void GamePageInitialized(object sender, EventArgs e)
        {
            GameFrame.Navigated += GameFrame_Navigated;
        }

        private void GamePageLoaded(object sender, RoutedEventArgs e)
        {
            if (!(DataContext is GameViewModel vm))
                return;

            vm.Game.PacketProcessed += OnPacketProcessed;
        }

        private void OnPacketProcessed(UInt16 packet)
        {
            if (packet == (UInt16)SMSGPackets.AvailableCards)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    GameFrame.Navigate(new SelectCards());
                });
            }
            else if (packet == (UInt16)SMSGPackets.SelectCards)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    GameFrame.Navigate(new PlayingGame());
                });
            }
        }

        private void GameFrame_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            NavigationService.RemoveBackEntry();

            if (!(DataContext is GameViewModel vm))
                return;

            vm.ChatViewModel.AddGameChat();
            vm.OnGameEnabledChanged();
        }

        private void Frame_LoadCompleted(object sender, System.Windows.Navigation.NavigationEventArgs e) => UpdateFrameDataContext();

        private void Frame_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e) => UpdateFrameDataContext();

        private void UpdateFrameDataContext()
        {
            if (ChatFrame.Content is FrameworkElement content)
                content.DataContext = ChatFrame.DataContext;
        }
    }
}
