using Client.Data;
using Client.Enums;
using Client.Game;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;

namespace Client.UI
{
    public class SlideShow
    {
        private static readonly int sliderSize = 10;
        private int index;
        private MainWindow mainWindow;
        private Timer timer;

        public Card SelectedCard { get; private set; }

        public SlideShow(MainWindow window)
        {
            timer = new Timer();
            timer.AutoReset = false;
            timer.Elapsed += delegate(object sender, ElapsedEventArgs e) { LoadItems(); };
            index = 0;
            mainWindow = window;
        }

        public void NextElements()
        {
            if (((index + 1) * sliderSize) < DataHolder.Cards.Count())
            {
                index++;
                RefreshImageList("FadeLstBox");
                OnClickNavigationButton();
            }
        }

        public void PreviousElements()
        {
            if ((index * sliderSize) > 0)
            {
                index--;
                RefreshImageList("FadeLstBoxBack");
                OnClickNavigationButton();
            }
        }

        private void OnClickNavigationButton()
        {
            timer.Stop();
            timer.Interval = 400;
            timer.Start();
        }

        public void LoadItems()
        {
            mainWindow.Invoke(new Action(delegate()
            {
                mainWindow.Images.DataContext = DataHolder.Cards.Skip(index * sliderSize).Take(sliderSize);
                mainWindow.Images.SelectedIndex = -1;
            }));
        }

        public void UnloadItems()
        {
            mainWindow.Invoke(new Action(delegate()
            {
                mainWindow.Images.DataContext = null;
                mainWindow.Images.SelectedIndex = -1;
                mainWindow.imgMain = null;
            }));
        }

        public void SetVisible(bool visible)
        {
            mainWindow.Invoke(new Action(delegate()
            {
                mainWindow.SlideShowGrid.Visibility = visible ? Visibility.Visible : Visibility.Hidden;
            }));
        }

        public void ItemSelected(Card card)
        {
            if (card == null)
                return;

            mainWindow.CardName.Text = string.Format("Name: {0}", card.Name);
            mainWindow.CardDamage.Text = string.Format("Damage: {0}", card.Damage);
            mainWindow.CardHitPoints.Text = string.Format("Hit points: {0}", card.Hp);
            mainWindow.CardMana.Text = string.Format("Mana: {0}", card.Mana);
            mainWindow.CardDefense.Text = string.Format("Defense: {0}", card.Defense);
            mainWindow.imgMain.Source = card.Image;

            mainWindow.SetSelectCard(card.SelectionType == SelectionType.Selected);
            SelectedCard = card;
        }

        private void RefreshImageList(string storyboardName)
        {
            Storyboard storyboard = mainWindow.FindResource(storyboardName) as Storyboard;
            if (storyboard != null)
                storyboard.Begin();
        }
    }
}
