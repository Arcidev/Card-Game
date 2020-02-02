#include "PacketHandler.h"
#include "../ConnectedUser.h"
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

// Handle CMSG_USER_CREATE packet
void PacketHandler::handleUserCreatePacket(ConnectedUser* cUser, Packet& packet)
{
    User user;
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

    cUser->SetDatabaseUserId(user.Id);
    cUser->SetName(user.UserName);

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
    User user = DatabaseInstance::GetDbCommandHandler().GetUser(email);
    Packet result(SMSGPackets::SMSG_USER_RESULT);
    if (!user.Id || Sha::CreateHash(password, user.PasswordSalt) != user.PasswordHash)
    {
        result << (uint8_t)USER_RESULT_INVALID_CREDENTIALS;
        cUser->SendPacket(result);
        return;
    }

    cUser->SetDatabaseUserId(user.Id);
    cUser->SetName(user.UserName);

    result << (uint8_t)USER_RESULT_LOGGED_IN << cUser->GetId() << cUser->GetName();
    cUser->SendPacket(result);
}

// Handle CMSG_USER_CHANGE_PASSWORD packet
void PacketHandler::handleUserChangePassword(ConnectedUser* cUser, Packet& packet)
{
    std::string password;
    std::string newPassword;
    packet >> password >> newPassword;

    DEBUG_LOG("CMSG_USER_CHANGE_PASSWORD:\r\n\tId: %d\r\n", cUser->GetId());
    User user = DatabaseInstance::GetDbCommandHandler().GetUser(cUser->GetId());
    Packet result(SMSGPackets::SMSG_USER_RESULT);
    if (!user.Id || Sha::CreateHash(password, user.PasswordSalt) != user.PasswordHash)
    {
        result << (uint8_t)USER_RESULT_INVALID_CREDENTIALS;
        cUser->SendPacket(result);
        return;
    }

    auto[salt, hash] = Sha::CreateHash(newPassword);
    DatabaseInstance::GetDbCommandHandler().SetUserPassword(cUser->GetId(), salt, hash);

    result << (uint8_t)USER_RESULT_PASSWORD_CHANGED;
    cUser->SendPacket(result);
}
