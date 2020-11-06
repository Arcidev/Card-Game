using Client.UI.Resources;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Resources;

namespace Client.UI.ViewModels.Settings
{
    public class SettingsViewModel : NotifyPropertyViewModel
    {
        private LanguageViewModel selectedLanguage;

        public List<LanguageViewModel> AvailableLanguages { get; }

        public bool IsModified { get; private set; }

        public bool LanguageModified => !(SelectedLanguage?.CultureInfo.Equals(CultureInfo.CurrentCulture) ?? true);

        public LanguageViewModel SelectedLanguage
        {
            get => selectedLanguage;
            set
            {
                if (selectedLanguage == value)
                    return;

                selectedLanguage = value;
                OnPropertyChanged();

                IsModified = true;
            }
        }

        public SettingsViewModel()
        {
            AvailableLanguages = GetAvailableCultures().Select(x => new LanguageViewModel(x)).ToList();
            selectedLanguage = AvailableLanguages.FirstOrDefault(x => x.CultureInfo.Equals(CultureInfo.CurrentCulture));
        }

        public void Save()
        {
            UI.Settings.Default.LanguageCode = SelectedLanguage?.CultureInfo.Name;
            UI.Settings.Default.Save();

            if (LanguageModified)
                App.SetLanguage(SelectedLanguage.CultureInfo);
        }

        private static IEnumerable<CultureInfo> GetAvailableCultures()
        {
            var result = new List<CultureInfo>();

            var rm = new ResourceManager(typeof(Texts));
            var cultures = CultureInfo.GetCultures(CultureTypes.AllCultures);
            foreach (var culture in cultures)
            {
                try
                {
                    if (rm.GetResourceSet(culture, true, false) != null)
                        result.Add(culture);
                }
                catch (CultureNotFoundException)
                {

                }
            }
            return result;
        }
    }
}
