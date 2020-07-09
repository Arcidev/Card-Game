using Client.UI.ViewModels.Cards;
using System.Windows.Controls;

namespace Client.UI.Controls
{
    /// <summary>
    /// Interaction logic for CardControl.xaml
    /// </summary>
    public partial class CardControl : UserControl
    {
        public CardControl()
        {
            InitializeComponent();
        }

        private void Grid_PreviewMouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (!(DataContext is CardViewModel vm))
                return;

            vm.OnClickCmd?.Execute(vm);
        }
    }
}
