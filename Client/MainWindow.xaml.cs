﻿using System;
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
using CardGameWPF.Game;
using System.ComponentModel;
using CardGameWPF.Enums;
using CardGameWPF.UI;

namespace CardGameWPF
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

        public SlideShow SlideShow { get; private set; }

        public MainWindow()
        {
            userName = "";
            selectedCardCount = 0;
            SlideShow = new SlideShow(this);
            InitializeComponent();
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

        private void OnMouseEnterOpponentCard(object sender, MouseEventArgs e)
        {
            if (!game.Player.IsActive)
                return;

            Image image = sender as Image;
            Card card = game.Opponent.GetCardByImageControlName(image.Name);
            if ((card != null) && (card.SelectionType != SelectionType.None))
                Cursor = Cursors.Hand;
        }

        private void OnMouseLeaveOpponentCard(object sender, MouseEventArgs e)
        {
            Cursor = Cursors.Arrow;
        }
    }
}
