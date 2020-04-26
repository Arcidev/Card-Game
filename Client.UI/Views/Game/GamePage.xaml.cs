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

            Loaded += MainMenu_Loaded;
        }

        private void MainMenu_Loaded(object sender, RoutedEventArgs e)
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
                    GameFrame.Navigate(new SelectCards());
                });
            }
        }
    }
}
