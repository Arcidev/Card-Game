
namespace Client.UI.ViewModels.User
{
    public class CreateAccountViewModel : LoginViewModel
    {
        public string Email { get; set; }

        public bool CreateAccount(string password)
        {
            return false;
        }
    }
}
