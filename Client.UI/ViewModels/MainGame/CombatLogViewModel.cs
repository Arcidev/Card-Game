using Client.Logic;
using Client.Logic.Enums;
using System.Text;

namespace Client.UI.ViewModels.MainGame
{
    public class CombatLogViewModel : NotifyPropertyViewModel
    {
        private readonly StringBuilder combatLog;

        public string CombatLog => combatLog.ToString();

        public CombatLogViewModel(Game game)
        {
            combatLog = new StringBuilder();

            game.CombatLog.UnsubscribeAllHandlers();
            game.CombatLog.MessageReceived += CombatLog_MessageReceived;
        }

        private void CombatLog_MessageReceived(CombatLogType logType, string msg)
        {
            combatLog.AppendLine(msg);
            OnPropertyChanged(nameof(CombatLog));
        }
    }
}
