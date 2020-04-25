using Client.Logic;
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

        protected override void OnExit(ExitEventArgs e)
        {
            game?.Dispose();
            base.OnExit(e);
        }
    }
}
