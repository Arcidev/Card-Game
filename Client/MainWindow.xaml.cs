using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Client.Game;
using System.ComponentModel;
using Client.Enums;
using Client.UI;
using Client.Misc;
using Client.Network;

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
        private bool cardClicked;
        private string cardControlName;

        public SlideShow SlideShow { get; private set; }

        public MainWindow()
        {
            InitializeComponent();

            userName = "";
            selectedCardCount = 0;
            cardClicked = false;
            cardControlName = "";
            SlideShow = new SlideShow(this);
            ServerComboBox.ItemsSource = ClientGame.Servers;
            ServerComboBox.SelectedIndex = 0;
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

            game = ClientGame.Create(userName, ServerComboBox.SelectedItem.ToString(), this);
            if (game == null)
            {
                MessageBox.Show("Server is offline");
                return;
            }

            LoginGrid.Visibility = Visibility.Hidden;

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

            opponent.RemoveSelectionFromCards();
            opponent.SetPossibleTargets(activeCard.GetPossibleTargets(opponent.CardDeck, game.Player.ActiveCardPosition));
            game.Player.DeselectSpellFriendlyTargets();
        }

        private void UseSpellButton_Click(object sender, RoutedEventArgs e)
        {
            if (game.SetPossibleSpellTargets())
                Cursor = CardAttackCursors.GetCursor(CardAttackCursorTypes.Staff);
        }

        private void DefendButton_Click(object sender, RoutedEventArgs e)
        {
            game.SendDefendSelf();
        }

        private void OnCardMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (!game.Player.IsActive)
                return;

            var image = sender as Image;
            if (image == null)
                return;

            var card = game.GetCardByImageControlName(image.Name);
            if ((card == null) || ((card.SelectionType != SelectionType.BasicDamageAttackable) && (card.SelectionType != SelectionType.SpellUsable)))
                return;

            cardClicked = true;
            cardControlName = image.Name;
        }

        private void OnCardMouseUp(object sender, MouseButtonEventArgs e)
        {
            if (cardClicked)
            {
                var card = game.GetCardByImageControlName(cardControlName);
                cardClicked = false;
                cardControlName = "";

                if (card != null)
                    game.SendCardAction(card);
            }
        }

        private void OnCardMouseLeave(object sender, MouseEventArgs e)
        {
            if (cardClicked)
                cardClicked = false;
        }

        private void OnCardMouseEnter(object sender, MouseEventArgs e)
        {
            if (!cardControlName.Any())
                return;

            var image = sender as Image;
            if (image == null)
                return;

            if (cardControlName == image.Name)
                cardClicked = true;
        }
    }
}
