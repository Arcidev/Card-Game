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
        private double volume;

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

        public double Volume
        {
            get => volume;
            set
            {
                if (volume == value)
                    return;

                volume = value;
                App.GetMediaPlayer().Volume = volume / 100d;
                OnPropertyChanged();
            }
        }

        public SettingsViewModel()
        {
            AvailableLanguages = GetAvailableCultures().Select(x => new LanguageViewModel(x)).ToList();
            selectedLanguage = AvailableLanguages.FirstOrDefault(x => x.CultureInfo.Equals(CultureInfo.CurrentCulture));
            volume = App.GetMediaPlayer().Volume * 100;
        }

        public static void Cancel()
        {
            App.GetMediaPlayer().Volume = UI.Settings.Default.MusicVolume;
        }

        public void Save()
        {
            UI.Settings.Default.LanguageCode = SelectedLanguage?.CultureInfo.Name;
            UI.Settings.Default.MusicVolume = App.GetMediaPlayer().Volume;
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
                    // just ignore it
                }
            }
            return result;
        }
    }
}
