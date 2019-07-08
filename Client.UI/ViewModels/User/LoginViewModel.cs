
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

        public bool Login(string password)
        {
            return false;
        }
    }
}
