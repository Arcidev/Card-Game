using System;
using System.Collections.Generic;
using System.Linq;

namespace Client.UI.ViewModels.Achievements
{
    public class AchievementsViewModel
    {
        public IEnumerable<AchievementViewModel> Achievements { get; }

        public AchievementsViewModel()
        {
            var game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
            Achievements = game.Achievements.OrderByDescending(x => x.IsCompleted).ThenByDescending(x => x.CompletionDate)
                .Select(x => new AchievementViewModel(x, game.DataHolder)).ToList();
        }
    }
}
