using Client.Logic.Data;
using Client.Logic.Data.Achievements;
using System.Collections.Generic;
using System.Linq;

namespace Client.UI.ViewModels.Achievements
{
    public class AchievementViewModel
    {
        private readonly Achievement achievement;
        private readonly AchievementInfo achievementInfo;

        public string Name => achievementInfo.Name;

        public string Image => $"/Assets/Images/Achievements/{achievementInfo.Image}";

        public bool IsCompleted => achievement.IsCompleted;

        public IEnumerable<CriteriaViewModel> Criterias { get; }

        public AchievementViewModel(Achievement achievement, IDataHolder dataHolder)
        {
            this.achievement = achievement;
            achievementInfo = dataHolder.GetAchievementInfo(achievement.Id);
            Criterias = achievement.Criterias.Select(x => new CriteriaViewModel(x, dataHolder)).ToList();
        }
    }
}
