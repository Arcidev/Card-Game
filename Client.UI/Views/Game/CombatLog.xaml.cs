using Client.Logic.Enums;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace Client.UI.Views.Game
{
    /// <summary>
    /// Interaction logic for CombatLog.xaml
    /// </summary>
    public partial class CombatLog : Page
    {
        public CombatLog()
        {
            InitializeComponent();

            var game = App.GetGame();
            if (game == null)
                return;

            game.CombatLog.UnsubscribeAllHandlers();
            game.CombatLog.MessageReceived += OnMessageReceived;
        }

        private void OnMessageReceived(CombatLogType type, string msg)
        {
            Application.Current.Dispatcher.Invoke(() =>
            {
                new TextRange(CombatLogTextBox.Document.ContentEnd, CombatLogTextBox.Document.ContentEnd)
                {
                    Text = $"{msg}{Environment.NewLine}"
                }.ApplyPropertyValue(TextElement.ForegroundProperty, GetCombatLogColor(type));

                CombatLogTextBox.ScrollToEnd();
            });
        }

        private static SolidColorBrush GetCombatLogColor(CombatLogType combatLogType)
        {
            return combatLogType switch
            {
                CombatLogType.BasicDamage => Brushes.Red,
                CombatLogType.SpellUsage => Brushes.Blue,
                CombatLogType.StatChange => Brushes.Orange,
                _ => Brushes.Black,
            };
        }
    }
}
