using Client.UI.ViewModels.MainGame;
using System.Windows.Controls;

namespace Client.UI.Views.Game
{
    /// <summary>
    /// Interaction logic for Chat.xaml
    /// </summary>
    public partial class Chat : Page
    {
        public Chat()
        {
            InitializeComponent();
        }

        private async void TextBox_KeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Enter && sender is Control elem && elem.DataContext is ChatWindowViewModel vm)
                await vm.HandleChatCommandCmd.ExecuteAsync(null);
        }
    }
}
