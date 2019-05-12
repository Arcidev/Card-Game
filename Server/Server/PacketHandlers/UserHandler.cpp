#include "PacketHandler.h"
#include "../Player.h"
#include "../../Crypto/Sha.h"
#include "../../DataBase/PostgreSQL/DatabaseInstance.h"
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

// Handle CMSG_USER_CREATE packet
void PacketHandler::handleUserCreatePacket(Player* player, Packet& packet)
{
    if (player->GetId())
        return;

    User user;
    packet >> user.UserName >> user.Email >> user.PasswordHash;

    DEBUG_LOG("CMSG_USER_CREATE:\r\n\tUserName: %s\r\n", user.Email.c_str());
    auto [salt, hash] = Sha::CreateHash(user.PasswordHash);
    user.PasswordSalt = salt;
    user.PasswordHash = hash;

    Packet result(SMSG_USER_RESULT);
    auto [emailInUse, userNameInUse] = DatabaseInstance::GetDbCommandHandler().CanCreateUser(user.Email, user.UserName);
    if (emailInUse)
    {
        result << (uint8_t)USER_RESULT_EMAIL_IN_USE;
        player->SendPacket(result);
        return;
    }

    if (userNameInUse)
    {
        result << (uint8_t)USER_RESULT_USERNAME_IN_USE;
        player->SendPacket(result);
        return;
    }

    DatabaseInstance::GetDbCommandHandler().CreateUser(user);
    if (!user.Id)
    {
        result << (uint8_t)USER_RESULT_UKNOWN_ERROR;
        player->SendPacket(result);
        return;
    }

    player->SetId(user.Id);
    player->SetName(user.UserName);

    result << (uint8_t)USER_RESULT_LOGGED_IN << player->GetId() << player->GetName();
    player->SendPacket(result);
}

// Handle CMSG_USER_LOGIN packet
void PacketHandler::handleUserLogInPacket(Player* player, Packet& packet)
{
    if (player->GetId())
        return;

    std::string email;
    std::string password;
    packet >> email >> password;

    DEBUG_LOG("CMSG_USER_LOGIN:\r\n\tUserName: %s\r\n", email.c_str());
    User user = DatabaseInstance::GetDbCommandHandler().GetUser(email);
    Packet result(SMSG_USER_RESULT);
    if (!user.Id || Sha::CreateHash(password, user.PasswordSalt) != user.PasswordHash)
    {
        result << (uint8_t)USER_RESULT_INVALID_CREDENTIALS;
        player->SendPacket(result);
        return;
    }

    player->SetId(user.Id);
    player->SetName(user.UserName);

    result << (uint8_t)USER_RESULT_LOGGED_IN << player->GetId() << player->GetName();
    player->SendPacket(result);
}

// Handle CMSG_USER_CHANGE_PASSWORD packet
void PacketHandler::handleUserChangePassword(Player* player, Packet& packet)
{
    std::string password;
    std::string newPassword;
    packet >> password >> newPassword;

    DEBUG_LOG("CMSG_USER_CHANGE_PASSWORD:\r\n\tId: %d\r\n", player->GetId());
    User user = DatabaseInstance::GetDbCommandHandler().GetUser(player->GetId());
    Packet result(SMSG_USER_RESULT);
    if (!user.Id || Sha::CreateHash(password, user.PasswordSalt) != user.PasswordHash)
    {
        result << (uint8_t)USER_RESULT_INVALID_CREDENTIALS;
        player->SendPacket(result);
        return;
    }

    auto[salt, hash] = Sha::CreateHash(newPassword);
    DatabaseInstance::GetDbCommandHandler().SetUserPassword(player->GetId(), hash, salt);

    result << (uint8_t)USER_RESULT_PASSWORD_CHANGED;
    player->SendPacket(result);
}
