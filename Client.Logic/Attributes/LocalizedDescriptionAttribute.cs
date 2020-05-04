using Client.Logic.Resources;
using System.ComponentModel;

namespace Client.Logic.Attributes
{
    public class LocalizedDescriptionAttribute : DescriptionAttribute
    {
        public LocalizedDescriptionAttribute(string key) : base(Localize(key)) { }

        private static string Localize(string key) => Texts.ResourceManager.GetString(key);
    }
}
