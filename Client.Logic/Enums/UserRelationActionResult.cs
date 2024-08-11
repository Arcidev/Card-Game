
namespace Client.Logic.Enums
{
    public enum UserRelationActionResult
    {
        Success = 0,
        UserNotFound,
        MissingFriendRequest,
        PendingFriendRequest,
        UserBlocked,
        SenderBlocked
    }
}
