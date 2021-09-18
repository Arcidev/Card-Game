using Client.Logic.Enums;
using Client.UI.ViewModels.MainGame;
using Client.UI.Views.Achievement;
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
        private bool loaded = false;

        public GamePage()
        {
            InitializeComponent();

            Loaded += GamePageLoaded;
        }

        private void GamePageLoaded(object sender, RoutedEventArgs e)
        {
            if (loaded || DataContext is not GameViewModel vm)
                return;

            loaded = true;
            vm.Game.PacketProcessed += OnPacketProcessed;
            GameFrame.Navigated += GameFrame_Navigated;
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
            else if (packet == (UInt16)SMSGPackets.Achievements)
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    GameFrame.Navigate(new AchievementsPage());
                });
            }
        }

        private void GameFrame_Navigated(object sender, System.Windows.Navigation.NavigationEventArgs e)
        {
            NavigationService.RemoveBackEntry();
            if (DataContext is not GameViewModel vm)
                return;

            vm.ChatViewModel.AddGameChat();
            vm.OnGameEnabledChanged();
        }

        private void Frame_LoadCompleted(object sender, System.Windows.Navigation.NavigationEventArgs e) => UpdateFrameDataContext(sender);

        private void Frame_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e) => UpdateFrameDataContext(sender);

        private static void UpdateFrameDataContext(object sender)
        {
            if (sender is Frame frame && frame.Content is FrameworkElement content)
                content.DataContext = frame.DataContext;
        }
    }
}
