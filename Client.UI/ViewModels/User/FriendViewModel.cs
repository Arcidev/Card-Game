using Client.Logic;
using Client.Logic.Enums;
using Client.UI.Controls;

namespace Client.UI.ViewModels.User
{
    public class FriendViewModel : UserViewModel
    {
        public bool IsOnline { get; }

        public AsyncCommandHandler RemoveFriend { get; }

        public FriendViewModel(string name, bool isOnline, Game game) : base(name, game)
        {
            IsOnline = isOnline;
            RemoveFriend = new AsyncCommandHandler(async () => await HandleFriendRequest(UserRelationAction.RemoveFriend));
        }
    }
}
