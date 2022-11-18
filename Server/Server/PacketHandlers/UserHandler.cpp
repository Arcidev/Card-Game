#include "PacketHandler.h"
#include "../ConnectedUser.h"
#include "../ServerNetwork.h"
#include "../../Crypto/Sha.h"
#include "../../Database/DatabaseInstance.h"
#include "../../Shared/SharedDefines.h"

enum UserResults
{
    USER_RESULT_LOGGED_IN = 0,
    USER_RESULT_PASSWORD_CHANGED,
    USER_RESULT_EMAIL_IN_USE,
    USER_RESULT_USERNAME_IN_USE,
    USER_RESULT_INVALID_CREDENTIALS,
    USER_RESULT_UKNOWN_ERROR
};

enum UserListType
{
    USER_LIST_REQUESTS = 0,
    USER_LIST_FRIENDS,
    USER_LIST_BLOCKED
};

enum UserRelationAction
{
    USER_RELATION_ACTION_ADD_FRIEND = 0,
    USER_RELATION_ACTION_ACCEPT_FRIEND,
    USER_RELATION_ACTION_DENY_FRIEND,
    USER_RELATION_ACTION_REMOVE_FRIEND,
    USER_RELATION_ACTION_BLOCK_USER,
    USER_RELATION_ACTION_REMOVE_BLOCKED_USER
};

enum UserRelationActionResult
{
    USER_RELATION_ACTION_RESULT_SUCCESS = 0,
    USER_RELATION_ACTION_RESULT_USER_NOT_FOUND,
    USER_RELATION_ACTION_RESULT_MISSING_FRIEND_REQUEST,
    USER_RELATION_ACTION_RESULT_USER_BLOCKED,
    USER_RELATION_ACTION_RESULT_SENDER_BLOCKED
};

// Handle CMSG_USER_CREATE packet
void PacketHandler::handleUserCreatePacket(ConnectedUser* cUser, Packet& packet)
{
    Db::User user;
    packet >> user.UserName >> user.Email >> user.PasswordHash;

    DEBUG_LOG("CMSG_USER_CREATE:\r\n\tEmail: %s\r\n", user.Email.c_str());

    Packet result(SMSGPackets::SMSG_USER_RESULT);
    auto [emailInUse, userNameInUse] = DatabaseInstance::GetDbCommandHandler().CanCreateUser(user.Email, user.UserName);
    if (emailInUse)
    {
        result << (uint8_t)USER_RESULT_EMAIL_IN_USE;
        cUser->SendPacket(result);
        return;
    }

    if (userNameInUse)
    {
        result << (uint8_t)USER_RESULT_USERNAME_IN_USE;
        cUser->SendPacket(result);
        return;
    }

    auto [salt, hash] = Sha::CreateHash(user.PasswordHash);
    user.PasswordSalt = salt;
    user.PasswordHash = hash;

    DatabaseInstance::GetDbCommandHandler().CreateUser(user);
    if (!user.Id)
    {
        result << (uint8_t)USER_RESULT_UKNOWN_ERROR;
        cUser->SendPacket(result);
        return;
    }

    cUser->SetName(user.UserName);
    cUser->OnLoggedIn(user.Id);

    result << (uint8_t)USER_RESULT_LOGGED_IN << cUser->GetId() << cUser->GetName();
    cUser->SendPacket(result);
}

// Handle CMSG_USER_LOGIN packet
void PacketHandler::handleUserLogInPacket(ConnectedUser* cUser, Packet& packet)
{
    std::string email;
    std::string password;
    packet >> email >> password;

    DEBUG_LOG("CMSG_USER_LOGIN:\r\n\tEmail: %s\r\n", email.c_str());
    Db::User user = DatabaseInstance::GetDbCommandHandler().GetUser(email);
    Packet result(SMSGPackets::SMSG_USER_RESULT);
    if (!user.Id || Sha::CreateHash(password, user.PasswordSalt) != user.PasswordHash)
    {
        result << (uint8_t)USER_RESULT_INVALID_CREDENTIALS;
        cUser->SendPacket(result);
        return;
    }

    cUser->SetName(user.UserName);
    cUser->OnLoggedIn(user.Id);

    result << (uint8_t)USER_RESULT_LOGGED_IN << cUser->GetId() << cUser->GetName();
    cUser->SendPacket(result);
}

// Handle CMSG_USER_CHANGE_PASSWORD packet
void PacketHandler::handleUserChangePassword(ConnectedUser* cUser, Packet& packet)
{
    DEBUG_LOG("CMSG_USER_CHANGE_PASSWORD:\r\n\tId: %d\r\n", cUser->GetId());

    std::string password;
    std::string newPassword;
    packet >> password >> newPassword;

    Db::User user = DatabaseInstance::GetDbCommandHandler().GetUser(cUser->GetDatabaseId());
    Packet result(SMSGPackets::SMSG_USER_RESULT);
    if (!user.Id || Sha::CreateHash(password, user.PasswordSalt) != user.PasswordHash)
    {
        result << (uint8_t)USER_RESULT_INVALID_CREDENTIALS;
        cUser->SendPacket(result);
        return;
    }

    auto[salt, hash] = Sha::CreateHash(newPassword);
    DatabaseInstance::GetDbCommandHandler().SetUserPassword(cUser->GetDatabaseId(), salt, hash);

    result << (uint8_t)USER_RESULT_PASSWORD_CHANGED;
    cUser->SendPacket(result);
}

void PacketHandler::handleGetUserListPacket(ConnectedUser* cUser, Packet& packet)
{
    uint8_t listType;
    packet >> listType;

    DEBUG_LOG("CMST_GET_USER_LIST:\r\n\tId: %d\r\n\tList Type: %d\r\n", cUser->GetId(), listType);
    DbCommandHandler const& dbHandler = DatabaseInstance::GetDbCommandHandler();
    Packet result(SMSGPackets::SMSG_USER_LIST);
    result << listType;

    switch (listType)
    {
        case USER_LIST_REQUESTS:
        {
            std::list<Db::UserRelation> userList = listType == USER_LIST_REQUESTS ? dbHandler.GetFriendRequests(cUser->GetDatabaseId()) : dbHandler.GetBlockedUsers(cUser->GetDatabaseId());
            result << (uint16_t)userList.size();
            for (Db::UserRelation const& user : userList)
                result << user.UserName;
            break;
        }
        case USER_LIST_FRIENDS:
            result << (uint16_t)cUser->GetFriends().size();
            for (Friend const& userFriend : cUser->GetFriends())
                result.WriteBit(userFriend.IsOnline());

            result.FlushBits();
            for (Friend const& userFriend : cUser->GetFriends())
                result << userFriend.GetName();
            break;
        case USER_LIST_BLOCKED:
            result << (uint16_t)cUser->GetBlockedUsers().size();
            for (auto const& [id, name] : cUser->GetBlockedUsers())
                result << name;
            break;
        default:
            return;
    }

    cUser->SendPacket(result);
}

void PacketHandler::handleUserRelationPacket(ConnectedUser* cUser, Packet& packet)
{
    std::string name;
    uint8_t type;
    packet >> name >> type;

    DEBUG_LOG("CMSG_USER_RELATION:\r\n\tId: %d\r\n\tType: %d\r\n\tName: %s\r\n", cUser->GetId(), type, name.c_str());
    DbCommandHandler const& dbHandler = DatabaseInstance::GetDbCommandHandler();
    uint32_t id = dbHandler.GetUserId(name);

    uint8_t result;
    if (id > 0)
    {
        result = USER_RELATION_ACTION_RESULT_SUCCESS;
        switch (type)
        {
            case USER_RELATION_ACTION_ADD_FRIEND:
                if (dbHandler.IsUserBlocked(id, cUser->GetDatabaseId()))
                    result = USER_RELATION_ACTION_RESULT_SENDER_BLOCKED;
                else if (dbHandler.IsUserBlocked(cUser->GetDatabaseId(), id))
                    result = USER_RELATION_ACTION_RESULT_USER_BLOCKED;
                else
                    dbHandler.SetFriendRequest(cUser->GetDatabaseId(), id);
                break;
            case USER_RELATION_ACTION_ACCEPT_FRIEND:
                if (dbHandler.HasFriendRequest(id, cUser->GetDatabaseId()))
                {
                    dbHandler.RemoveFriendRequest(cUser->GetDatabaseId(), id);
                    dbHandler.SetFriend(cUser->GetDatabaseId(), id);
                    cUser->AddFriend(id, name);

                    if (ConnectedUser* user = cUser->GetNetwork()->GetUser(id))
                    {
                        user->AddFriend(cUser->GetDatabaseId(), cUser->GetName());
                        Packet friendPacket(SMSGPackets::SMSG_USER_RESULT);
                        friendPacket << type << result << cUser->GetName();
                        user->SendPacket(friendPacket);
                    }
                }
                else
                    result = USER_RELATION_ACTION_RESULT_MISSING_FRIEND_REQUEST;
                break;
            case USER_RELATION_ACTION_DENY_FRIEND:
                dbHandler.RemoveFriendRequest(cUser->GetDatabaseId(), id);
                break;
            case USER_RELATION_ACTION_REMOVE_FRIEND:
                dbHandler.RemoveFriend(cUser->GetDatabaseId(), id);
                cUser->RemoveFriend(id);
                if (ConnectedUser* user = cUser->GetNetwork()->GetUser(id))
                    user->RemoveFriend(cUser->GetDatabaseId());
                break;
            case USER_RELATION_ACTION_BLOCK_USER:
                dbHandler.SetBlockedUser(cUser->GetDatabaseId(), id);
                cUser->BlockUser(id, name);
                break;
            case USER_RELATION_ACTION_REMOVE_BLOCKED_USER:
                dbHandler.RemoveBlockedUser(cUser->GetDatabaseId(), id);
                cUser->RemoveBlockedUser(id);
                break;
        }
    }
    else
        result = USER_RELATION_ACTION_RESULT_USER_NOT_FOUND;

    Packet response(SMSGPackets::SMSG_USER_RELATION);
    response << type << result << name;

    cUser->SendPacket(response);
}
