using Client.Logic;
using System;
using System.Globalization;
using System.Threading;
using System.Windows;
using System.Windows.Media;

namespace Client.UI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private Game game;
        private readonly MediaPlayer mediaPlayer = new();

        public static Game GetGame() => (Current as App).game;

        public static MediaPlayer GetMediaPlayer() => (Current as App).mediaPlayer;

        public static void SetGame(Game game)
        {
            if (Current is not App app)
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

            mediaPlayer.Open(new Uri("pack://siteoforigin:,,,/Assets/Sounds/MainMenu.mp3"));
            mediaPlayer.MediaEnded += MusicPlayer_MediaEnded;
            mediaPlayer.MediaOpened += (_, _) => mediaPlayer.Volume = Settings.Default.MusicVolume;
            mediaPlayer.Play();

            base.OnStartup(e);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            mediaPlayer.Close();
            game?.Dispose();
            base.OnExit(e);
        }

        private void MusicPlayer_MediaEnded(object sender, EventArgs e)
        {
            // Loop
            mediaPlayer.Position = TimeSpan.Zero;
            mediaPlayer.Play();
        }
    }
}
