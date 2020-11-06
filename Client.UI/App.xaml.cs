using Client.Logic;
using System;
using System.Globalization;
using System.Threading;
using System.Windows;

namespace Client.UI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private Game game;

        public static Game GetGame() => (Current as App)?.game;

        public static void SetGame(Game game)
        {
            if (!(Current is App app))
                return;

            if (game == app.game)
                return;

            app.game?.Dispose();
            app.game = game;
        }

        public static void SetLanguage(CultureInfo cultureInfo)
        {
            if (cultureInfo == null)
                return;

            Thread.CurrentThread.CurrentCulture = cultureInfo;
            Thread.CurrentThread.CurrentUICulture = cultureInfo;
            CultureInfo.DefaultThreadCurrentCulture = cultureInfo;
            CultureInfo.DefaultThreadCurrentUICulture = cultureInfo;
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            if (!string.IsNullOrEmpty(Settings.Default.LanguageCode))
            {
                try
                {
                    var culture = new CultureInfo(Settings.Default.LanguageCode);
                    SetLanguage(culture);
                }
                catch (Exception)
                {
                    Settings.Default.LanguageCode = null;
                    Settings.Default.Save();
                }
            }

            base.OnStartup(e);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            game?.Dispose();
            base.OnExit(e);
        }
    }
}
