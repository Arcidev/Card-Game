using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Client.Game;
using System.ComponentModel;
using Client.Enums;
using Client.UI;
using Client.Misc;

namespace Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private static int maxCardsCount = 5;
        private string userName;
        private ClientGame game;
        private int selectedCardCount;
        private bool opponentCardClicked;
        private string opponentCardControlName;

        public SlideShow SlideShow { get; private set; }

        public MainWindow()
        {
            InitializeComponent();

            userName = "";
            selectedCardCount = 0;
            opponentCardClicked = false;
            opponentCardControlName = "";
            SlideShow = new SlideShow(this);
        }

        public void Invoke(Action action)
        {
            Dispatcher.Invoke(action);
        }

        public void SetSelectCard(bool selected)
        {
            if (!SelectCardButton.IsVisible)
                SelectCardButton.Visibility = Visibility.Visible;

            SelectCardButton.Content = selected ? "Unselect" : "Select";
        }

        public void ResetCardCursor()
        {
            Cursor = Cursors.Arrow;
        }

        private void LoginButtonClick(object sender, RoutedEventArgs e)
        {
            if (!UserNameBox.Text.Any())
            {
                MessageBox.Show("Enter your user name");
                return;
            }

            LoginBox.Visibility = Visibility.Hidden;
            UserNameBox.Visibility = Visibility.Hidden;
            game = new ClientGame(userName, this);

            ShowChat(true);
        }

        private void UserNameBox_Changed(object sender, TextChangedEventArgs e)
        {
            if (UserNameBox.Text.Any(c => !Char.IsLetter(c)))
            {
                var caretIndex = UserNameBox.CaretIndex - 1;
                UserNameBox.Text = userName;
                UserNameBox.CaretIndex = caretIndex;
            }
            else
                userName = UserNameBox.Text;
        }

        private void ShowChat(bool show)
        {
            Visibility visible = show ? Visibility.Visible : Visibility.Hidden;
            ChatTabGrid.Visibility = visible;
        }

        private void MainWindow_Closing(object sender, CancelEventArgs e)
        {
            if (game != null)
                game.Dispose();
        }

        private void ChatButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(ChatTextBox.Text))
                return;

            if (ChatTextBox.Text[0] == '/')
                game.HandleCommand(ChatTextBox.Text.Substring(1));
            else
                game.SendChatMessage(ChatTextBox.Text, ChatTypes.AutoSelect);

            ChatTextBox.Text = String.Empty;
        }

        private void ChatTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
                ChatButton_Click(this, new EventArgs());
        }

        private void btnPrevious_Click(object sender, RoutedEventArgs e)
        {
            SlideShow.PreviousElements();
        }

        private void btnNext_Click(object sender, RoutedEventArgs e)
        {
            SlideShow.NextElements();
        }

        private void Images_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (Images.SelectedIndex != -1)
                SlideShow.ItemSelected(Images.SelectedItem as Card);
        }

        private void HandleSelectCardCount()
        {
            selectedCardCount += SlideShow.SelectedCard.SelectionType == SelectionType.Selected ? 1 : -1;
            if (selectedCardCount == maxCardsCount)
                SendCardsButton.Visibility = Visibility.Visible;
            else if (SendCardsButton.IsVisible)
                SendCardsButton.Visibility = Visibility.Hidden;
        }

        private void SendCardsButton_Click(object sender, RoutedEventArgs e)
        {
            if (selectedCardCount != 5)
            {
                game.Chat.Write(string.Format("You have not selected {0} cards", maxCardsCount), ChatTypes.Info);
                return;
            }

            game.SendSelectedCards();
        }

        private void SelectCardButton_Click(object sender, RoutedEventArgs e)
        {
            if (SlideShow.SelectedCard == null)
                return;

            if ((SlideShow.SelectedCard.SelectionType == SelectionType.None) && (selectedCardCount == maxCardsCount))
            {
                game.Chat.Write(string.Format("You can't select more cards than {0}", maxCardsCount), ChatTypes.Info);
                return;
            }

            SlideShow.SelectedCard.SelectionType = (SlideShow.SelectedCard.SelectionType == SelectionType.None) ? SelectionType.Selected : SelectionType.None;
            SetSelectCard(SlideShow.SelectedCard.SelectionType == SelectionType.Selected);
            SlideShow.LoadItems();
            imgMain.Source = SlideShow.SelectedCard.Image;
            HandleSelectCardCount();
        }

        private void BasicAttackButton_Click(object sender, RoutedEventArgs e)
        {
            var activeCard = game.Player.ActiveCard;
            var opponent = game.Opponent;
            Cursor = CardAttackCursors.GetCursor(activeCard.Type == CreatureTypes.Melee ? CardAttackCursorTypes.Sword : CardAttackCursorTypes.Arrow);

            opponent.SetPossibleTargets(
                activeCard.GetPossibleTargets(opponent.CardDeck, game.Player.ActiveCardPosition),
                SelectionType.BasicDamageAttackable);
        }

        private void UseSpellButton_Click(object sender, RoutedEventArgs e)
        {
            var opponent = game.Opponent;
            Cursor = CardAttackCursors.GetCursor(CardAttackCursorTypes.Staff);

            opponent.SetPossibleTargets(
                opponent.CardDeck.Select(x => x.Guid),
                SelectionType.SpellUsable);
        }

        private void DefendButton_Click(object sender, RoutedEventArgs e)
        {
            game.SendDefendSelf();
        }

        private void OnOpponentCardMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (!game.Player.IsActive)
                return;

            var image = sender as Image;
            if (image == null)
                return;

            var card = game.Opponent.GetCardByImageControlName(image.Name);
            if ((card == null) || (card.SelectionType == SelectionType.None))
                return;

            opponentCardClicked = true;
            opponentCardControlName = image.Name;
        }

        private void OnOpponentCardMouseUp(object sender, MouseButtonEventArgs e)
        {
            if (opponentCardClicked)
            {
                var card = game.Opponent.GetCardByImageControlName(opponentCardControlName);
                opponentCardClicked = false;
                opponentCardControlName = "";

                if (card != null)
                    game.SendCardAction(card);
            }
        }

        private void OnOpponentCardMouseLeave(object sender, MouseEventArgs e)
        {
            if (opponentCardClicked)
                opponentCardClicked = false;
        }

        private void OnOpponentCardMouseEnter(object sender, MouseEventArgs e)
        {
            if (!opponentCardControlName.Any())
                return;

            var image = sender as Image;
            if (image == null)
                return;

            if (opponentCardControlName == image.Name)
                opponentCardClicked = true;
        }
    }
}
