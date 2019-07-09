using Client.Logic;
using System.Collections.Generic;
using System.Linq;

namespace Client.UI.ViewModels.User
{
    public class LoginViewModel : NotifyPropertyViewModel
    {
        private string errorMessage;

        public string Username { get; set; }

        public string ErrorMessage
        {
            get => errorMessage;
            set
            {
                if (errorMessage == value)
                    return;

                errorMessage = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(HasError));
            }
        }

        public bool HasError => !string.IsNullOrEmpty(ErrorMessage);

        public IEnumerable<string> ServerList => Game.Servers;

        public string Server { get; set; }

        public LoginViewModel()
        {
            Server = ServerList.FirstOrDefault();
        }

        public bool Login(string password)
        {
            return false;
        }
    }
}
