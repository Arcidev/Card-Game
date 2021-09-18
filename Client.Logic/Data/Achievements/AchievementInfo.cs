
namespace Client.Logic.Data.Achievements
{
    public class AchievementInfo
    {
        public string Name { get; }

        public string Image { get; }

        public AchievementInfo(string name, string image)
        {
            Name = name;
            Image = image;
        }
    }
}
