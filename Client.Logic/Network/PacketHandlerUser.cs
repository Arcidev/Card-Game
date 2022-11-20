using Arci.Networking.Data;
using Client.Logic.Enums;
using Client.Logic.Resources;
using System.Collections.Generic;

namespace Client.Logic.Network
{
    public static partial class PacketHandler
    {
        /// <summary>
        /// Handles SMSG_USER_RESULT
        /// </summary>
        /// <param name="packet">Packet to be processed</param>
        /// <param name="game">Instance of game</param>
        private static void HandleUserResult(Packet packet, Game game)
        {
            var result = (UserResult)packet.ReadByte();

            switch (result)
            {
                case UserResult.LogedIn:
                    game.Player = new Player(packet.ReadUInt32(), packet.ReadString(), game);
                    return;
                case UserResult.PasswordChanged:
                    game.OnInformationReceived(Texts.PasswordChanged);
                    return;
                case UserResult.InvalidCredentials:
                    game.OnErrorOccured(Texts.InvalidCredentials);
                    return;
                case UserResult.EmailInUse:
                    game.OnErrorOccured(Texts.EmailInUse);
                    return;
                case UserResult.UsernameInUse:
                    game.OnErrorOccured(Texts.UsernameInUse);
                    return;
                case UserResult.UnknownError:
                    game.OnErrorOccured(Texts.UnknownError);
                    return;
            }
        }

        /// <summary>
        /// Handles SMSG_USER_LIST
        /// </summary>
        /// <param name="packet">Packet to be processed</param>
        /// <param name="game">Instance of game</param>
        private static void HandleUserList(Packet packet, Game game)
        {
            var type = (UserListType)packet.ReadByte();
            var count = packet.ReadUInt16();

            switch (type)
            {
                case UserListType.Requests:
                case UserListType.Blocked:
                    var users = new List<string>(count);
                    for (int i = 0; i < count; i++)
                        users.Add(packet.ReadString());

                    if (type == UserListType.Requests)
                        game.FriendRequets = users;
                    else
                        game.BlockedUsers = users;
                    break;
                case UserListType.Friends:
                    var onlineStatuses = new bool[count];
                    for (int i = 0; i < count; i++)
                        onlineStatuses[i] = packet.ReadBit();

                    var friends = new List<(string, bool)>(count);
                    for (int i = 0; i < count; i++)
                        friends.Add((packet.ReadString(), onlineStatuses[i]));

                    game.Friends = friends;
                    break;
            }
        }

        /// <summary>
        /// Handles SMSG_FRIEND_STATUS
        /// </summary>
        /// <param name="packet">Packet to be processed</param>
        /// <param name="game">Instance of game</param>
        private static void HandleFriendStatus(Packet packet, Game game)
        {
            var isOnline = packet.ReadBit();
            var name = packet.ReadString();

            game.OnInformationReceived(string.Format(isOnline ? Texts.FriendOnline : Texts.FriendOffline, name));
        }

        /// <summary>
        /// Handles SMSG_USER_RELATION
        /// </summary>
        /// <param name="packet">Packet to be processed</param>
        /// <param name="game">Instance of game</param>
        private static void HandleUserRelation(Packet packet, Game game)
        {
            var type = (UserRelationAction)packet.ReadByte();
            var result = (UserRelationActionResult)packet.ReadByte();
            var name = packet.ReadString();

            void Success()
            {
                switch (type)
                {
                    case UserRelationAction.AddFriend:
                        game.OnInformationReceived(string.Format(Texts.FriendRequestSent, name));
                        break;
                    case UserRelationAction.NewFriendRequest:
                        game.OnInformationReceived(string.Format(Texts.FriendRequestReceived, name));
                        break;
                    case UserRelationAction.AcceptFriend:
                        game.OnInformationReceived(string.Format(Texts.FriendRequestAccepted, name));
                        break;
                    case UserRelationAction.DenyFriend:
                        game.OnInformationReceived(string.Format(Texts.FriendRequestDenied, name));
                        break;
                    case UserRelationAction.RemoveFriend:
                        game.OnInformationReceived(string.Format(Texts.FriendRemoved, name));
                        break;
                    case UserRelationAction.BlockUser:
                        game.OnInformationReceived(string.Format(Texts.UserBlocked, name));
                        break;
                    case UserRelationAction.RemoveBlockedUser:
                        game.OnInformationReceived(string.Format(Texts.UserUnblocked, name));
                        break;
                }
            }

            switch (result)
            {
                case UserRelationActionResult.Success:
                    Success();
                    break;
                case UserRelationActionResult.UserNotFound:
                    game.OnInformationReceived(string.Format(Texts.UserNotFound, name));
                    break;
                case UserRelationActionResult.MissingFriendRequest:
                    game.OnInformationReceived(string.Format(Texts.FriendRequestMissing, name));
                    break;
                case UserRelationActionResult.UserBlocked:
                    game.OnInformationReceived(string.Format(Texts.UserBlocked, name));
                    break;
                case UserRelationActionResult.SenderBlocked:
                    game.OnInformationReceived(string.Format(Texts.SenderBlocked, name));
                    break;
            }
        }
    }
}
