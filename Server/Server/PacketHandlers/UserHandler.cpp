#include "PacketHandler.h"
#include "Packet.h"
#include "../Player.h"
#include "../../Crypto/Sha.h"
#include "../../DataBase/PostgreSQL/DatabaseInstance.h"
#include "../../Shared/SharedDefines.h"

// Handle CMSG_USER_CREATE packet
void PacketHandler::handleUserCreatePacket(Player* player, Packet* packet)
{
    User user;
    *packet >> user.UserName >> user.Email >> user.PasswordHash;

    DEBUG_LOG("CMSG_USER_CREATE:\r\n\tUserName: %s\r\n", user.Email.c_str());
    auto [salt, hash] = Sha::CreateHash(user.PasswordHash);
    user.PasswordSalt = salt;
    user.PasswordHash = hash;
    
    DatabaseInstance::GetDbCommandHandler().CreateUser(user);

    if (!user.Id)
    {
        // error
        return;
    }

    player->SetId(user.Id);
    player->SetName(user.UserName);
}

// Handle CMSG_USER_LOGIN packet
void PacketHandler::handleUserLogInPacket(Player* player, Packet* packet)
{
    std::string email;
    std::string password;

    *packet >> email >> password;

    DEBUG_LOG("CMSG_USER_LOGIN:\r\n\tUserName: %s\r\n", email.c_str());
    User user = DatabaseInstance::GetDbCommandHandler().GetUser(email);
    if (!user.Id)
    {
        // User not found
        return;
    }

    if (Sha::CreateHash(password, user.PasswordSalt) != user.PasswordHash)
    {
        // Invalid password
        return;
    }

    player->SetId(user.Id);
    player->SetName(user.UserName);
}
