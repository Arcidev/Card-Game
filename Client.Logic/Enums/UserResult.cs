
namespace Client.Logic.Enums
{
    public enum UserResult
    {
        LogedIn = 0,
        PasswordChanged,
        EmailInUse,
        UserNameInUse,
        InvalidCredentials,
        UnknownError
    }
}
