using Client.Logic;
using System;

namespace Client.UI.ViewModels.Settings
{
    public class ChangePasswordViewModel : NotifyPropertyViewModel
    {
        private string errorMessage;

        public Game Game { get; }

        public string ErrorMessage
        {
            get => errorMessage;
            set
            {
                if (errorMessage == value)
                    return;

                errorMessage = value;
                OnPropertyChanged();
            }
        }

        public ChangePasswordViewModel()
        {
            Game = App.GetGame() ?? throw new InvalidOperationException("Game must exist at this point");
        }
    }
}
