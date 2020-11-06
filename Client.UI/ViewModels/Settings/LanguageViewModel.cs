using System.Globalization;

namespace Client.UI.ViewModels.Settings
{
    public class LanguageViewModel : NotifyPropertyViewModel
    {
        public CultureInfo CultureInfo { get; }

        public string Name
        {
            get
            {
                if (CultureInfo.InvariantCulture.Equals(CultureInfo))
                    return new CultureInfo("en-US").NativeName;

                return CultureInfo.NativeName;
            }
        }

        public LanguageViewModel(CultureInfo culture)
        {
            CultureInfo = culture;
        }
    }
}
